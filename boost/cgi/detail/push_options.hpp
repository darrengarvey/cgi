//              -- detail/push_options.hpp --
//
//            Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////

//#ifdef _MSC_VER

#include "boost/cgi/config.hpp"

// You may want to remove these.
#if defined (BOOST_WINDOWS)
#   define _CRT_SECURE_NO_DEPRECATE 1
#   define _SCL_SECURE_NO_WARNINGS 1
#   define _CRT_SECURE_NO_WARNINGS 1
#   define NOMINMAX
#   pragma warning( disable : 4503 ) // warning: decorated  name length exceeded
#endif // defined (BOOST_WINDOWS)

#if !defined(BOOST_CGI_INLINE)
#  if defined(BOOST_CGI_BUILD_LIB)
#    define BOOST_CGI_INLINE
#  else
#    define BOOST_CGI_INLINE inline
#  endif
#endif

#ifdef BOOST_CGI_EXPORTS
#   define BOOST_CGI_API __declspec(dllexport)
#else
#   define BOOST_CGI_API __declspec(dllimport)
#endif // BOOST_CGI_EXPORTS

/// Keep empty query string variables.
/** Empty query string parameters (eg.
 *   `empty` in /path/to/script?empty&foo=bar)
 * aren't guaranteed by the CGI spec to be kept, but you might want to use
 * them. You just have to define `BOOST_CGI_KEEP_EMPTY_VARS` (**FIXME**
 * currently on by default).
 */
#define BOOST_CGI_KEEP_EMPTY_VARS
