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
    typedef ::cgi::map                         map_type;
    typedef Connection                         connection_type;
    typedef typename connection_type::pointer  conn_ptr;

    /// Constructor
    cgi_request_impl_base()
      : stdin_parsed_(false)
      , http_status_(http::ok)
      , request_status_(unloaded)
    {
    }

    map_type& env_vars()             { return env_vars_;       }
    map_type& get_vars()             { return get_vars_;       }
    map_type& post_vars()            { return post_vars_;      }
    map_type& cookie_vars()          { return cookie_vars_;    }

    bool& stdin_parsed()              { return stdin_parsed_;   }
    http::status_code& http_status() { return http_status_;    }
    status_type& status()            { return request_status_; }

    conn_ptr& connection()           { return connection_;     }
    std::string& null_str()          { return null_str_;       }

  protected:
    //conn_ptr connection() { return connection_; }

    //friend class cgi_service_impl_base<RequestImpl>;

    map_type env_vars_;
    map_type get_vars_;
    map_type post_vars_;
    map_type cookie_vars_;

    bool stdin_parsed_;

    http::status_code http_status_;
    status_type request_status_;

    conn_ptr connection_;

    std::string null_str_;
  };

  //template<> inline const std::string&
  //cgi_request_impl::var<tags::ENV>(const std::string& name)
  //{
  //  return ::getenv(name.c_str());
  //}

  /// Get a request map of all the environment meta-variables (slow)
  /**
   * -- NOT IMPLEMENTED FOR NOW --
   *
   * In the case of a CGI request, the environment meta-data is usually stored
   * in the process environment, which means there is no direct access to all
   * of them as a map_type&. In other words, this function call will have to
   * load all of the variables into memory and then return the map
   */
  //template<> inline cgi_request_impl::map_type&
  //cgi_request_impl::var<tags::ENV>()
  //{
  //  throw std::logic_error("Can't get all environment vars as a map_type&");
  //}

  //template<> inline cgi_request_impl::map_type&
  //cgi_request_impl::var<tags::HTTP>() { return http_map_; }

  //template<> inline cgi_request_impl::map_type&
  //cgi_request_impl::var<tags::COOKIE>() { return cookie_map_; }

  //template<> inline cgi_request_impl::map_type&
  //cgi_request_impl::var<tags::GET>() { return get_map_; }

  //template<> inline cgi_request_impl::map_type&
  //cgi_request_impl::var<tags::POST>() { return post_map_; }

} // namespace cgi

#endif // CGI_CGI_REQUEST_IMPL_BASE_HPP_INCLUDED__
