//               -- anonymous_pipe.hpp --
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef BOOST_CGI_WIN32_CONNECTION_INCLUDED__
#define BOOST_CGI_WIN32_CONNECTION_INCLUDED__

#include "boost/cgi/common/protocol_traits.hpp"
#include "boost/cgi/connections/anonymous_pipe.hpp"
#include "boost/cgi/connections/shareable_tcp_socket.hpp"
#include "boost/cgi/fcgi/transport_selection.hpp"

//////////////////////////////////////////////////////////////////////////

BOOST_CGI_NAMESPACE_BEGIN
  namespace common {

  /// Specialisation of basic_connection for fcgi on Windows.

  template<>
  class basic_connection<tags::fcgi_transport>
    : public connection_base
  {
  public:
    typedef basic_connection<tags::fcgi_transport> type;
    typedef boost::shared_ptr<type>                pointer;
    typedef boost::asio::ip::tcp::socket           next_layer_type;

    basic_connection(io_service& ios)
      : transport_(detail::transport_type())
    {
      if (transport_ == detail::transport::pipe)
        pipe_.reset(new basic_connection<tags::anonymous_pipe>(ios));
      else // transport_ == detail::transport::socket
        socket_.reset(new basic_connection<tags::shareable_tcp_socket>(ios));
    }

    bool is_open() const
    {
      if (transport_ == detail::transport::pipe)
        return pipe_->is_open();
      else // transport_ == detail::transport::socket
        return socket_->is_open();
    }

    void close()
    {
      if (transport_ == detail::transport::pipe)
        pipe_->close();
      else // transport_ == detail::transport::socket
        socket_->close();
    }

    static pointer create(io_service& ios)
    {
        return pointer( new basic_connection<tags::fcgi_transport>(ios));
    }      

    template<typename MutableBufferSequence>
    std::size_t read_some(MutableBufferSequence& buf)
    {
      if (transport_ == detail::transport::pipe)
        return pipe_->read_some(buf);
      else // transport_ == detail::transport::socket
        return socket_->read_some(buf);
    }

    template<typename MutableBufferSequence>
    std::size_t read_some(MutableBufferSequence& buf
                         , boost::system::error_code& ec)
    {
      if (transport_ == detail::transport::pipe)
        return pipe_->read_some(buf, ec);
      else // transport_ == detail::transport::socket
        return socket_->read_some(buf, ec);
    }

    template<typename MutableBufferSequence, typename Handler>
    void async_read_some(MutableBufferSequence& buf, Handler handler)
    {
      if (transport_ == detail::transport::pipe)
        pipe_->async_read_some(buf, handler);
      else // transport_ == detail::transport::socket
        socket_->async_read_some(buf, handler);
    }

    template<typename ConstBufferSequence>
    std::size_t write_some(ConstBufferSequence& buf)
    {
      if (transport_ == detail::transport::pipe)
        return pipe_->write_some(buf);
      else // transport_ == detail::transport::socket
        return socket_->write_some(buf);
    }

    template<typename ConstBufferSequence>
    std::size_t write_some(ConstBufferSequence& buf
                          , boost::system::error_code& ec)
    {
      if (transport_ == detail::transport::pipe)
        return pipe_->write_some(buf, ec);
      else // transport_ == detail::transport::socket
        return socket_->write_some(buf, ec);
    }

    template<typename ConstBufferSequence, typename Handler>
    void async_write_some(ConstBufferSequence& buf, Handler handler)
    {
      if (transport_ == detail::transport::pipe)
        pipe_->async_write_some(buf, handler);
      else // transport_ == detail::transport::socket
        socket_->async_write_some(buf, handler);
    }

    next_layer_type& next_layer()
    {
      return socket_->next_layer();
    }

  public:
    boost::scoped_ptr<basic_connection<common::tags::anonymous_pipe>> pipe_;
    boost::scoped_ptr<basic_connection<common::tags::shareable_tcp_socket>> socket_;
  private:

    detail::transport::type transport_;
  };
  
 } // namespace common

  namespace connections {
    typedef common::basic_connection<common::tags::fcgi_transport> fcgi_transport;
  } // namespace connections

BOOST_CGI_NAMESPACE_END

//////////////////////////////////////////////////////////////////////////

#include "boost/cgi/detail/pop_options.hpp"

#endif BOOST_CGI_WIN32_CONNECTION_INCLUDED__
