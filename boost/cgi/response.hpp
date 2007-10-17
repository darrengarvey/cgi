//                   -- response.hpp --
//
//            Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_RESPONSE_HPP_INCLUDED__
#define CGI_RESPONSE_HPP_INCLUDED__

#include <string>

#include "boost/cgi/detail/push_options.hpp"
#include "boost/cgi/request_ostream.hpp"
#include "boost/cgi/buffer.hpp"
#include "boost/cgi/cookie.hpp"
#include "boost/cgi/header.hpp"

namespace cgi {

  /// The response class: a helper for responding to requests

  // The request_ostream is destined to become a basic_request_ostream
//typedef request_ostream<> response_;


  class response
    : public cgi::request_ostream
  {
  public:
    response(http::status_code sc = http::ok)
      : cgi::request_ostream(sc)
    {
    }

    response(cgi::streambuf* buf, http::status_code sc = http::ok)
      : cgi::request_ostream(buf, sc)
    {
    }

    ~response()
    {
    } 

    /// Some helper functions for the basic CGI 1.1 meta-variables
//     void auth_type(const std::string& value)
//     {
//       std::string str("Auth-type: ");
//       str += value;
//       this->headers_.push_back(cgi::buffer(str.c_str(), str.size()));
//     }
// 
//     void content_length(const std::string& value)
//     {
//       std::string str("Content-length: ");
//       str += value;
//       this->headers_.push_back(cgi::buffer(str.c_str(), str.size()));
//     }

  private:
    template<typename T>
    friend response& operator<<(response& resp, const T& t);
  };

  template<typename T>
  response& operator<<(response& resp, const T& t)
  {
    resp.ostream_<< t;
    return resp;
  }

  template<>
  response& operator<<(response& resp, const header& hdr)
  {
    resp.headers_sent_ = true;
    return resp<< hdr.content;
  }

  template<>
  response& operator<<(response& resp, const cookie& ck)
  {
    // Note: the 'set-cookie' isn't part of the cookie object since
    // the cookie can also be set after the headers have been sent.
    // See http://tinyurl.com/33znkj
    return resp<< "Set-cookie: " << ck << "\r\n";
  }

} // namespace cgi

#include "detail/pop_options.hpp"

#endif // CGI_RESPONSE_HPP_INCLUDED__
