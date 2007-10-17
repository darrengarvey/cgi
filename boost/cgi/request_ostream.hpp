//                -- request_ostream.hpp --
//
//            Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_REQUEST_OSTREAM_HPP_INCLUDED__
#define CGI_REQUEST_OSTREAM_HPP_INCLUDED__

/*********************************
ISSUES:
-------
* async_flush()
The other flush() functions clear the buffer after flushing the data to the
request. It's trickier with async_flush(), but for now the buffer is still
cleared after the flush completes. This means you can't write to the ostream
until an async_flush completes; that's probably true of any async operation
however so it seems a reasonable restriction.

* Is ostream the best name for this? request_ostream?

* This class could do more: the functions basic_request<>::write() could write
to the request unbuffered. ie. a write call would instantly wrap the supplied
buffer in suitable headers and send it straight away, rather than buffering it
first. That would also mean the ostream class is of more use than it is now.

* This should probably derive from std::ostream, as users would probably expect
that.
*********************************/

#include "detail/push_options.hpp"

#include <ostream>
#include <sstream>
#include <vector>
//#include <streambuf>
#include <boost/assert.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/system/error_code.hpp>
#include <boost/asio.hpp>

#include "streambuf.hpp"
#include "buffer.hpp"
#include "write.hpp"
#include "tags.hpp"
#include "data_sink.hpp"
#include "request_base.hpp"
#include "basic_request.hpp"
#include "http/status_code.hpp"
//#include "detail/take_buffer.hpp"

namespace cgi {

  /* Notes
   * -----
   *
   * The constructor could take an additional bool which determines
   * if any default headers are prepended to the message on sending
   * eg. 'Content-type: text/plain' could by default be added, making
   * the entry barrier lower and testing simpler.
   */


