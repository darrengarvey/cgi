//                -- fcgi/acceptor.hpp --
//
//           Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_FCGI_ACCEPTOR_HPP_INCLUDED__
#define CGI_FCGI_ACCEPTOR_HPP_INCLUDED__

#include "boost/cgi/basic_request_acceptor.hpp"
#include "boost/cgi/fcgi/request_acceptor_service.hpp"

namespace cgi {
 namespace fcgi {

   /// Typedef for common usage (FCGI)
   typedef
     ::cgi::common::basic_request_acceptor<
       fcgi_request_acceptor_service<>
     >
   acceptor;

 } // namespace fcgi
} // namespace cgi

#endif // CGI_FCGI_ACCEPTOR_HPP_INCLUDED__
