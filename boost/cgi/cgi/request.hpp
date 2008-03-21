//                  -- cgi_request.hpp --
//
//            Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_CGI_REQUEST_HPP_INCLUDED__
#define CGI_CGI_REQUEST_HPP_INCLUDED__

#include "service.hpp"
#include "request_service.hpp"
#include "boost/cgi/tags.hpp"
#include "boost/cgi/basic_request_fwd.hpp"
#include "boost/cgi/cgi/request_impl.hpp"
#include "boost/cgi/basic_request.hpp"

namespace cgi {

  class cgi_service_impl;

  typedef common::basic_request<cgi_request_service, cgi_service> cgi_request;
  typedef cgi_request request;

 //namespace cgi {
 //  typedef cgi_request request;
 //} // namespace cgi
} // namespace cgi

#endif // CGI_CGI_REQUEST_HPP_INCLUDED__
