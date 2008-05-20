//                 -- fcgi/client.hpp --
//
//            Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_FCGI_CLIENT_HPP_INCLUDED__
#define CGI_FCGI_CLIENT_HPP_INCLUDED__

#include <vector>
///////////////////////////////////////////////////////////
#include <boost/shared_ptr.hpp>
#include <boost/logic/tribool.hpp>
#include <boost/asio/buffer.hpp>
///////////////////////////////////////////////////////////
#include "boost/cgi/tags.hpp"
#include "boost/cgi/read.hpp"
#include "boost/cgi/error.hpp"
#include "boost/cgi/common/map.hpp"
#include "boost/cgi/basic_client.hpp"
#include "boost/cgi/common/buffer.hpp"
#include "boost/cgi/common/io_service.hpp"
#include "boost/cgi/basic_request_fwd.hpp"
#include "boost/cgi/fcgi/specification.hpp"
#include "boost/cgi/detail/throw_error.hpp"
#include "boost/cgi/detail/protocol_traits.hpp"
#include "boost/cgi/connections/shareable_tcp_socket.hpp"

namespace cgi {
 namespace common {

  enum client_status
  {
    none_, // **FIXME** !
    constructed,
    params_read,
    stdin_read,
    end_request_sent,
    closed_, // **FIXME** !
    //aborted
  };

  /// A client that uses a TCP socket that owned by it.
  template<typename Protocol>
  class basic_client<common::shareable_tcp_connection, Protocol>
  {
  public:
    typedef ::cgi::io_service                 io_service_type;
    typedef ::cgi::common::map                map_type;
    typedef Protocol                          protocol_type;
    typedef common::shareable_tcp_connection  connection_type;
    //typedef detail::protocol_traits<
    //  fcgi_
    //>::request_impl_type                      request_impl_type;
    //struct connection_type : shareable_tcp_connection
    //{ typedef boost::shared_ptr<connection_type> pointer; }
    typedef typename connection_type::pointer          connection_ptr;
    typedef boost::array<
        unsigned char
      , fcgi::spec::header_length::value
    >                                         header_buffer_type;
    typedef boost::asio::mutable_buffers_1    mutable_buffers_type;
    typedef fcgi::spec_detail::role_t         role_type;

    /// Construct
    basic_client()
      : request_id_(-1)
      , status_(none_)
      , keep_connection_(false)
    {
    }

    /// Construct
    basic_client(io_service_type& ios)
      : request_id_(-1)
      , status_(none_)
      , keep_connection_(false)
      //, io_service_(ios)
      //, connection_(new connection_type::pointer(ios))
    {
    }

    /// Destroy
    /** Closing the connection as early as possible is good for efficiency */
    ~basic_client()
    {
      close();
    }

    /// Construct the client by claiming a request id.
    /**
     * Before loading a request, it will usually not have a request id. This
     * function reads headers (and corresponding bodies if necessary) until
     * a BEGIN_REQUEST record is found. The calling request then claims and
     * serves that request.
     */
    template<typename RequestImpl>
    boost::system::error_code
      construct(RequestImpl& req, boost::system::error_code& ec)
    {
      status_ = constructed;

      return ec;
    }

    bool is_open() const
    {
      return connection_->is_open();
    }

    void close(boost::uint64_t app_status = 0)
    {
      boost::system::error_code ec;
      close(app_status, ec);
      detail::throw_error(ec);
    }

    boost::system::error_code
      close(boost::uint64_t app_status, boost::system::error_code& ec)
    {
      if (status_ == closed_) return ec;

      std::vector<boost::asio::const_buffer> bufs;

      // Write an EndRequest packet to the server.
      out_header_[0] = static_cast<unsigned char>(1); // FastCGI version
      out_header_[1] = static_cast<unsigned char>(3); // END_REQUEST
      out_header_[2] = static_cast<unsigned char>(request_id_ >> 8) & 0xff;
      out_header_[3] = static_cast<unsigned char>(request_id_) & 0xff;
      out_header_[4] = static_cast<unsigned char>(8 >> 8) & 0xff;
      out_header_[5] = static_cast<unsigned char>(8) & 0xff;
      out_header_[6] = static_cast<unsigned char>(0);
      out_header_[7] = 0;

      //BOOST_ASSERT(role_ == fcgi::spec_detail::RESPONDER
      //             && "Only supports Responder role for now (**FIXME**)");

      header_buffer_type end_request_body =
      {{
          static_cast<unsigned char>(app_status >> 24) & 0xff
        , static_cast<unsigned char>(app_status >> 16) & 0xff
        , static_cast<unsigned char>(app_status >>  8) & 0xff
        , static_cast<unsigned char>(app_status >>  0) & 0xff
        , static_cast<unsigned char>(fcgi::spec_detail::REQUEST_COMPLETE)
        , static_cast<unsigned char>(0)
        , static_cast<unsigned char>(0)
        , static_cast<unsigned char>(0)
      }};

      bufs.push_back(buffer(out_header_));
      bufs.push_back(buffer(end_request_body));

      write(*connection_, bufs, boost::asio::transfer_all(), ec);

      if (!keep_connection_)
        connection_->close();

      return ec;
    }

