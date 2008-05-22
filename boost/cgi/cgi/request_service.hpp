//              -- cgi_service_impl.hpp --
//
//            Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_CGI_SERVICE_IMPL_HPP_INCLUDED__
#define CGI_CGI_SERVICE_IMPL_HPP_INCLUDED__

#include <map>
#include <string>
///////////////////////////////////////////////////////////
#include <boost/system/error_code.hpp>
///////////////////////////////////////////////////////////
#include "boost/cgi/cgi/request_impl.hpp"
#include "boost/cgi/connections/stdio.hpp"
#include "boost/cgi/detail/extract_params.hpp"
#include "boost/cgi/detail/cgi_service_impl_base.hpp"

namespace cgi {

  //class cgi_request_impl;

  //template<typename RequestImplType>
  class cgi_request_service
   : public cgi_service_impl_base<cgi_request_impl>
  {
  public:
    typedef cgi_request_impl   impl_type;
    typedef common::tags::cgi  protocol_type;

    cgi_request_service()
      : cgi_service_impl_base<cgi_request_impl>()
    {
    }

    template<typename T>
    cgi_request_service(T&)
      : cgi_service_impl_base<cgi_request_impl>()
    {
    }

    void construct(implementation_type& impl)
    {
      impl.client_.set_connection(
        implementation_type::connection_type::create()
      );
    }

    void destroy(implementation_type& impl)
    {
    }
  };

} // namespace cgi

#endif // CGI_CGI_SERVICE_IMPL_HPP_INCLUDED__
