//               -- request_service.hpp --
//
//            Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_REQUEST_SERVICE_HPP_INCLUDED
#define CGI_REQUEST_SERVICE_HPP_INCLUDED

#include "../detail/push_options.hpp"

#include <boost/utility/enable_if.hpp>

//#include "is_async.hpp"
#include "../io_service.hpp"
#include "../detail/throw_error.hpp"
#include "../detail/protocol_traits.hpp"
#include "../basic_protocol_service_fwd.hpp"
#include "../detail/service_base.hpp"
//#include "service_selector.hpp"

namespace cgi {

  /// The generic service class for basic_request<>s
  /**
   * Note: If the protocol is an asynchronous protocol, which means it requires
   * access to a boost::asio::io_service instance, then this class becomes a
   * model of the Service concept (**LINK**) and must only use the constructor
   * which takes a ProtocolService (**LINK**). If the protocol isn't async then
   * the class can be used without a ProtocolService.
   */
  template<typename Protocol>
  class scgi_request_acceptor_service
    : public detail::service_base<request_service<Protocol> >
  {
  public:
    //typedef typename service_impl_type::impl_type     impl_type;

    typedef scgi_request_acceptor_impl        implementation_type;
    typedef typename implementation_type::protocol_type
                                              protocol_type;
    typedef basic_protocol_service<Protocol>  protocol_service_type;

    /// The unique service identifier
    //static boost::asio::io_service::id id;
    //explicit request_service()
    //{
    //}

    scgi_request_acceptor_service(cgi::io_service& ios)
      : detail::service_base<request_service<Protocol> >(ios)
    {
    }

    /*
    request_service(protocol_service_type& ps)
      : detail::service_base<request_service<Protocol> >(ps.io_service())
    {
    }
    */
   
    void construct(implementation_type& impl)
    {
    }

    void destroy(implementation_type& impl)
    {
    }

    void shutdown_service()
    {
    }

    boost::system::error_code& accept(implementation_type& impl
                                     , boost::system::error_code& ec)
    {
      
    }

    template<typename Handler>
    void async_accept(implementation_type& impl, Handler handler)
    {
      
    }
  };

} // namespace cgi

#include "../detail/pop_options.hpp"

#endif // CGI_REQUEST_SERVICE_HPP_INCLUDED
