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

#include "service.hpp"
#include "boost/cgi/detail/cgi_request_impl_base.hpp"
#include "boost/cgi/connections/async_stdio.hpp"

// Make this ProtocolService-independent

namespace cgi {

  // Forward declaration
  class acgi_service_impl;

  class acgi_request_impl
    : public cgi_request_impl_base<async_stdio_connection>
  {
  public:
    typedef acgi_service    protocol_service_type;

    acgi_request_impl()
      : cgi_request_impl_base<async_stdio_connection>()
    {
    }

  protected:
    //acgi_request_impl(); // private default constructor
    friend class acgi_service_impl;
  };

} // namespace cgi

#endif // CGI_ASYNC_CGI_REQUEST_IMPL_HPP_INCLUDED__
