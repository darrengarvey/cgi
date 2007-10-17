//              -- scgi_gateway_service.hpp --
//
//            Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_SCGI_GATEWAY_SERVICE_HPP_INCLUDED__
#define CGI_SCGI_GATEWAY_SERVICE_HPP_INCLUDED__

#include <set>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>

#include "../connections/tcp_socket.hpp"

namespace cgi {

  class scgi_gateway_service
  {
  public:

    struct implementation_type
    {
      typedef tcp_connection                      connection_type;
      typedef boost::shared_ptr<connection_type>  connection_ptr;
    private:
      std::set<connection_ptr> connections_;
    };

    scgi_gateway_service()  {}
    ~scgi_gateway_service() {}

    template<typename T> void construct(T&) {}
    template<typename T> void destroy(T&)   {}
    template<typename T> void start(T&)     {}
    template<typename T> void stop(T&)      {}

    void stop(implementation_type& impl)
    {
      std::for_each(impl.connections_.begin(), impl.connections_.end()
                   , boost::bind(&implementation_type::connection_type::stop
                                , *_1));
    }

    void reset(implementation_type& impl)
    {
      stop(impl);
    }
  };

} // namespace cgi

#endif // CGI_SCGI_GATEWAY_SERVICE_HPP_INCLUDED__
