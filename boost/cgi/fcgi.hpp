//                   -- fcgi.hpp --
//
//           Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_FCGI_HPP_INCLUDED__
#define CGI_FCGI_HPP_INCLUDED__

// #include all fcgi-related files only
#include "boost/cgi/fcgi/service.hpp"
#include "boost/cgi/fcgi/acceptor.hpp"
#include "boost/cgi/fcgi/client.hpp"
#include "boost/cgi/fcgi/request.hpp"
//#include "boost/cgi/fcgi/request_service.hpp"
//#include "boost/cgi/fcgi/request_acceptor_service.hpp"
#include "boost/cgi/detail/common_headers.hpp"

namespace cgi {
 namespace fcgi {
   //using namespace ::cgi; // **FIXME** this must go.
   using namespace ::cgi::common; // import common elements.
 } // namespace fcgi
} // namespace cgi

/// Dump fcgi stuff into the boost namespace
namespace boost {
 namespace fcgi {
   using namespace ::cgi::fcgi;
 } // namespace acgi
} // namespace boost

#endif // CGI_FCGI_HPP_INCLUDED__
