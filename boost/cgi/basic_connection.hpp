//                -- basic_connection.hpp --
//
//            Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_BASIC_CONNECTION_HPP_INCLUDED__
#define CGI_BASIC_CONNECTION_HPP_INCLUDED__

//#include <istream>
//#include <ostream>
//#include <boost/asio.hpp>

//#include "boost/cgi/tags.hpp"
//#include "boost/cgi/connection_base.hpp"
//#include "connections/async_stdio_connection_impl.hpp"

namespace cgi {
 namespace common {

  template<typename ConnectionType>
  class basic_connection;

/*
    : public connection_base
  //, connection_wrapper<ConnectionType>
  {
  public:
  //  typedef ConnectionType  impl_type;
  //  //typedef boost::shared_ptr<conection_base>                 pointer;

  //  explicit basic_connection(protocol_service_type& ps)
  //    : impl_(ps)
  //  {
  //  }

  //  static pointer create()
  //  {
  //    return new basic_connection<protocol_service_type
  //                               , connection_type>(impl_.protocol_service());
  //  }

  //private:
  //  impl_type impl_;
  };

*/

  //  template<typename ProtocolService>
  //class async_stdio_connection_impl;

  //template<typename ProtocolService>
  //class stdio_connection_impl;


  //#include "connections/stdio_connection_impl.hpp"

  //template<typename ProtocolService>
  // class basic_connection<tags::stdio, ProtocolService>
  //  : public stdio_connection_impl<ProtocolService>
  //{
  //};

  //typedef basic_connection<tags::stdio, stdio_connection


  //#include "connections/async_stdio_connection_impl.hpp"

  //template<typename ProtocolService>
  //class basic_connection<tags::async_stdio, ProtocolService>
  //  : public async_stdio_connection_impl<ProtocolService>
  //{
  //};

  //typedef basic_connection<tags::tcp>      tcp_connection;
  //typedef basic_connection<tags::pipe>     pipe_connection; // not implemented
//#include "connections/stdio_connection.hpp"
//#include "connections/async_stdio_connection.hpp"
//#include "connections/tcp_connection.hpp"

 } // namespace common
} // namespace cgi

#endif // CGI_BASIC_CONNECTION_HPP_INCLUDED__
