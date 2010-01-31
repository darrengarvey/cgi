//                -- protocol_traits.hpp --
//
//            Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
//
// Wow this is messy. It's not expected to last though; it's
// merely here to make things more separated than they need to
// be, so everything can be meshed together slowly.
// (or something like that)
//
/////////////////////////////////////////////////////////////////
#ifndef CGI_REQUEST_TRAITS_HPP_INCLUDED__
#define CGI_REQUEST_TRAITS_HPP_INCLUDED__

#include <boost/asio/ip/tcp.hpp>
#include <boost/none.hpp>
#include <boost/shared_ptr.hpp>
///////////////////////////////////////////////////////////
#include "boost/cgi/common/parse_options.hpp"
#include "boost/cgi/common/role_type.hpp"
#include "boost/cgi/common/tags.hpp"
#include "boost/cgi/fcgi/specification.hpp"
#include "boost/cgi/fwd/basic_client_fwd.hpp"
#include "boost/cgi/fwd/basic_connection_fwd.hpp"
#include "boost/cgi/fwd/basic_protocol_service_fwd.hpp"
#include "boost/cgi/fwd/basic_request_fwd.hpp"
#include "boost/cgi/fwd/form_parser_fwd.hpp"

BOOST_CGI_NAMESPACE_BEGIN

  namespace cgi  {}
  namespace fcgi
  {
  class fcgi_service_impl;
  class fcgi_request_service;
  class fcgi_acceptor_service;
  }
  namespace scgi
  {
  class scgi_service_impl;
  class scgi_request_service;
  class scgi_acceptor_service;
  }

  // Forward declarations

  class cgi_service_impl;
  class fcgi_service_impl;

  class fcgi_acceptor_service;

  class cgi_request_service;
  class fcgi_request_service;

 namespace detail {

   namespace tags = ::BOOST_CGI_NAMESPACE::common::tags;
   
   template<typename Protocol>
    struct protocol_traits
    {
    };

    template<>
    struct protocol_traits<tags::cgi>
    {
      typedef protocol_traits<tags::cgi>             type;
      typedef cgi_request_service                    request_service_impl;
      typedef cgi_request_service                    service_type;
      typedef common::basic_protocol_service<
                  tags::cgi
              >                                      protocol_service_type;
      typedef common::basic_request<
                  tags::cgi
              >                                      request_type; 
      typedef cgi_service_impl                       service_impl_type;
      typedef common::basic_connection<
                  tags::async_stdio
              >                                      connection_type;
      typedef common::basic_client<
                  connection_type,
                  tags::cgi
              >                                      client_type;
      typedef common::form_parser                    form_parser_type;
      typedef boost::none_t                          header_type;
      typedef char                                   char_type;
      typedef std::basic_string<char_type>           string_type;
      typedef string_type                            buffer_type;
      typedef boost::asio::const_buffers_1           const_buffers_type;
      typedef boost::asio::mutable_buffers_1         mutable_buffers_type;
      typedef common::role_type                      role_type;
      typedef boost::shared_ptr<request_type>        pointer;
      
      static const common::parse_options parse_opts = common::parse_all;
    };

    template<>
    struct protocol_traits<tags::fcgi>
    {
      typedef protocol_traits<tags::fcgi>            type;
      typedef fcgi::fcgi_request_service             request_service_impl;
      typedef fcgi::fcgi_request_service             service_type;
      typedef common::basic_protocol_service<
                  tags::fcgi
              >                                      protocol_service_type;
      typedef common::basic_request<
                  tags::fcgi
              >                                      request_type; 
      typedef boost::shared_ptr<request_type>        request_ptr;
      typedef fcgi::fcgi_service_impl                service_impl_type;
      typedef fcgi::fcgi_acceptor_service            acceptor_service_impl;
      typedef common::basic_connection<
                  tags::shareable_tcp_socket
              >                                      connection_type;
      typedef boost::asio::ip::tcp                   native_protocol_type;
      typedef boost::asio::socket_acceptor_service<
                  native_protocol_type
              >                                      acceptor_service_type;
      typedef 
          acceptor_service_type::implementation_type acceptor_impl_type;
      typedef acceptor_service_type::native_type     native_type;
      typedef unsigned short                         port_number_type;
      typedef boost::asio::ip::tcp::endpoint         endpoint_type;
      typedef common::basic_client<
                  connection_type,
                  tags::fcgi
              >                                      client_type;
      typedef common::form_parser                    form_parser_type;
      typedef fcgi::spec::header                     header_type;
      typedef fcgi::spec_detail::role_types          role_type;

      typedef char                                   char_type;
      typedef std::basic_string<char_type>           string_type;
      typedef string_type                            buffer_type;
      typedef boost::asio::const_buffers_1           const_buffers_type;
      typedef boost::asio::mutable_buffers_1         mutable_buffers_type;
      typedef boost::shared_ptr<request_type>        pointer;

      static const common::parse_options parse_opts = common::parse_none;
    };

    template<>
    struct protocol_traits<tags::scgi>
    {
      typedef protocol_traits<tags::scgi>            type;
      typedef scgi::scgi_request_service             request_service_impl;
      typedef scgi::scgi_request_service             service_type;
      typedef common::basic_protocol_service<tags::scgi>     protocol_service_type;
      typedef common::basic_request<
                  tags::scgi
      >                                              request_type; 
      typedef scgi::scgi_service_impl                service_impl_type;
      typedef scgi::scgi_acceptor_service            acceptor_service_impl;
      typedef common::basic_connection<tags::tcp_socket>     connection_type;
      typedef common::basic_client<
                  connection_type,
                  tags::scgi
              >                                      client_type;
      typedef common::form_parser                    form_parser_type;
      typedef char                                   char_type;
      typedef std::basic_string<char_type>           string_type;
      typedef string_type                            buffer_type;
      typedef boost::asio::const_buffers_1           const_buffers_type;
      typedef boost::asio::mutable_buffers_1         mutable_buffers_type;
      typedef boost::shared_ptr<request_type>        pointer;

      static const common::parse_options parse_opts = common::parse_none;
    };

 } // namespace detail
BOOST_CGI_NAMESPACE_END

#endif // CGI_REQUEST_TRAITS_HPP_INCLUDED__
