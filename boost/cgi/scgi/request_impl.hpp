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

#include "../map.hpp"
#include "../role_type.hpp"
#include "../status_type.hpp"
#include "../http/status_code.hpp"
#include "../connections/tcp_socket.hpp"

namespace cgi {

  class scgi_request_impl
  {
  public:
    typedef cgi::map                  map_type;
    typedef tcp_connection            connection_type;
    typedef connection_type::pointer  connection_ptr;

    scgi_request_impl()
      : stdin_parsed_(false)
      , http_status_(http::ok)
      , request_status_(unloaded)
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

  };

} // namespace cgi

#endif // CGI_SCGI_REQUEST_IMPL_HPP_INCLUDED__
