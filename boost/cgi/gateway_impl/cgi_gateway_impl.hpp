//               -- cgi_gateway_impl.hpp --
//
//            Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_CGI_GATEWAY_IMPL_HPP_INCLUDED__
#define CGI_CGI_GATEWAY_IMPL_HPP_INCLUDED__

#include <boost/noncopyable.hpp>

namespace cgi {

  // Forward declaration
  class cgi_gateway_service;


  class cgi_gateway_impl
    : private boost::noncopyable
  {
  public:
    cgi_gateway_impl()  {}
    ~cgi_gateway_impl() {}
  protected:
    friend class cgi_gateway_service;
  };

} // namespace cgi

#endif // CGI_CGI_GATEWAY_IMPL_HPP_INCLUDED__
