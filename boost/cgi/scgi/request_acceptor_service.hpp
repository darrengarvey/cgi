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
#include "boost/cgi/detail/push_options.hpp"

#include <boost/utility/enable_if.hpp>

//#include "is_async.hpp"
#include "boost/cgi/io_service.hpp"
#include "boost/cgi/detail/throw_error.hpp"
#include "boost/cgi/detail/protocol_traits.hpp"
#include "boost/cgi/basic_protocol_service_fwd.hpp"
#include "boost/cgi/detail/service_base.hpp"
//#include "service_selector.hpp"
#include "boost/cgi/scgi/acceptor_service_impl.hpp"

namespace cgi {

  /// The service class for SCGI basic_request_acceptor<>s
  /**
   * Note: If the protocol is an asynchronous protocol, which means it requires
   * access to a boost::asio::io_service instance, then this class becomes a
   * model of the Service concept (**LINK**) and must only use the constructor
   * which takes a ProtocolService (**LINK**). If the protocol isn't async then
   * the class can be used without a ProtocolService.
   */
  template<typename Protocol_ = scgi_>
  class scgi_request_acceptor_service
    : public detail::service_base<scgi_request_acceptor_service<Protocol_> >
  {
  public:
    //typedef typename service_impl_type::impl_type     impl_type;

    typedef scgi::acceptor_service_impl<>               service_impl_type;
    typedef service_impl_type::implementation_type      implementation_type;
    typedef typename implementation_type::protocol_type          protocol_type;
    typedef implementation_type::endpoint_type          endpoint_type;
    typedef typename service_impl_type::native_type     native_type;
    //typedef basic_protocol_service<protocol_type>       protocol_service_type;

    /// The unique service identifier
    //static boost::asio::io_service::id id;

    scgi_request_acceptor_service(::cgi::io_service& ios)
      : detail::service_base<scgi_request_acceptor_service<protocol_type> >(ios)
      , service_impl_(ios)
    {
    }

    void construct(implementation_type& impl)
    {
      service_impl_.construct(impl);
    }

    void destroy(implementation_type& impl)
    {
      service_impl_.destroy(impl);
    }

    void shutdown_service()
    {
      service_impl_.shutdown_service();
    }

    bool is_open(implementation_type& impl)
    {
      return service_impl_.is_open(impl);
    }

    void close(implementation_type& impl)
    {
      boost::system::error_code ec;
      service_impl_.close(impl, ec);
      detail::throw_error(ec);
    }

    boost::system::error_code
      close(implementation_type& impl, boost::system::error_code& ec)
    {
      return service_impl_.close(impl, ec);
    }

    template<typename Protocol>
    boost::system::error_code
      open(implementation_type& impl, const Protocol& protocol
          , boost::system::error_code& ec)
    {
      return service_impl_.open(impl, protocol, ec);
    }

    template<typename Endpoint>
    boost::system::error_code
      bind(implementation_type& impl, const Endpoint& endpoint
          , boost::system::error_code& ec)
    {
      return service_impl_.bind(impl, endpoint, ec);
    }

    boost::system::error_code
      listen(implementation_type& impl, int backlog, boost::system::error_code& ec)
    {
      return service_impl_.listen(impl, backlog, ec);
    }

    template<typename CommonGatewayRequest>
    boost::system::error_code
      accept(implementation_type& impl, CommonGatewayRequest& request
            , boost::system::error_code& ec)
    {
      return service_impl_.accept(impl, request, ec);
    }

    template<typename CommonGatewayRequest, typename Endpoint>
    boost::system::error_code
      accept(implementation_type& impl, CommonGatewayRequest& request
            , Endpoint* ep, boost::system::error_code& ec)
    {
      return service_impl_.accept(impl, request, ep, ec);
    }

    template<typename Handler>
    void async_accept(implementation_type& impl, Handler handler)
    {
      service_impl_.async_accept(impl, handler);
    }

    template<typename T>
    void set_protocol_service(implementation_type& impl, T& ps)
    {
      impl.service_ = &ps;
    }

    typename implementation_type::endpoint_type
      local_endpoint(implementation_type& impl, boost::system::error_code& ec)
    {
      return service_impl_.local_endpoint(impl, ec);
    }

    template<typename Protocol>
    boost::system::error_code
      assign(implementation_type& impl, Protocol protocol
            , const native_type& native_acceptor, boost::system::error_code& ec)
    {
      return service_impl_.assign(impl, protocol, native_acceptor, ec);
    }

  public:
    service_impl_type service_impl_;
  };

} // namespace cgi

#include "boost/cgi/detail/pop_options.hpp"

#endif // CGI_REQUEST_SERVICE_HPP_INCLUDED

