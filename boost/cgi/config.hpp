//                   -- form_parser.hpp --
//
//            Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef BOOST_CGI_CONFIG_HPP_INCLUDED__
#define BOOST_CGI_CONFIG_HPP_INCLUDED__

namespace boost {
 namespace cgi {}
} // namespace boost

#ifndef NDEBUG
# include <iostream>
#endif // NDEBUG

/// The namespace of the library.
/**
 * Default value:
 *   boost::cgi
 * which puts the library in the boost::cgi namespace.
 */
#ifndef BOOST_CGI_NAMESPACE
# define BOOST_CGI_NAMESPACE boost::cgi
#endif // BOOST_CGI_NAMESPACE

/// The opening namespace tag.
/**
 * Default value:
 *   namespace boost { namespace cgi {
 * which puts the library in the boost::cgi namespace.
 */
#ifndef BOOST_CGI_NAMESPACE_BEGIN
# define BOOST_CGI_NAMESPACE_BEGIN namespace boost { namespace cgi {
#endif // BOOST_CGI_NAMESPACE_BEGIN

/// The closing namespace tag.
/**
 * Default value:
 *   } } // namespace BOOST_CGI_NAMESPACE
 * which ends the boost::cgi namespaces.
 */
#ifndef BOOST_CGI_NAMESPACE_END
# define BOOST_CGI_NAMESPACE_END } } // namespace BOOST_CGI_NAMESPACE
#endif // BOOST_CGI_NAMESPACE_END

/// Library assertions, only in debug mode.
#ifndef BOOST_CGI_DEBUG_ASSERT
# ifdef NDEBUG
#   define BOOST_CGI_DEBUG_ASSERT(x) void
# else
#   define BOOST_CGI_DEBUG_ASSERT(x) assert(x)
# endif // NDEBUG
#endif // BOOST_CGI_DEBUG_ASSERT

/// Library assertions.
#ifndef BOOST_CGI_ASSERT
# ifdef NDEBUG
#   define BOOST_CGI_ASSERT(x) BOOST_ASSERT(x)
# else
#   define BOOST_CGI_ASSERT(x) BOOST_CGI_DEBUG_ASSERT(x)
# endif // NDEBUG
#endif // BOOST_CGI_ASSERT

#endif // BOOST_CGI_CONFIG_HPP_INCLUDED__
