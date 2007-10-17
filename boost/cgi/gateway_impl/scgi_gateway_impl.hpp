//               -- scgi_gateway_impl.hpp --
//
//            Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_SCGI_GATEWAY_IMPL_HPP_INCLUDED__
#define CGI_SCGI_GATEWAY_IMPL_HPP_INCLUDED__

#include <boost/noncopyable.hpp>

namespace cgi {

  // Forward declaration
  class scgi_gateway_service;


  class scgi_gateway_impl
    : private boost::noncopyable
  {
  public:
    scgi_gateway_impl()  {}
    ~scgi_gateway_impl() {}
  protected:
    friend class scgi_gateway_service;
  };

} // namespace cgi

#endif // CGI_SCGI_GATEWAY_IMPL_HPP_INCLUDED__
