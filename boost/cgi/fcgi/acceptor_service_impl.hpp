//           -- fcgi/acceptor_service_impl.hpp --
//
//            Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_FCGI_ACCEPTOR_SERVICE_IMPL_HPP_INCLUDED__
#define CGI_FCGI_ACCEPTOR_SERVICE_IMPL_HPP_INCLUDED__

#include "boost/cgi/detail/push_options.hpp"

#include <boost/ref.hpp>
#include <boost/static_assert.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/system/error_code.hpp>
///////////////////////////////////////////////////////////
#include "boost/cgi/common/io_service.hpp"
#include "boost/cgi/detail/throw_error.hpp"
#include "boost/cgi/detail/protocol_traits.hpp"
#include "boost/cgi/basic_protocol_service_fwd.hpp"
#include "boost/cgi/detail/service_base.hpp"
#include "boost/cgi/fcgi/request.hpp"

namespace cgi {
   
   namespace detail {

     /// Helper functions for async_accept operation.
     template<typename T, typename Handler>
     struct accept_handler
     {
       accept_handler(T& t, typename T::implementation_type& impl
                     , typename T::implementation_type::request_type& req
                     , Handler& hnd)
         : type(t)
         , implementation(impl)
         , request(req)
         , handler(hnd)
       {}

       void operator()()
       {
         type.check_for_waiting_request(implementation, request, handler);
       }

       T& type;
       typename T::implementation_type& implementation;
       typename T::implementation_type::request_type& request;
       Handler handler;
     };
  
   } // namespace detail

  namespace fcgi {

