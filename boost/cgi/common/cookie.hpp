//                   -- cookie.hpp --
//
//           Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_COOKIE_HPP_INCLUDED__
#define CGI_COOKIE_HPP_INCLUDED__

#include <string>
#include <boost/system/error_code.hpp>
#include <boost/tokenizer.hpp>

namespace cgi {
 namespace common {

  template<typename CharT> struct basic_cookie;
  
  // typedefs for common usage
  typedef basic_cookie<char>    cookie;
  typedef basic_cookie<wchar_t> wcookie;

  /// A `basic_cookie<>` object that can be (out-) streamed
  /**
   * Either set the parameters in the constructor, or set them directly.
   * Note: If you want to set the parameters individually, remember that each
   * parameter must NOT have a trailing semi-colon!
   *
   * TODO
   * - Data should be URL-encoded, or maybe provide an overload for url_decode
   *   that takes an HttpCookie?
   * - Add from_string() ?
   */
  template<typename CharT>
  struct basic_cookie
  {
    typedef CharT                             char_type;
    typedef typename std::basic_string<CharT> string_type;

    basic_cookie() {}

    /// Delete the cookie named `_name`.
    basic_cookie(const string_type& _name)
      : name(_name)
      , value()
      , expires("Fri, 05-Jun-1989 15:30:00 GMT")
      , path("/")
      , secure(false)
      , http_only(false)
    {
    }

    /// Create a cookie.
    basic_cookie(const string_type& _name, const string_type& _val
                , const string_type& _expires = ""
                , const string_type& _path = "/"
                , const string_type& _domain = ""
                , bool _secure = false
                , bool HttpOnly = false)
      : name(_name)
      , value(_val)
      , expires(_expires)
      , path(_path)
      , domain(_domain)
      , secure(_secure)
      , http_only(HttpOnly)
    {
    }

    string_type name;
    string_type value;
    string_type expires;
    string_type path;
    string_type domain;
    bool secure;
    bool http_only;

    /// Create a cookie from a const char*
    /**
     * Rules taken from: http://wp.netscape.com/newsref/std/cookie_spec.html
     *
     * Assumes:
     * - Parts of the cookie are delimited by '; '. ie. if there is no space,
     *   or multiple spaces after the semi-colon, this function won't work...
     */
    /* Actually, I'm omitting these functions for now, just had a thought...
    static basic_cookie<string_type>
      from_string(const char* str, boost::system::error_code& ec)
    {
      typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
      boost::char_separator<char> sep(";=");
      tokenizer tokens(str, sep);
      for (tokenizer::iterator iter = tokens.begin();
           iter != tokens.end(); ++iter)
      {

      }
      return ck;
    }

    static basic_cookie<string_type> from_string(const char* str)
    {
      boost::system::error_code ec;
      cookie ck = from_string(ec);
      detail::throw_error(ec);
      return ck;
    }

    static basic_cookie<string_type> from_string(std::string& str)
    {
      return from_string(str.c_str());
    }

    static basic_cookie<string_type>
      from_string(std::string& str, boost::system::error_code& ec)
    {
      return from_string(str.c_str(), ec);
    }
    */

    /// Make a string out of the cookie
    string_type to_string() const
    {
      string_type str(name + "=" + value);
      if (!expires.empty()) str += ("; expires=" + expires);
      if (!path.empty()   ) str += ("; path="    + path);
      if (!domain.empty() ) str += ("; domain="  + domain);
      if ( secure         ) str += "; secure";
      if ( http_only      ) str += "; HttpOnly";
      return str;
    }
  };

 } // namespace common
} // namespace cgi


template<typename OutStream, typename T>
inline OutStream& operator<< (OutStream& os, cgi::common::basic_cookie<T> const& ck)
{
  os<< ck.to_string();
  return os;
}

#endif // CGI_COOKIE_HPP_INCLUDED__

