//                   -- scgi.hpp --
//
//           Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_SCGI_HPP_INCLUDED__
#define CGI_SCGI_HPP_INCLUDED__

// #include headers for SCGI use.
#include "boost/cgi/scgi/service.hpp"
#include "boost/cgi/scgi/acceptor.hpp"
#include "boost/cgi/scgi/request_service.hpp"
#include "boost/cgi/scgi/request_acceptor_service.hpp"
#include "boost/cgi/detail/common_headers.hpp"

/*
namespace cgi {

  typedef basic_request_acceptor<scgi_request_acceptor_service> scgi_acceptor;

# ifndef CGI_NO_IMPLICIT_TYPEDEFS
    typedef scgi_acceptor acceptor;
# endif

} // namespace cgi
*/

/// Dump acgi stuff into the boost namespace
BOOST_CGI_NAMESPACE_BEGIN
 namespace acgi {
   using namespace ::cgi::scgi;
   using namespace ::cgi::common;
 } // namespace acgi
BOOST_CGI_NAMESPACE_END

#endif // CGI_SCGI_HPP_INCLUDED__