  /// The ostream class: a stream interface for writing to requests
  /**
   * This is simply a wrapper around an ostream with a few extra details that
   * aid writing to a request's output or error sinks.
   *
   * This is a generalisation of the cgi::reply and cgi::logger classes.
   */
  class request_ostream
    // derive from std::ostream? (yes, basically)
  {
  public:
    /// Default constructor
    request_ostream(http::status_code sc = http::ok)
      : buffer_(new cgi::streambuf())
      , ostream_(buffer_.get())
      , http_status_(sc)
      , headers_sent_(false)
    {
    }

    /// Construct with a particular buffer
    /**
     * Takes the buffer and uses it internally, does nothing with it on
     * destruction.
     */
    request_ostream(cgi::streambuf* buf, http::status_code sc = http::ok)
      : /*request_(NULL)
          , */ostream_(buf)
      , http_status_(sc)
    {
    }

    /// Construct, taking a buffer from an external source
    /**
     * Gets a buffer from the request/protocol service held by the request.
     * <strike>
     * Takes a buffer from T (can be a model of ProtocolService or
     * CommonGatewayRequest) to use internally.
     * </strike>
     */
//    template<typename CommonGatewayRequest>
//    request_ostream(CommonGatewayRequest& req, http::status_code sc = http::ok)
//      : request_(&req)
//      , buffer_(new cgi::streambuf())
//      , ostream_(buffer_.get()) //detail::take_buffer(req))
//      , http_status_(sc)
////    , destination_(destination)
//    {
//    }

    ~request_ostream()
    {
      //if (request_) send();
    }

    void clear()
    {
      ostream_.clear();
      headers_.clear();
    }

    // provide this too?
    std::size_t write(const char* str, std::size_t len)
    {
      ostream_.write(str, len);
      return len;
    }

    std::size_t write(const std::string& str)
    {
      return write(str.c_str(), str.size());
    }

    template<typename ConstBufferSequence>
    std::size_t write(const ConstBufferSequence& buf)
    {
      //ostream_.write(buf.data(), buf.size());
      return buf.size();
    }

    /// Synchronously flush the data to the current request
    /**
     * If there is no error, the buffer is cleared.
     */
    //void flush()
    //{
    //  BOOST_ASSERT(request_ != NULL);
    //  flush(*request_);
    //}

    /// Synchronously flush the data to the supplied request
    /**
     * This call uses throwing semantics. ie. an exception will be thrown on
     * any failure.
     * If there is no error, the buffer is cleared.
     */
    template<typename CommonGatewayRequest>
    void flush(CommonGatewayRequest& req)
    {
      if (!headers_sent_)
      {
        ostream_<< "Content-type: text/plain\r\n\r\n";
        headers_sent_ = true;
      }
      cgi::write(req, headers_);
      cgi::write(req, rdbuf()->data());
      // the above function will throw on an error
      clear();
    }

    /// Synchronously flush the data via the supplied request
    /**
     * This call uses error_code semantics. ie. ec is set if an error occurs.
     * If there is no error, the buffer is cleared.
     */
    template<typename CommonGatewayRequest>
    boost::system::error_code& flush(CommonGatewayRequest& req
                                    , boost::system::error_code& ec)
    {
      if (!headers_sent_)
      {
        ostream_<< "Content-type: text/plain\r\n\r\n";
        headers_sent_ = true;
      }
      if(!cgi::write(req, rdbuf()->data(), ec))
        clear();
      return ec;
    }

    // Class for doing post-flush housekeeping (ie. clearing the stream data)
    template<typename Handler>
    class flush_handler
    {
    public:
      flush_handler(request_ostream& os, Handler handler)
        : ostream_(os)
        , handler_(handler)
      {
      }

      void operator()(boost::system::error_code& ec)
      {
        if(!ec) ostream_.clear();
        handler_(ec);
      }
    private:
      request_ostream& ostream_;
      Handler handler_;
    };

    /// Asynchronously flush the data through the supplied request
    /**
     * If there is no error, the buffer is cleared *after* the write has
     * finished.
     */
    template<typename CommonGatewayRequest, typename Handler>
    void async_flush(CommonGatewayRequest& req, Handler handler)
    {
      if (!headers_sent_)
      {
        ostream_<< "Content-type: text/plain\r\n\r\n";
        headers_sent_ = true;
      }
      cgi::async_write(req, rdbuf()->data()
                      , flush_handler<Handler>
                          (*this, handler, boost::arg<1>()));
    }


    /// Synchronously send the reply to the default request
    /**
     * Note: The data in the stream isn't cleared after this call, but the
     * request held in the ostream is removed. ie. send() can't be called
     * twice without an arguement (unless you add another request - something
     * not possible yet).
     */
    //void send()
    //{
    //  BOOST_ASSERT(request_ != NULL);
    //  send(*request_);
    //  request_ = NULL;
    //}

    /// Synchronously send the reply to the default request
    /**
     * Note: The data in the stream isn't cleared after this call. If the send
     * is sucessful, the request held in the ostream is removed. ie. send()
     * can't be called twice without an arguement (unless you add another
     * request - something not possible yet).
     */
    //boost::system::error_code& send(boost::system::error_code& ec)
    //{
    //  BOOST_ASSERT(request_ != NULL);
    //  if(!send(*request_, ec))
    //    request_ = NULL;
    //  return ec;
    //}


    /// Synchronously send the data via the supplied request
    /**
     * This call uses throwing semantics. ie. an exception will be thrown on
     * any failure.
     * Note: The data in the stream isn't cleared after this call.
     */
    template<typename CommonGatewayRequest>
    void send(CommonGatewayRequest& req)
    {
      if (!headers_sent_)
      {
        ostream_<< "Content-type: text/plain\r\n\r\n";
        headers_sent_ = true;
      }
      cgi::write(req.client(), rdbuf()->data());
      req.set_status(http_status_);
    }

    /// Synchronously send the data via the supplied request
    /**
     * This call uses error_code semantics. ie. ec is set if an error occurs.
     * Note: The data in the stream isn't cleared after this call.
     */
    template<typename CommonGatewayRequest>
    boost::system::error_code& send(CommonGatewayRequest& req
                                   , boost::system::error_code& ec)
    {
      if (!headers_sent_)
      {
        ostream_<< "Content-type: text/plain\r\n\r\n";
        headers_sent_ = true;
      }
      cgi::write(req.client(), rdbuf()->data(), ec);
      req.set_status(http_status_);
      return ec;
    }

    /// Asynchronously send the data through the supplied request
    /**
     * Note: The data in the stream isn't cleared after this call.
     */
    template<typename CommonGatewayRequest, typename Handler>
    void async_send(CommonGatewayRequest& req, Handler handler)
    {
      req.set_status(http_status_);
      if (!headers_sent_)
      {
        ostream_<< "Content-type: text/plain\r\n\r\n";
        headers_sent_ = true;
      }
      cgi::async_write(req, rdbuf()->data(), handler);
    }

    /// Get the buffer associated with the stream
    cgi::streambuf* rdbuf()
    {
      return static_cast<cgi::streambuf*>(ostream_.rdbuf());
    }

    void set_status(const http::status_code& num)
    {
      http_status_ = num;
    }

    http::status_code& get_status()
    {
      return http_status_;
    }

  protected:
    std::vector<boost::asio::const_buffer> headers_;
    boost::shared_ptr<cgi::streambuf> buffer_;
    std::ostream ostream_;
    http::status_code http_status_;
    bool headers_sent_;

    template<typename T>
    friend request_ostream& operator<<(request_ostream&, const T&);
  };



  /// Operator<< overload for basic outputting ability
  template<typename T>
  request_ostream& operator<<(request_ostream& os, const T& t)
  {
    os.ostream_<< t;
    return os;
  }

} // namespace cgi

#include "detail/pop_options.hpp"

#endif // CGI_REQUEST_OSTREAM_HPP_INCLUDED__
