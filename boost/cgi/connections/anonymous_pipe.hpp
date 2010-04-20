//               -- anonymous_pipe.hpp --
//
//          Copyright (c) Darren Garvey 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef BOOST_CGI_ANONYMOUS_PIPE_INCLUDED_20100204
#define BOOST_CGI_ANONYMOUS_PIPE_INCLUDED_20100204

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include "boost/cgi/error.hpp"
#include "boost/cgi/common/tags.hpp"
#include "boost/cgi/basic_connection.hpp"
#include "boost/cgi/import/io_service.hpp"
#include "boost/cgi/detail/push_options.hpp"
#include "boost/cgi/common/connection_base.hpp"
#include "boost/cgi/common/protocol_traits.hpp"

//////////////////////////////////////////////////////////////////////////

BOOST_CGI_NAMESPACE_BEGIN
  namespace common {

  /// Specialisation of basic_connection for anonymous pipes on Windows.
  /**
   * For default initialisation of FastCGI to work on Windows with mod_fcgid
   * and Apache, we need to support anonymous pipes. This class is used to
   * allow the support.
   */
  template<>
  class basic_connection<tags::anonymous_pipe>
    : public connection_base
  {
    enum { max_buffers = 64 };
  public:
    typedef basic_connection<tags::anonymous_pipe> self_type;
    typedef boost::shared_ptr<self_type>           pointer;

	  basic_connection(boost::asio::io_service& ios)
      : io_service(ios)
      , file_handle(INVALID_HANDLE_VALUE)
    {
    }

    static pointer create(boost::asio::io_service& ios) {
      return pointer(new self_type(ios));
    }

    bool is_open() { return file_handle != INVALID_HANDLE_VALUE; }

    int close()
    {
      /*
       * Make sure that the client (ie. a Web Server in this case) has
       * read all data from the pipe before we disconnect.
       */
      if (!::FlushFileBuffers(file_handle)) return -1;
      if (!::DisconnectNamedPipe(file_handle)) return -1;
      file_handle = INVALID_HANDLE_VALUE;
      return 0;
    }

    template<typename MutableBufferSequence>
    std::size_t read_some(const MutableBufferSequence& buffers)
    {
      boost::system::error_code ec;
      std::size_t bytes_read = read_some(buffers, ec);
      detail::throw_error(ec);
      return bytes_read;
    }

    template<typename MutableBufferSequence>
    std::size_t read_some(const MutableBufferSequence& buffers
                         , boost::system::error_code& ec)
    {
      ec = boost::system::error_code();
      typename MutableBufferSequence::const_iterator iter = buffers.begin();
      typename MutableBufferSequence::const_iterator end = buffers.end();
      DWORD i = 0;
      size_t buffer_size = 0;
      size_t bytes_transferred = 0;
      size_t total_bytes_transferred = 0;
      for (; !ec && iter != end && i < max_buffers; ++iter, ++i)
      {
        boost::asio::mutable_buffer buffer(*iter);
        buffer_size = boost::asio::buffer_size(buffer);
        bytes_transferred = read_some(
          boost::asio::buffer_cast<char*>(buffer), buffer_size , ec);
        if (bytes_transferred < buffer_size)
          return bytes_transferred;
        total_bytes_transferred += buffer_size;
      }
      return total_bytes_transferred;
    }

    std::size_t read_some(char* buf, std::size_t len
        , boost::system::error_code& ec)
    {
      ec = boost::system::error_code();
      DWORD bytesRead;
      int ret = -1;

      if (::ReadFile(file_handle, buf, len, &bytesRead, NULL))
        ret = bytesRead;
      else
        ec = boost::system::error_code(
                ::GetLastError(), boost::system::system_category);

      return ret;
    }

    template<typename ConstBufferSequence>
    std::size_t write_some(ConstBufferSequence& buf
                          , boost::system::error_code& ec)
    {
      ec = boost::system::error_code();
      std::size_t bytes_transferred(0);
      for(typename ConstBufferSequence::const_iterator i = buf.begin(),
          end (buf.end())
         ; !ec && i != end; ++i)
      {
        std::size_t buf_len = boost::asio::buffer_size(*i);
        bytes_transferred += write_some(boost::asio::buffer_cast<const char *>(*i), buf_len, ec);
      }
      return bytes_transferred;
    }

    std::size_t write_some(const char* buf, std::size_t len
                          , boost::system::error_code& ec)
    {
      ec = boost::system::error_code();
      DWORD bytesWritten;
      int ret = -1;

      if (::WriteFile(file_handle, buf, len, &bytesWritten, NULL))
        ret = bytesWritten;
      else
        ec = boost::system::error_code(
                ::GetLastError(), boost::system::system_category);

      return ret;
    }

    boost::asio::io_service& io_service;
    HANDLE file_handle;
  };

 } // namespace common
BOOST_CGI_NAMESPACE_END

//////////////////////////////////////////////////////////////////////////

#include "boost/cgi/detail/pop_options.hpp"

#endif // BOOST_CGI_ANONYMOUS_PIPE_INCLUDED_20100204
