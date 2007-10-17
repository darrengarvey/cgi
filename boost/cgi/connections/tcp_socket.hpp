//            -- connections/tcp_socket.hpp --
//
//            Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_CONNECTIONS_TCP_SOCKET_HPP_INCLUDED__
#define CGI_CONNECTIONS_TCP_SOCKET_HPP_INCLUDED__

#include <boost/shared_ptr.hpp>

#include "../tags.hpp"
#include "../io_service.hpp"
#include "../connection_base.hpp"
#include "../basic_connection.hpp"
#include "../detail/push_options.hpp"

namespace cgi {

  template<>
  class basic_connection<tags::tcp_socket>
    : public connection_base
  {
  public:
    typedef boost::shared_ptr<basic_connection<tags::tcp_socket> >
      pointer;

    basic_connection(io_service& ios)
      : sock_(ios)
    {
    }

    static pointer create(io_service& ios)
    {
      return static_cast<pointer>(new basic_connection<tags::tcp_socket>(ios));
    }      

    template<typename MutableBufferSequence>
    std::size_t read_some(MutableBufferSequence& buf)
    {
      return sock_.read_some(buf);
    }

    template<typename MutableBufferSequence>
    std::size_t read_some(MutableBufferSequence& buf
                         , boost::system::error_code& ec)
    {
      return sock_.read_some(buf, ec);
    }

    template<typename MutableBufferSequence, typename Handler>
    void async_read_some(MutableBufferSequence& buf, Handler handler)
    {
      sock_.async_read_some(buf, handler);
    }

    template<typename ConstBufferSequence>
    std::size_t write_some(ConstBufferSequence& buf)
    {
      return sock_.write_some(buf);
    }

    template<typename ConstBufferSequence>
    std::size_t write_some(ConstBufferSequence& buf
                          , boost::system::error_code& ec)
    {
      return sock_.write_some(buf, ec);
    }

    template<typename ConstBufferSequence, typename Handler>
    void async_write_some(ConstBufferSequence& buf, Handler handler)
    {
      sock_.async_write_some(buf, handler);
    }

    void stop()
    {
      sock_.close();
    }

  private:
    boost::asio::ip::tcp::socket sock_;
  };

  typedef basic_connection<tags::tcp_socket> tcp_connection;


  //template<typename ProtocolService = detail::fcgi_service>
  //struct tcp_connection
  //{
  //  typedef basic_connection<tags::tcp_socket, ProtocolService>    type;
  //};

} // namespace cgi

#include "../detail/pop_options.hpp"

#endif // CGI_CONNECTIONS_TCP_SOCKET_HPP_INCLUDED__
