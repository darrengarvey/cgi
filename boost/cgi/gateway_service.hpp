//                -- gateway_service.hpp --
//
//            Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_GATEWAY_SERVICE_HPP_INCLUDED__
#define CGI_GATEWAY_SERVICE_HPP_INCLUDED__

namespace cgi {


  template<typename Protocol
          , typename ProtocolService = basic_protocol_service<Protocol> >
  class gateway_service
  {
  public:
    typedef Protocol                              protocol_type;
    typedef ProtocolService                     io_service_type;
    typedef boost::shared_ptr<connection_type>         conn_ptr;


    gateway_service(io_service_type& ios)
      : io_service_(ios)
    {
    }

    void stop(impl_type& impl)
    {


    //boost::system::error_code&
    //accept(impl_type& impl, conn_ptr conn, boost::system::error_code& ec)
    //{
    //  boost::asio::acceptor(
    //}

    //boost::system::error_code&
    //accept(conn_ptr conn, boost::system::error_code& ec)
    //{
    //  conn = create_connection();

    //  if( accept_impl(conn, ec) )
    //  {
    //    io_service_.connections_.insert(conn);
    //    return ec;
    //  }

    //  // else change the current connection type held by the service, and try again
    //  // (when implemented by asio & server front-ends)
    //  // for now just set an error and return
    //  ec = boost::system::system_error(666);
    //  return ec;
    //}

    //template<typename Handler>
    //class accept_handler
    //{
    //public:
    //  accept_handler(Handler handler)
    //    : handler_(handler)
    //  {
    //  }

    //  void operator()(const boost::system::error_code& ec)
    //  {
    //    handler_(ec);
    //  }

    //private:
    //  Handler handler_;
    //};

    //template<typename Handler>
    //void async_accept(Handler handler)
    //{
    //}    

  private:

    /// Create a connection of the correct type
    /**
     * For now this only creates one type of connection (either a tcp socket for FCGI,
     * or a cin/cout wrapper for vanilla CGI).
     *
     * It will be necessary once asio has the capabilities, to allow for different
     * connection types here. This should be relatively simple assuming conn_ptr is
     * a shared_ptr of a connection_base and all connection types inherit from this.
     */
    conn_ptr create_connection()
    {
      return new tcp_connection(io_service_);
    }

    io_service_type& io_service_;
  };


  template<> boost::system::error_code&
  gateway_service<protocol::cgi>::accept( gateway_service<protocol::cgi>::conn_ptr conn
				        , boost::system::error_code& ec
				        )
  {
    conn = create_connection();
  }

  template<> boost::shared_ptr<connection_base>
  gateway_service<protocol::cgi>::create_connection()
  {
    return new stdio_connection(gateway_);
  }

} // namespace cgi

#endif // CGI_GATEWAY_SERVICE_HPP_INCLUDED__
