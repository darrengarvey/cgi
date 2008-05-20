//              -- acgi_request_impl.hpp --
//
//            Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_ACGI_REQUEST_IMPL_HPP_INCLUDED__
#define CGI_ACGI_REQUEST_IMPL_HPP_INCLUDED__

#include "boost/cgi/acgi/service.hpp"
#include "boost/cgi/basic_client.hpp"
#include "boost/cgi/connections/async_stdio.hpp"
#include "boost/cgi/detail/cgi_request_impl_base.hpp"

// Make this ProtocolService-independent

namespace cgi {

  // Forward declaration
  class acgi_service_impl;

  class acgi_request_impl
    : public detail::cgi_request_impl_base<common::async_stdio_connection>
  {
  public:
    typedef acgi_service    protocol_service_type;
    typedef common::async_stdio_connection connection_type;
    typedef
      ::cgi::common::basic_client<
        connection_type, tags::acgi
      >
    client_type;

    acgi_request_impl()
      : detail::cgi_request_impl_base<connection_type>()
    {
    }

    protocol_service_type* service_;
  protected:
    //acgi_request_impl(); // private default constructor
    friend class acgi_service_impl;
  };

} // namespace cgi

#endif // CGI_ASYNC_CGI_REQUEST_IMPL_HPP_INCLUDED__