    //io_service_type& io_service() { return io_service_; }

    /// Associate a connection with this client
    /**
     * Note: the connection must have been created using the new operator
     */
    bool set_connection(connection_type* conn)
    {
      // make sure there isn't already a connection associated with the client
      //if (!connection_) return false;
      connection_.reset(conn);
      return true;
    }

    /// Associate a connection with this client
    bool set_connection(const connection_type::pointer& conn)
    {
      // make sure there isn't already a connection associated with the client
      //if (!connection_) return false;
      connection_ = conn;
      return true;
    }

    /// Get a shared_ptr of the connection associated with the client.
    connection_type::pointer&
      connection() { return connection_; }

    /// Write some data to the client.
    template<typename ConstBufferSequence>
    std::size_t 
      write_some(const ConstBufferSequence& buf, boost::system::error_code& ec)
    {
      typename ConstBufferSequence::const_iterator iter = buf.begin();
      typename ConstBufferSequence::const_iterator end  = buf.end(); 

      std::vector<boost::asio::const_buffer> bufs;
      bufs.push_back(boost::asio::const_buffer());//header_buf_)); // space for header

      int total_buffer_size(0);
      for(; iter != end; ++iter)
      {
        boost::asio::const_buffer buffer(*iter);
        int new_buf_size( boost::asio::buffer_size(buffer) );
        if (total_buffer_size + new_buf_size
             > fcgi::spec::max_packet_size::value)
          break;
        total_buffer_size += new_buf_size;
        bufs.push_back(*iter);
      }
      std::cerr<< "buffer size := " << total_buffer_size << std::endl;
      //detail::make_header(out_header_, buf
      out_header_[0] = static_cast<unsigned char>(1);
      out_header_[1] = static_cast<unsigned char>(6);
      out_header_[2] = static_cast<unsigned char>(request_id_ >> 8) & 0xff;
      out_header_[3] = static_cast<unsigned char>(request_id_) & 0xff;
      out_header_[4] = static_cast<unsigned char>(total_buffer_size >> 8) & 0xff;
      out_header_[5] = static_cast<unsigned char>(total_buffer_size) & 0xff;
      out_header_[6] = static_cast<unsigned char>(0);
      out_header_[7] = 0;

      bufs.front() = boost::asio::buffer(out_header_);
      std::size_t bytes_transferred
        = boost::asio::write(*connection_, bufs, boost::asio::transfer_all(), ec);

      if (0 != (total_buffer_size + fcgi::spec::header_length::value
          - bytes_transferred))
        ec = error::couldnt_write_complete_packet;

      return bytes_transferred;
    }

    /// Asynchronously write some data to the client.
    template<typename ConstBufferSequence, typename Handler>
    void async_write_some(const ConstBufferSequence& buf, Handler handler)
    {
      connection_->async_write_some(buf, handler);
    }

    /// Asynchronously read some data from the client.
    template<typename MutableBufferSequence, typename Handler>
    void async_read_some(const MutableBufferSequence& buf, Handler handler)
    {
      connection_->async_read_some(buf, handler);
    }


    const client_status& status() const
    {
      return status_;
    }

    bool keep_connection() const
    {
      return keep_connection_;
    }

    //int id() { return request_id_; }

  public:
    friend class fcgi_request_service;
    boost::uint16_t request_id_;
    client_status status_;
    std::size_t bytes_left_;
    //request_impl_type* current_request_;
    
    /// A marker to say if the final STDIN (and/or DATA) packets have been
    // read. Note: having data on the connection doesn't imply it's for
    // this request; we can save time by knowing when to not even try.
    //bool closed_;

    connection_ptr   connection_;

    //fcgi::spec_detail::Header hdr_;
    /// Buffer used to check the header of each packet.
    header_buffer_type out_header_;

    bool keep_connection_;
    role_type role_;

  public:

    //template<typename Request>
    //Request& lookup_request(boost::uint16_t id)
    //{


            //*/
  };

//#include "boost/cgi/fcgi/client_fwd.hpp"

 } // namespace common

namespace fcgi {
    typedef
      common::basic_client<
        common::shareable_tcp_connection, ::cgi::fcgi_
      >
    client;
} // namespace fcgi



}// namespace cgi

#endif // CGI_FCGI_CLIENT_HPP_INCLUDED__

