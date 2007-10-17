//               -- acgi_gateway_impl.hpp --
//
//            Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_ACGI_GATEWAY_IMPL_HPP_INCLUDED__
#define CGI_ACGI_GATEWAY_IMPL_HPP_INCLUDED__

#include "cgi_gateway_impl.hpp"

namespace cgi {

  // Forward declaration
  class acgi_gateway_service;


  class acgi_gateway_impl
    : public cgi_gateway_impl
  {
  public:
    acgi_gateway_impl()  {}
    ~acgi_gateway_impl() {}
  private:
    friend class acgi_gateway_service;
  };

} // namespace cgi

#endif // CGI_ACGI_GATEWAY_IMPL_HPP_INCLUDED__
