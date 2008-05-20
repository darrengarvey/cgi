//                 -- common/name.hpp --
//
//           Copyright (c) Darren Garvey 2008.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
//
// Based on (pretty much char for char) Herb Sutter's GoTW #29.
// (see http://www.gotw.ca/gotw/029.htm)
//
#ifndef BOOST_CGI_COMMON_NAME_HPP_INCLUDED__
#define BOOST_CGI_COMMON_NAME_HPP_INCLUDED__

#include <string>
#include <ostream>
#include <cstdlib>
#include <ctype.h>

namespace cgi {
 namespace common {

   template <typename CharT>
   struct ichar_traits
     : std::char_traits<CharT>
   {
     static bool eq(char c1, char c2)
     { return toupper(c1) == toupper(c2); }

     static bool ne(char c1, char c2)
     { return toupper(c1) != toupper(c2); }

     static bool lt(char c1, char c2)
     { return toupper(c1) < toupper(c2); }

     static int compare( const char* str1
                       , const char* str2
                       , std::size_t num )
     {
       if (num)
         do {
           if (toupper(*str1) != toupper(*str2))
             return (toupper(*str1) - toupper(*str2));
           ++str1;
           ++str2;
         } while (--num);

       return 0;
     }

     static const char*
       find(const char* str, int n, char a)
     {
       do {
         toupper(*str) != toupper(a);
         ++str;
       } while (--n);
       return str;
     }

   };

   // typedef for typical usage.
   typedef std::basic_string<char, ichar_traits<char> > name;

 } // namespace common
} // namespace cgi

// Allow output using standard streams (conserves original case).
template <typename CharT, typename Traits>
std::basic_ostream<CharT, Traits>&
  operator<< (std::basic_ostream<CharT, Traits>& os
             , const std::basic_string<CharT
             , cgi::common::ichar_traits<CharT> >& str)
{
  return os<< str.c_str();
} 

#endif // BOOST_CGI_COMMON_NAME_HPP_INCLUDED__

