//                -- acgi/acceptor.hpp --
//
//           Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_ACGI_ACCEPTOR_HPP_INCLUDED__
#define CGI_ACGI_ACCEPTOR_HPP_INCLUDED__

#include "boost/cgi/basic_request_acceptor.hpp"
#include "boost/cgi/acgi/request_acceptor_service.hpp"

namespace cgi {
 namespace acgi {

   /// Typedef for common usage (ACGI)
   typedef
     ::cgi::common::basic_request_acceptor<
       acgi_request_acceptor_service<>
     >
   acceptor;

 } // namespace acgi
} // namespace cgi

#endif // CGI_ACGI_ACCEPTOR_HPP_INCLUDED__
