//               -- acgi_service_impl.hpp --
//
//            Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_ACGI_SERVICE_IMPL_HPP_INCLUDED__
#define CGI_ACGI_SERVICE_IMPL_HPP_INCLUDED__

#include "boost/cgi/common/tags.hpp"
#include "boost/cgi/common/map.hpp"
#include "boost/cgi/acgi/request_impl.hpp"
#include "boost/cgi/import/io_service.hpp"
#include "boost/cgi/detail/service_base.hpp"
#include "boost/cgi/detail/extract_params.hpp"
#include "boost/cgi/connections/async_stdio.hpp"
#include "boost/cgi/detail/cgi_service_impl_base.hpp"

namespace cgi {
 namespace acgi {

  class request_service
    : public cgi_service_impl_base<acgi::request_impl>
    , public detail::service_base<request_service>
  {
  public:
    typedef request_service             type;
    typedef common::tags::acgi          protocol_type;
    typedef acgi::service               protocol_service_type;

    request_service(common::io_service& ios)
      : detail::service_base<request_service>(ios)
    {
    }

    void shutdown_service()
    {
    }

    void construct(implementation_type& impl)
    {
      impl.client_.set_connection(
        implementation_type::connection_type::create(this->io_service())
      );
    }

    void destroy(implementation_type& impl)
    {
    }
    
    void set_service(implementation_type& impl, protocol_service_type& ps)
    {
      impl.service_ = &ps;
    }
  };

 } // namespace acgi
} // namespace cgi

#endif // CGI_ASYNC_CGI_SERVICE_IMPL_HPP_INCLUDED__
