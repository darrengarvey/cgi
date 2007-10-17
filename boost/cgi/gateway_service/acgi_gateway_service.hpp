//             -- acgi_gateway_service.hpp --
//
//            Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_ACGI_GATEWAY_SERVICE_HPP_INCLUDED__
#define CGI_ACGI_GATEWAY_SERVICE_HPP_INCLUDED__

#include "cgi_gateway_service.hpp"

namespace cgi {

  class acgi_gateway_service
    : public cgi_gateway_service
  {
  public:
    template<typename T>
    acgi_gateway_service(T&)  {}

    acgi_gateway_service()  {}

    ~acgi_gateway_service() {}
  };

} // namespace cgi

#endif // CGI_ACGI_GATEWAY_SERVICE_HPP_INCLUDED__
