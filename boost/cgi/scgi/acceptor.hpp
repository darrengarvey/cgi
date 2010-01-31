//                -- scgi/acceptor.hpp --
//
//           Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_SCGI_ACCEPTOR_HPP_INCLUDED__
#define CGI_SCGI_ACCEPTOR_HPP_INCLUDED__

#include "boost/cgi/basic_request_acceptor.hpp"
#include "boost/cgi/scgi/request_acceptor_service.hpp"

namespace BOOST_CGI_NAMESPACE {
 namespace scgi {

   /// Typedef for common usage (SCGI)
   typedef basic_request_acceptor<scgi_request_acceptor_service<> > acceptor;

 } // namespace scgi
} // namespace BOOST_CGI_NAMESPACE

#endif // CGI_SCGI_ACCEPTOR_HPP_INCLUDED__
