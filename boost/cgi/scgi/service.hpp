//                 -- scgi/service.hpp --
//
//            Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_SCGI_SERVICE_HPP_INCLUDED__
#define CGI_SCGI_SERVICE_HPP_INCLUDED__

#include "boost/cgi/tags.hpp"
#include "boost/cgi/basic_protocol_service.hpp"

namespace cgi {

  //typedef basic_protocol_service<tags::scgi> scgi_service;

 namespace scgi {

   // typedef for standard scgi::service (a model of ProtocolService)
   typedef basic_protocol_service< ::cgi::scgi_> service;

 }
} // namespace cgi

#endif // CGI_SCGI_SERVICE_HPP_INCLUDED__
