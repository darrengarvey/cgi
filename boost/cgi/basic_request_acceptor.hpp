//            -- basic_request_acceptor.hpp --
//
//            Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_BASIC_REQUEST_ACCEPTOR_HPP_INCLUDED__
#define CGI_BASIC_REQUEST_ACCEPTOR_HPP_INCLUDED__

#include <boost/noncopyable.hpp>
#include <boost/system/error_code.hpp>

#include <boost/asio/basic_io_object.hpp>
#include "detail/throw_error.hpp"

namespace cgi {

  template<typename RequestAcceptorService>
  class basic_request_acceptor
    : private boost::noncopyable
    , public boost::asio::basic_io_object<RequestAcceptorService>
  {
  public:
    //  typedef impl_type;
    typedef RequestAcceptorService         service_type;
    typedef service_type::protocol_type    protocol_type;

    explicit basic_request_acceptor(basic_protocol_service<protocol_type>& s)
      : boost::asio::basic_io_object<RequestAcceptorService>(s.io_service())
    {
    }

    ~basic_request_acceptor()
    {
    }

    template<typename CommonGatewayRequest>
    void accept(CommonGatewayRequest& request)
    {
      boost::system::error_code ec;
      this->service.accept(this->implementation, request, ec);
      detail::throw_error(ec);
    }

    template<typename CommonGatewayRequest> boost::system::error_code&
    accept(CommonGatewayRequest& request, boost::system::error_code& ec)
    {
      return this->service.accept(this->implementation, request, ec);
    }

    template<typename CommonGatewayRequest, typename Handler>
    void async_accept(CommonGatewayRequest& request, Handler handler)
    {
      this->service.async_accept(this->implementation, request, handler);
    }
  };

} // namespace cgi

#endif // CGI_BASIC_REQUEST_ACCEPTOR_HPP_INCLUDED__
