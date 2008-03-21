//                 -- fcgi/request.hpp --
//
//            Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_FCGI_REQUEST_HPP_INCLUDED__
#define CGI_FCGI_REQUEST_HPP_INCLUDED__

#include "boost/cgi/tags.hpp"
#include "boost/cgi/basic_request_fwd.hpp"
//#include "boost/cgi/request_service_fwd.hpp"
#include "boost/cgi/fcgi/service.hpp"
#include "boost/cgi/fcgi/request_service.hpp"

namespace cgi {
 namespace fcgi {
   
   // typedef for typical usage (FCGI)
   typedef
     ::cgi::common::basic_request<
        fcgi_request_service, service
     >
   request;

 } // namespace fcgi
} // namespace cgi

//#include "boost/cgi/basic_request.hpp"

#endif // CGI_FCGI_REQUEST_HPP_INCLUDED__
