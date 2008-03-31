//             -- cgi_request_impl_base.hpp --
//
//            Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_CGI_REQUEST_IMPL_BASE_HPP_INCLUDED__
#define CGI_CGI_REQUEST_IMPL_BASE_HPP_INCLUDED__

#include <map>
#include <string>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

#include "boost/cgi/http/status_code.hpp"
#include "boost/cgi/connections/stdio.hpp"
#include "boost/cgi/role_type.hpp"
#include "boost/cgi/status_type.hpp"
#include "boost/cgi/map.hpp"
//#include

// Make this ProtocolService-independent

namespace cgi {

  //Forward declaration
  template<typename T>
  class cgi_service_impl_base;


  /// Implementation for a standard CGI request
  /**
   * Note: This isn't noncopyable since there's no real reason it can't be
   * copied around. Since basic_request is noncopyable, basic copying will be
   * restricted but if someone really wants to copy the data, then they can.
   */
  template<typename Connection>
  class cgi_request_impl_base
  {
  public:
    typedef ::cgi::common::map                         map_type;
    typedef Connection                         connection_type;
    typedef 
      common::basic_client<Connection, tags::acgi>     client_type;
    typedef typename connection_type::pointer  conn_ptr;

    /// Constructor
    cgi_request_impl_base()
      : stdin_parsed_(false)
      , stdin_data_read_(false)
      , stdin_bytes_left_(-1)
      , http_status_(http::ok)
      , request_status_(unloaded)
    {
    }

    map_type& env_vars()             { return env_vars_;       }
    map_type& get_vars()             { return get_vars_;       }
    map_type& post_vars()            { return post_vars_;      }
    map_type& cookie_vars()          { return cookie_vars_;    }

    bool stdin_parsed()              { return stdin_parsed_;   }
    http::status_code& http_status() { return http_status_;    }
    status_type& status()            { return request_status_; }

    conn_ptr& connection()           { return connection_;     }
    //std::string& null_str()          { return null_str_;       }

  public:
    //conn_ptr connection() { return connection_; }

    //friend class cgi_service_impl_base<RequestImpl>;

    map_type env_vars_;
    map_type get_vars_;
    map_type post_vars_;
    map_type cookie_vars_;

  public:
    bool stdin_parsed_;
    bool stdin_data_read_;
    std::size_t stdin_bytes_left_;
  protected:

    http::status_code http_status_;
    status_type request_status_;

    conn_ptr connection_;
  };

} // namespace cgi

#endif // CGI_CGI_REQUEST_IMPL_BASE_HPP_INCLUDED__
