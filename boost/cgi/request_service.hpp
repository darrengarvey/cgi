//               -- request_service.hpp --
//
//            Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_REQUEST_SERVICE_HPP_INCLUDED
#define CGI_REQUEST_SERVICE_HPP_INCLUDED

#include "detail/push_options.hpp"

#include <boost/utility/enable_if.hpp>

//#include "is_async.hpp"
#include "io_service.hpp"
#include "detail/protocol_traits.hpp"
#include "basic_protocol_service_fwd.hpp"
#include "detail/service_base.hpp"
//#include "service_selector.hpp"

namespace cgi {

  /// The generic service class for basic_request<>s
  /**
   * Note: If the protocol is an asynchronous protocol, which means it requires
   * access to a boost::asio::io_service instance, then this class becomes a
   * model of the Service concept (**LINK**) and must only use the constructor
   * which takes a ProtocolService (**LINK**). If the protocol isn't async then
   * the class can be used without a ProtocolService.
   */
  template<typename Protocol>
  class request_service
    : public detail::service_base<request_service<Protocol> >
  {
    // The platform-specific implementation (only one for now)
    typedef typename detail::protocol_traits<Protocol>::service_impl_type
      service_impl_type;

  public:
    typedef typename service_impl_type::impl_type     impl_type;
    typedef typename service_impl_type::implementation_type 
                                                      implementation_type;
    typedef Protocol                                  protocol_type;
    typedef basic_protocol_service<Protocol>  protocol_service_type;

    /// The unique service identifier
    //static boost::asio::io_service::id id;
    //explicit request_service()
    //{
    //}

    request_service(cgi::io_service& ios)
      : detail::service_base<request_service<Protocol> >(ios)
      , service_impl_(boost::asio::use_service<service_impl_type>(ios))
    {
    }

    request_service(protocol_service_type& ps)
      : detail::service_base<request_service<Protocol> >(ps.io_service())
      , service_impl_(boost::asio::use_service<service_impl_type>(ps.io_service()))
    {
    }

    void construct(impl_type& impl)
    {
      service_impl_.construct(impl);
    }

    void destroy(impl_type& impl)
    {
      service_impl_.destroy(impl);
    }

    void shutdown_service()
    {
      service_impl_.shutdown_service();
    }

    impl_type null() const
    {
      return service_impl_.null();
    }

    //void construct

    boost::system::error_code& load(impl_type& impl, bool parse_stdin
                                   , boost::system::error_code& ec)
    {
      return service_impl_.load(impl, parse_stdin, ec);
    }


    template<typename Handler>
    void async_load(impl_type& impl, bool parse_stdin, Handler handler)
    {
      service_impl_.async_load(impl, parse_stdin, handler);
    }

    bool is_open(impl_type& impl)
    {
      return service_impl_.is_open(impl);
    }

    boost::system::error_code&
    set_header(impl_type& impl, const std::string& name
              , const std::string& value, boost::system::error_code& ec)
    {
      return service_impl_.set_header(impl, name, value, ec);
    }
      

    template<typename ConstBufferSequence>
    std::size_t write_some(impl_type& impl, const ConstBufferSequence& buf
                          , boost::system::error_code& ec)
    {
      return service_impl_.write_some(impl, buf, ec);
    }

    template<typename MutableBufferSequence>
    std::size_t read_some(impl_type& impl, MutableBufferSequence buf
                          , boost::system::error_code& ec)
    {
      return service_impl_.read_some(impl, buf, ec);
    }

    std::string meta_get(impl_type& impl, const std::string& name
                        , boost::system::error_code& ec)
    {
      return service_impl_.meta_get(impl, name, ec);
    }

    std::string meta_post(impl_type& impl, const std::string& name
                         , boost::system::error_code& ec)
    {
      return service_impl_.meta_post(impl, name, ec);
    }

    std::string cookie(impl_type& impl, const std::string& name
                      , boost::system::error_code& ec)
    {
      return service_impl_.cookie(impl, name, ec);
    }

  private:
    service_impl_type& service_impl_;
  };

} // namespace cgi

#include "detail/pop_options.hpp"

#endif // CGI_REQUEST_SERVICE_HPP_INCLUDED