  /// The service_impl class for FCGI basic_request_acceptor<>s
   /**
    * Note: this is near enough to being generic. It will hopefully translate
    *       directly to the fcgi_acceptor_service_impl. In other words you would
    *       then have one acceptor_service_impl<>, so you'd use
    *       acceptor_service_impl<scgi> acceptor_service_impl_; // and
    *       acceptor_service_impl<fcgi> acceptor_service_impl_; // etc...
    *
    * Note: If the protocol is an asynchronous protocol, which means it requires
    * access to a boost::asio::io_service instance, then this class becomes a
    * model of the Service concept (**LINK**) and must only use the constructor
    * which takes a ProtocolService (**LINK**). If the protocol isn't async then
    * the class can be used without a ProtocolService.
    */
   template<typename Protocol_ = ::cgi::fcgi_>
   class acceptor_service_impl
     : public detail::service_base< ::cgi::fcgi::acceptor_service_impl<Protocol_> >
   {
   public:

     /// The unique service identifier
     //static boost::asio::io_service::id id;
 
     struct implementation_type
     {
       typedef Protocol_                             protocol_type;
       typedef common::basic_protocol_service<
                 protocol_type
               >                                     protocol_service_type;
       typedef boost::asio::ip::tcp                  native_protocol_type;
       typedef fcgi::request                         request_type;
       typedef boost::asio::socket_acceptor_service<
                 native_protocol_type
               >                                     acceptor_service_type;
       typedef unsigned short                        port_number_type;
       typedef boost::asio::ip::tcp::endpoint        endpoint_type;
       //typedef typename
       //  acceptor_service_type::native_type          native_type;
 
       acceptor_service_type::implementation_type    acceptor_;
       boost::mutex                                  mutex_;
       std::queue<boost::shared_ptr<request_type> >  waiting_requests_;
       protocol_service_type*                        service_;
       port_number_type                              port_num_;
       endpoint_type                                 endpoint_;
     }; 

     typedef acceptor_service_impl<Protocol_>            type;
     typedef typename
       type::implementation_type::protocol_type
                                                         protocol_type;
     typedef typename
       type::implementation_type::protocol_service_type
                                                         protocol_service_type;
     typedef typename
       type::implementation_type::acceptor_service_type
                                                         acceptor_service_type;
     typedef typename
       type::implementation_type::native_protocol_type
                                                         native_protocol_type;
     typedef typename
       acceptor_service_type::native_type                native_type;

     typedef typename 
       type::implementation_type::endpoint_type          endpoint_type;
 

     explicit acceptor_service_impl(::cgi::io_service& ios)
       : detail::service_base< ::cgi::fcgi::acceptor_service_impl<Protocol_> >(ios)
       , acceptor_service_(boost::asio::use_service<acceptor_service_type>(ios))
       //, endpoint(boost::asio::ip::tcp::v4())
     {
     }
 
     protocol_service_type&
       service(implementation_type const& impl) const
     {
       return *impl.service_;
     }

     /// Default-initialize the acceptor.
     boost::system::error_code
       default_init(implementation_type& impl, boost::system::error_code& ec)
     {
#if ! defined(BOOST_WINDOWS)
       //assign(impl.acceptor_, , 0, ec);
       return acceptor_service_.assign(impl.acceptor_, boost::asio::ip::tcp::v4()
                                      , 0, ec);
 #else
//#      error "Windows isn't supported at the moment"
       HANDLE hListen = INVALID_HANDLE_VALUE;
       boost::asio::detail::socket_type sock;
       struct sockaddr sa;
       int sa_len = sizeof(sa);
#if NO_WSAACCEPT
       sock = accept((boost::asio::detail::socket_type)hListen, &sa, &sa_len);
       if (sock == INVALID_SOCKET)
         return cgi::error::invalid_socket;
#else
       sock = WSAAccept((unsigned int)hListen, &sa, &sa_len, NULL, (DWORD)NULL);
       if (sock == INVALID_SOCKET)
         return ::cgi::error::invalid_socket;
#endif
#endif
       return ec;
     }

     void set_protocol_service(implementation_type& impl
                              , protocol_service_type& ps)
     {
       impl.protocol_service_ = &ps;
     }

     protocol_service_type& 
       get_protocol_service(implementation_type& impl)
     {
       BOOST_ASSERT(impl.service_ != NULL);
       return *impl.service_;
     }

     void construct(implementation_type& impl)
     {
       acceptor_service_.construct(impl.acceptor_);
       //impl.acceptor_ptr().reset(impl::acceptor_type(this->io_service()));
     }
 
     void destroy(implementation_type& impl)
     {
       // close/reject all the waiting requests
       /***/
       acceptor_service_.destroy(impl.acceptor_);
     }
 
     void shutdown_service()
     {
       acceptor_service_.shutdown_service();
     }
 
     /// Check if the given implementation is open.
     bool is_open(implementation_type& impl)
     {
       return acceptor_service_.is_open(impl.acceptor_);
     }
 
     /// Open a new *socket* acceptor implementation.
     boost::system::error_code
       open(implementation_type& impl, const native_protocol_type& protocol
           , boost::system::error_code& ec)
     {
       return acceptor_service_.open(impl.acceptor_, protocol, ec);
     }
 
     template<typename Endpoint>
     boost::system::error_code
       bind(implementation_type& impl, const Endpoint& endpoint
           , boost::system::error_code& ec)
     {
       acceptor_service_.set_option(impl.acceptor_,
           boost::asio::socket_base::reuse_address(true), ec);
       return acceptor_service_.bind(impl.acceptor_, endpoint, ec);
     }
 
     /// Assign an existing native acceptor to a *socket* acceptor.
     boost::system::error_code
       assign(implementation_type& impl, const native_protocol_type& protocol
             , const native_type& native_acceptor
             , boost::system::error_code& ec)
     {
       return acceptor_service_.assign(impl.acceptor_, protocol
                                      , native_acceptor, ec);
     }    
 
     boost::system::error_code
       listen(implementation_type& impl, int backlog, boost::system::error_code& ec)
     {
       return acceptor_service_.listen(impl.acceptor_, backlog, ec);
     }
 
     /// Accepts one request.
     template<typename CommonGatewayRequest>
     boost::system::error_code
       accept(implementation_type& impl, CommonGatewayRequest& request
             , endpoint_type* endpoint, boost::system::error_code& ec)
     {
       /* THIS BIT IS BROKEN:
        *-- The noncopyable semantics of a basic_request<> don't allow the
            assignment. There are a couple of ways around this; the one that
            seems sensible is to keep the basic_request<>s noncopyable, but
            allow the actual data be copied. At the moment the actual data is
            held in a vector<string> headers container and a cgi::streambuf.
            These two bits should really be factored out into a message type.
            IOW, the message type will be copyable (but should probably have
            unique-ownership semantics).
        --*
       {
         boost::mutex::scoped_lock lk(impl.mutex_);
         if (!impl.waiting_requests_.empty())
         {
           request = *(impl.waiting_requests_.front());
           impl.waiting_requests_.pop();
           return ec;
         }
       }
       */

       BOOST_ASSERT
       ( ! request.is_open()
        && "Error: Calling accept on open request (close it first?)."
       );

       // If the client is open, make sure the request is clean.
       // ie. don't leak data from one request to another!
       if (request.client().is_open())
       {
         request.clear();
       }

       // If we can reuse this request's connection, return.
       if (request.client().keep_connection())
         return ec;

       // ...otherwise accept a new connection.
       return acceptor_service_.accept(impl.acceptor_,
                request.client().connection()->next_layer(), endpoint, ec);
     }
 
     /// Asynchronously accepts one request.
     //template<typename CommonGatewayRequest, typename Handler>
     //void async_accept(implementation_type& impl, CommonGatewayRequest& request
     template<typename Handler>
     void async_accept(implementation_type& impl, typename implementation_type::request_type& request
                      , Handler handler)
     {
       this->io_service().post(
         detail::accept_handler<type, Handler>(*this, impl, request, handler)
       );
     }

     /// Close the acceptor (not implemented yet).
     boost::system::error_code
       close(implementation_type& impl, boost::system::error_code& ec)
     {
       return boost::system::error_code(348, boost::system::system_category);
     }

     typename implementation_type::endpoint_type
       local_endpoint(implementation_type& impl, boost::system::error_code& ec)
     {
       return acceptor_service_.local_endpoint(impl.acceptor_, ec);
     }

   public:
     template<typename CommonGatewayRequest, typename Handler>
     void check_for_waiting_request(implementation_type& impl
                                   , CommonGatewayRequest& request
                                   , Handler handler)
     {
       // We can't call accept on an open request (close it first).
       if (request.is_open())
         return handler(error::accepting_on_an_open_request);

       // If the client is open, make sure the request is clean.
       // ie. don't leak data from one request to another!
       if (request.client().is_open())
       {
         request.clear();
       }

       // If we can reuse this request's connection, return.
       if (request.client().keep_connection())
         return handler(boost::system::error_code());

       // ...otherwise accept a new connection (asynchronously).
       return acceptor_service_.async_accept(impl.acceptor_,
         request.client().connection()->next_layer(), 0, handler);
     }
 
   public:
     /// The underlying socket acceptor service.
     acceptor_service_type& acceptor_service_;
   };
 
 } // namespace fcgi
} // namespace cgi
 
#include "boost/cgi/detail/pop_options.hpp"

#endif // CGI_FCGI_ACCEPTOR_SERVICE_IMPL_HPP_INCLUDED__

