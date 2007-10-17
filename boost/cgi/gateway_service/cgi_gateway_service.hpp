//              -- cgi_gateway_service.hpp --
//
//            Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_CGI_GATEWAY_SERVICE_HPP_INCLUDED__
#define CGI_CGI_GATEWAY_SERVICE_HPP_INCLUDED__

namespace cgi {

  class cgi_gateway_service
  {
  public:
    cgi_gateway_service()  {}
    ~cgi_gateway_service() {}

    template<typename T> void construct(T&) {}
    template<typename T> void destroy(T&)   {}
    template<typename T> void start(T&)     {}
    template<typename T> void stop(T&)      {}
    void                      stop()        {}
    void                      reset()       {}
  };

} // namespace cgi

#endif // CGI_CGI_GATEWAY_SERVICE_HPP_INCLUDED__
