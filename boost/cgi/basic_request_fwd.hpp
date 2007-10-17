//              -- basic_request_fwd.hpp --
//
//            Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_BASIC_REQUEST_FWD_HPP_INCLUDED__
#define CGI_BASIC_REQUEST_FWD_HPP_INCLUDED__

#include <memory>

#include "request_service_fwd.hpp"
#include "role_type.hpp"
#include "basic_protocol_service_fwd.hpp"

namespace cgi {

  template<typename RequestService
          , typename ProtocolService
          , enum role_type Role = responder
          , typename Allocator = std::allocator<char> >
  class basic_request;

} // namespace cgi

#endif // CGI_BASIC_REQUEST_FWD_HPP_INCLUDED__
