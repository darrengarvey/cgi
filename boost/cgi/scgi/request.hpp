//                 -- scgi/request.hpp --
//
//            Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_SCGI_REQUEST_HPP_INCLUDED__
#define CGI_SCGI_REQUEST_HPP_INCLUDED__

#include "boost/cgi/tags.hpp"
#include "boost/cgi/basic_request_fwd.hpp"
//#include "boost/cgi/request_service_fwd.hpp"
#include "boost/cgi/scgi/request_service.hpp"
#include "boost/cgi/scgi/service.hpp"
namespace cgi {

  //class scgi::scgi_request_service;

  // This is deprecated/obsolete
  //typedef basic_request<scgi::scgi_request_service> scgi_request;

 namespace scgi {
   // typedef for typical usage (SCGI)
   typedef basic_request<scgi_request_service, service> request;
 } // namespace scgi
} // namespace cgi

//#include "boost/cgi/request_service.hpp"
#include "boost/cgi/basic_request.hpp"

#endif // CGI_SCGI_REQUEST_HPP_INCLUDED__
