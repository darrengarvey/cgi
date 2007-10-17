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

#include "../tags.hpp"
#include "../basic_request.hpp"

namespace cgi {

  class scgi_request_service;

  typedef basic_request<scgi_request_service> scgi_request;

} // namespace cgi

#endif // CGI_SCGI_REQUEST_HPP_INCLUDED__
