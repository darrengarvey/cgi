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

namespace BOOST_CGI_NAMESPACE {

  //typedef basic_protocol_service<tags::scgi> scgi_service;

 namespace scgi {

   // typedef for standard scgi::service (a model of ProtocolService)
   typedef basic_protocol_service< ::BOOST_CGI_NAMESPACE::scgi_> service;

 }
} // namespace BOOST_CGI_NAMESPACE

#endif // CGI_SCGI_SERVICE_HPP_INCLUDED__
