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

  template<typename String> struct basic_cookie;
  
  // typedefs for common usage
  typedef basic_cookie<std::string> cookie;
  typedef basic_cookie<std::wstring> wcookie;

  /// A `basic_cookie<>` object that can be (out-) streamed
  /**
   * Either set the parameters in the constructor, or set them directly.
   * Note: If you want to set the parameters individually, remember that each
   * parameter must NOT have a trailing semi-colon!
   *
   * TODO
   * - Data should be URL-encoded, or maybe provide an overload for url_decode
   *   that takes an HttpCookie?
   * - Add to_string() ?
   */
  template<typename String>
  struct basic_cookie
  {
    typedef String string_type;

    basic_cookie() {}

    /// Delete the cookie named `_name`.
    basic_cookie(const string_type& _name)
      : content(_name)
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
      : content(_name)
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
    /* Actually, I'm omitting these functions for now, just had a though...
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
    std::string to_string()
    {
      std::string str(ck.name + "=" + ck.value);
      if (!ck.expires.empty()) str += "; expires=" += ck.expires;
      if (!ck.path.empty()   ) str += "; path="    += ck.path;
      if (!ck.domain.empty() ) str += "; domain="  += ck.domain;
      if ( secure            ) str += "; secure";
      if ( ck.http_only      ) str += "; HttpOnly";
      return str;
    }
  };

  template<typename OutStream, typename Cookie>
    OutStream& operator<<(OutStream& os, Cookie ck)
  {
    return os<< ck.to_string();
  }

} // namespace cgi

#endif // CGI_COOKIE_HPP_INCLUDED__
