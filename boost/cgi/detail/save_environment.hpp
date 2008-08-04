//           -- detail/save_environment.hpp --
//
//           Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_DETAIL_SAVE_ENVIRONMENT_HPP_INCLUDED__
#define CGI_DETAIL_SAVE_ENVIRONMENT_HPP_INCLUDED__

#include <map>
#include <string>
#include <cstdlib>
#include "boost/cgi/common/map.hpp"

// The process' environment
#if BOOST_WORKAROUND(BOOST_MSVC, BOOST_TESTED_AT(1500))
  // MSVC warns of 'inconsistent dll linkage' here...
  _CRTIMP extern char** _environ;
#else
  extern char** environ;
#endif


namespace cgi {
 namespace detail {

   /// Save all information from `environment` to env_map
   /**
    * @param env This defaults to `::environ`, or the current process'
    *            environment.
    */
   template<typename MapT>
   void save_environment(MapT& env_map, char** env = 
#if BOOST_WORKAROUND(BOOST_MSVC, BOOST_TESTED_AT(1500))
	_environ
#else
	 environ
#endif
	   )
   {
     std::string sa;
     std::string sb;

     for(; *env; ++env)
     {
       int i=0;
       int j=strlen(*env);
       for(; i < j; ++i)
         if ((*env)[i] == '=')
           break;

       // Note: empty variables are not guaranteed to be set by the server, so
       // we are free to ignore them too.
       if ((*env)[i+1] != '\0')
       {
         sa.assign(*env, i);
         sb.assign((*env+i+1), j-i-1);
         env_map[sa.c_str()] = sb;
       }
     }
   }

 } // namespace detail
} // namespace cgi

/* Alternative version which doesn't copy the 'value' of the variable
 ******************************************************************************
   void save_environment(std::map<std::string,const char*>& env_map
                        , char** env = ::environ)
   {
     for(; *env; ++env)
     {
       int i=0;
       int j=strlen(*env);
       for(; i < j; ++i)
         if ((*env)[i] == '=')
           break;

       // Note: empty variables are not guaranteed to be set by the server, so
       // we are free to ignore them too.
       if ((*env)[i+1] != '\0')
       {
         std::string sa(*env, i);
         env_map[sa] = (*env+i+1);
       }
     }
   }
 ******************************************************************************
 */

#endif // CGI_DETAIL_SAVE_ENVIRONMENT_HPP_INCLUDED__
