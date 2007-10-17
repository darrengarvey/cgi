//                    -- request.hpp --
//
//            Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_REQUEST_HPP_INCLUDED__
#define CGI_REQUEST_HPP_INCLUDED__

#include "basic_request.hpp"
#include "tags.hpp"

namespace cgi {

  typedef basic_request<tags::cgi> request;

} // namespace cgi

#endif // CGI_REQUEST_HPP_INCLUDED__
