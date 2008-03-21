//                   -- header.hpp --
//
//           Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_HEADER_HPP_INCLUDED__
#define CGI_HEADER_HPP_INCLUDED__

#include <string>
#include <boost/lexical_cast.hpp>

namespace cgi {
 namespace common {

  //template<typename StringT = std::string>
  struct header
  {
    typedef std::string string_type;
    
    header()
      : content()
    {
    }

    
    /// Templated constructor to allow user-defined types to be converted
    //template<typename T>
    //header(T& t)
    //  : content(t.to_string())
    //{
    //}
    

    //template<>
    header(const string_type& _content)
      : content(_content)
    {
    }

    header(const string_type& name, const string_type& val)
      : content(name + ": " + val)
    {
    }

    //header(const std::string& name, const std::string& val)
    //  : content(name + ": " + val)
    //{
    //}

    /// Construct an header from a cookie.
    template<typename T>
    header(const basic_cookie<T>& ck)
      : content("Set-cookie: " + ck.to_string())
    {
    }

    string_type content;

  };

/*
  template<typename StringT>
  header<StringT>
    make_header(const StringT& name, const StringT& val)
  {
    return basic_header<StringT>(name, val);
  }* /

  template<typename T, typename StringT>
  T make_header(const StringT& name, const StringT& val)
  {
    return basic_header<StringT>(name, val);
  }*/


  //{ Some shortcuts, to cut down on typing errors.
  template<typename StringT>
  header
    content_type(StringT str)
  {
    return header("Content-type", str);
  }

  template<typename StringT>
  header
    content_encoding(const StringT& str)
  {
    return header("Content-encoding", str);
  }

  template<typename T>
  header
    content_length(const T& t)
  {
    return header("Content-length", boost::lexical_cast<std::string>(t));
  }

  template<typename T, typename Traits, typename Alloc>
  header
    content_length(const std::basic_string<T, Traits, Alloc>& str)
  {
    return header("Content-length", str);
  }
/*
  template<typename StringT>
  header<StringT>
    location(const StringT& url)
  {
    return header<StringT>("Location", url);
  }

  header<std::string>
    location(const std::string& url)
  {
    return header<std::string>("Location", url);
  }*/
  //template<typename T>
  header location(const std::string& url)
  {
    return header("Location", url);
  }
  //}

  // typedefs for typical usage
  //typedef basic_header<std::string>  header;
  //typedef basic_header<std::wstring> wheader;

 } // namespace common
} // namespace cgi

#endif // CGI_HEADER_HPP_INCLUDED__
