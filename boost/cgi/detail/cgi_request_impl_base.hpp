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
///////////////////////////////////////////////////////////
#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <boost/fusion/support.hpp>
#include <boost/fusion/include/vector.hpp>
///////////////////////////////////////////////////////////
#include "boost/cgi/common/map.hpp"
#include "boost/cgi/common/role_type.hpp"
#include "boost/cgi/http/status_code.hpp"
#include "boost/cgi/connections/stdio.hpp"
#include "boost/cgi/common/request_status.hpp"

namespace cgi {
 namespace detail {

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
    typedef ::cgi::common::map                                map_type;
    typedef Connection                                        connection_type;
    typedef 
      common::basic_client<Connection, common::tags::acgi>    client_type;
    typedef typename connection_type::pointer                 conn_ptr;

    /// Constructor
    cgi_request_impl_base()
      : stdin_parsed_(false)
      , stdin_data_read_(false)
      , stdin_bytes_left_(-1)
      , http_status_(common::http::ok)
      , request_status_(common::unloaded)
    {
    }

    bool stdin_parsed()                      { return stdin_parsed_;   }
    common::http::status_code& http_status() { return http_status_;    }
    common::request_status& status()         { return request_status_; }

    conn_ptr& connection()                   { return connection_;     }
  public:
    //conn_ptr connection() { return connection_; }

    //friend class cgi_service_impl_base<RequestImpl>;

  public:
    bool stdin_parsed_;
    bool stdin_data_read_;
    std::size_t stdin_bytes_left_;
  protected:

    common::http::status_code http_status_;
    common::request_status request_status_;

    conn_ptr connection_;
  };

 } // namespace detail
} // namespace cgi

#endif // CGI_CGI_REQUEST_IMPL_BASE_HPP_INCLUDED__

