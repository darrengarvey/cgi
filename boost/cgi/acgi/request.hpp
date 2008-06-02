//                 -- acgi/request.hpp --
//
//            Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_ACGI_REQUEST_HPP_INCLUDED__
#define CGI_ACGI_REQUEST_HPP_INCLUDED__

#include "boost/cgi/detail/protocol_traits.hpp"
#include "boost/cgi/common/tags.hpp"
#include "boost/cgi/acgi/service.hpp"
#include "boost/cgi/acgi/request_impl.hpp"
#include "boost/cgi/acgi/request_service.hpp"
#include "boost/cgi/fwd/basic_request_fwd.hpp"

namespace cgi {
 namespace acgi {

  typedef
    common::basic_request<
      acgi::request_service, acgi::service
    >
  request;

 } // namespace acgi
} // namespace cgi

//namespace boost { namespace acgi {
//  typedef ::cgi::basic_request< cgi::acgi_request_service, cgi::acgi_service> request;
//} }

#include "boost/cgi/basic_request.hpp"

#endif // CGI_ACGI_REQUEST_HPP_INCLUDED__
