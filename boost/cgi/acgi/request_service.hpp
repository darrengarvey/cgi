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

#include "boost/cgi/acgi/request_impl.hpp"
#include "boost/cgi/tags.hpp"
#include "boost/cgi/io_service.hpp"
#include "boost/cgi/common/map.hpp"
#include "boost/cgi/detail/cgi_service_impl_base.hpp"
#include "boost/cgi/detail/service_base.hpp"
#include "boost/cgi/detail/extract_params.hpp"
#include "boost/cgi/connections/async_stdio.hpp"

namespace cgi {

  //template<typename ProtocolService>
  class acgi_request_service
  //: public boost::asio::io_service::service
    : public cgi_service_impl_base<acgi_request_impl>
    , public detail::service_base<acgi_request_service>
  {
  public:
    typedef acgi_request_service        type;
    typedef tags::acgi                  protocol_type;
    typedef acgi_service                protocol_service_type;

    /// The unique service identifier
    //    static boost::asio::io_service::id id;

    acgi_request_service(::cgi::io_service& ios)
      : detail::service_base<acgi_request_service>(ios)
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

} // namespace cgi

#endif // CGI_ASYNC_CGI_SERVICE_IMPL_HPP_INCLUDED__
