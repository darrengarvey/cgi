//               -- scgi/request_impl.hpp --
//
//            Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_SCGI_REQUEST_IMPL_HPP_INCLUDED__
#define CGI_SCGI_REQUEST_IMPL_HPP_INCLUDED__

#include <boost/shared_ptr.hpp>

#include "boost/cgi/map.hpp"
#include "boost/cgi/role_type.hpp"
#include "boost/cgi/status_type.hpp"
#include "boost/cgi/http/status_code.hpp"
#include "boost/cgi/connections/tcp_socket.hpp"

/**************
 *
 * THIS FILE IS OBSOLETE.
 *
 * SEE request_service.hpp:BOOST_CGI_NAMESPACE::scgi::scgi_request_service::implementation_type
 * INSTEAD.
 *
**************/

namespace BOOST_CGI_NAMESPACE {
 namespace scgi {

  
  /// The implementation_type for scgi_request_service
  class scgi_request_impl
  {
  public:
    typedef BOOST_CGI_NAMESPACE::map                map_type;
    typedef tcp_connection                          connection_type;
    typedef connection_type::pointer                connection_ptr;
    typedef ::BOOST_CGI_NAMESPACE::basic_client<
        connection_type
      >                                             client_type;

    scgi_request_impl()
      : stdin_parsed_(false)
      , http_status_(http::ok)
      , request_status_(unloaded)
      , request_finished_(false)
    {
    }

    ~scgi_request_impl()
    {
    }

    connection_ptr connection()
    {
      return connection_;
    }

  private:
    friend class scgi_service_impl;

    connection_ptr connection_;

    bool stdin_parsed_;
    http::status_code http_status_;
    status_type request_status_;

    map_type env_vars_;
    map_type get_vars_;
    map_type post_vars_;
    map_type cookie_vars_;

    std::string null_str_;
    bool request_finished_;
  };

 } // namespace scgi
} // namespace BOOST_CGI_NAMESPACE

#endif // CGI_SCGI_REQUEST_IMPL_HPP_INCLUDED__
