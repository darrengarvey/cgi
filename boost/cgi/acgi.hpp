//                   -- acgi.hpp --
//
//           Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_ACGI_HPP_INCLUDED__
#define CGI_ACGI_HPP_INCLUDED__

// #include all acgi-related headers only
#include "boost/cgi/acgi/service.hpp"
#include "boost/cgi/acgi/request.hpp"
//#include "boost/cgi/acgi/request_service.hpp"
//#include "boost/cgi/acgi/request_impl.hpp"
//#include "gateway_impl/acgi_gateway_impl.hpp"
//#include "gateway_service/acgi_gateway_service.hpp"

// Include headers common to all protocols
#include "boost/cgi/detail/common_headers.hpp"

namespace cgi {
#ifndef CGI_NO_IMPLICIT_TYPEDEFS
  //typedef acgi_request request;
#endif
 namespace acgi {

   typedef acgi_request request;
   //typedef acgi_service service;
   //typedef acgi_acceptor acceptor;
   using namespace cgi;

 } // namespace acgi
} // namespace cgi

#endif // CGI_ACGI_HPP_INCLUDED__
