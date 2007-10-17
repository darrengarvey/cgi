//                  -- url_decode.hpp --
//
//           Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_DETAIL_URL_DECODE_HPP_INCLUDED__
#define CGI_DETAIL_URL_DECODE_HPP_INCLUDED__

#include <string>

namespace cgi {
 namespace detail {

   /// Take two characters (a hex sequence) and return a char
   char url_decode( const char& c1, const char& c2 )
   {
     int ret = ( (c1 >= 'A' && c1 <= 'Z') || (c1 >= 'a' && c1 <= 'z')
                   ? ((c1 & 0xdf) - 'A') + 10
                   : (c1 - '0')
                 ) << 4;

     ret += ( (c2 >= 'A' && c2 <= 'Z') || (c2 >= 'a' && c2 <= 'z')
                ? ((c2 & 0xdf) - 'A') + 10
                : (c2 - '0')
            );

     return static_cast<char>(ret);
   }

   /// Workaround for istreams, since the above can't be used directly
   char url_decode(std::istream& is)
   {
     const char c1 = is.get();
     return url_decode(c1, is.get());
   }

   /// URL-decode a string
   std::string url_decode( const std::string& str )
   {
     std::string ret;

     for( unsigned int i=0; i < str.size(); i++ )
     {
       switch( str[i] )
       {
         case ' ':
           break;
         case '+':
           ret += ' ';
           break;
         case '%':
           ret += url_decode(str[i+1], str[i+2]);
           i += 2;
           break;
         default:
           ret += str[i];
       }
     }

     return ret;
   }

 } // namespace detail
} // namespace cgi

#endif // CGI_DETAIL_URL_DECODE_HPP_INCLUDED__
