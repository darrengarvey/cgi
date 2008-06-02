//                  -- impl/response.ipp --
//
//            Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
//
// The implementation for common::basic_response<>.
//
#ifndef CGI_RESPONSE_IPP_INCLUDED__
#define CGI_RESPONSE_IPP_INCLUDED__

#include "boost/cgi/detail/push_options.hpp"

#include <string>
///////////////////////////////////////////////////////////
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
///////////////////////////////////////////////////////////
#include "boost/cgi/import/write.hpp"
#include "boost/cgi/import/buffer.hpp"
#include "boost/cgi/common/cookie.hpp"
#include "boost/cgi/common/header.hpp"
#include "boost/cgi/common/response.hpp"
#include "boost/cgi/http/status_code.hpp"
#include "boost/cgi/import/streambuf.hpp"
#include "boost/cgi/detail/throw_error.hpp"
#include "boost/cgi/fwd/basic_request_fwd.hpp"


namespace cgi {
 namespace common {

  template<typename T> BOOST_CGI_INLINE
  basic_response<T>::basic_response(http::status_code sc)
    : buffer_(new common::streambuf())
    , ostream_(buffer_.get())
    , http_status_(sc)
    , headers_terminated_(false)
  {
  }

  /// Construct with a particular buffer
  /**
   * Takes the buffer and uses it internally, does nothing with it on
   * destruction.
   */
  template<typename T> BOOST_CGI_INLINE
  basic_response<T>::basic_response(common::streambuf* buf,
      http::status_code sc)
    : ostream_(buf)
    , http_status_(sc)
  {
  }

  template<typename T> BOOST_CGI_INLINE
  basic_response<T>::~basic_response()
  {
  } 

    /// Clear the response buffer.
  template<typename T> BOOST_CGI_INLINE
  void basic_response<T>::clear()
  {
    ostream_.clear();
    headers_.clear();
    headers_terminated_ = false;
  }

    /// Return the response to the 'just constructed' state.
  template<typename T> BOOST_CGI_INLINE
  void basic_response<T>::reset()
  {
    clear();
    headers_terminated_ = false;
  }

    // provide this too?
  template<typename T> BOOST_CGI_INLINE
  std::size_t basic_response<T>::write(const char_type* str, std::size_t len)
  {
    ostream_.write(str, len);
    return len;
  }

  template<typename T> BOOST_CGI_INLINE
  std::size_t basic_response<T>::write(std::basic_string<T> const& str)
  {
    return write(str.c_str(), str.size());
  }

  template<typename T>
  template<typename ConstBufferSequence> BOOST_CGI_INLINE
  std::size_t basic_response<T>::write(const ConstBufferSequence& buf)
  {
    return ostream_.write(buf.begin(), buf.end());
  }

  /// Synchronously flush the data to the supplied SyncWriteStream
  /**
   * This call uses throwing semantics. ie. an exception will be thrown on
   * any failure.
   * If there is no error, the buffer is cleared.
   */
  template<typename T>
  template<typename SyncWriteStream> BOOST_CGI_INLINE
  void basic_response<T>::flush(SyncWriteStream& sws)
  {
    boost::system::error_code ec;
    flush(sws, ec);
    detail::throw_error(ec);
  }
    
  /// Synchronously flush the data via the supplied request
  /**
   * This call uses error_code semantics. ie. ec is set if an error occurs.
   * If there is no error, the buffer is cleared.
   */
  template<typename T>
  template<typename SyncWriteStream> BOOST_CGI_INLINE
  boost::system::error_code
  basic_response<T>::flush(SyncWriteStream& sws, boost::system::error_code& ec)
  {
    if (!headers_terminated_)
    {
      std::vector<boost::asio::const_buffer> headers;
      prepare_headers(headers);//, ec);
      common::write(sws, headers, boost::asio::transfer_all(), ec);
      if (ec)
        return ec;
    }

    std::size_t bytes_written
        = common::write(sws, buffer_->data(), boost::asio::transfer_all(), ec);
    if (!ec)
      buffer_->consume(bytes_written);

    return ec;
  }

  /// Synchronously send the data via the supplied request.
  /**
   * This call uses throwing semantics. ie. an exception will be thrown on
   * any failure.
   * Note: The data in the stream isn't cleared after this call.
   */
  template<typename T>
  template<typename SyncWriteStream> BOOST_CGI_INLINE
  void basic_response<T>::send(SyncWriteStream& sws)
  {
    boost::system::error_code ec;
    send(sws, ec);
    detail::throw_error(ec);
  }

  /// Synchronously send the data via the supplied request.
  /**
   * This call will not throw, but will set `ec` such that `ec == true` if
   * an error occurs. Details of the error are held in the `error_code`
   * object.
   */
  template<typename T>
  template<typename SyncWriteStream> BOOST_CGI_INLINE
  boost::system::error_code
    basic_response<T>::send(SyncWriteStream& sws
                           , boost::system::error_code& ec)
  {
    if (!headers_terminated_)
    {
      /* Not sure if streambuf allows this
       *
      // We want to be able to keep adding to a response, calling send() on
      // it whenever, without resending the headers. Call resend() if you
      // want to send the whole response again.
      headers_terminated_ = true;
       */
      std::vector<boost::asio::const_buffer> headers;
      prepare_headers(headers);//, ec)
      common::write(sws, headers, boost::asio::transfer_all(), ec);
    }

    common::write(sws, buffer_->data(), boost::asio::transfer_all(), ec);

    return ec;
  }

    /// Resend headers + content regardless of value of `headers_terminated_`.
  template<typename T>
  template<typename SyncWriteStream> BOOST_CGI_INLINE
  void basic_response<T>::resend(SyncWriteStream& sws)
  {
    std::vector<boost::asio::const_buffer> headers;
    prepare_headers(headers);//, ec)
    common::write(sws, headers);

    common::write(sws, buffer_->data());
  }

    /// Asynchronously send the data through the supplied request
    /**
     * Note: This is quite crude at the moment and not as asynchronous as
     *       it could/should be. The data in the stream isn't cleared after
     *       this call.
     */
  template<typename T>
  template<typename AsyncWriteStream, typename Handler> BOOST_CGI_INLINE
  void basic_response<T>::async_send(AsyncWriteStream& aws, Handler handler)
  {
    aws.io_service().post(
      boost::bind(&basic_response<char_type>::do_async_send, aws, handler)
    );
  }

  template<typename T>
  template<typename AsyncWriteStream, typename Handler> BOOST_CGI_INLINE
  void basic_response<T>::do_async_send(AsyncWriteStream& aws, Handler handler)
  {
    //req.set_status(http_status_);
    /*
    if (!headers_terminated_)
    {
      ostream_<< "Content-type: text/plain\r\n\r\n";
      headers_terminated_ = true;
    }
    */
    common::async_write(aws, rdbuf()->data(), handler);
  }

  /// Get the buffer associated with the stream
  template<typename T> BOOST_CGI_INLINE
  common::streambuf*
  basic_response<T>::rdbuf()
  {
    return static_cast<common::streambuf*>(ostream_.rdbuf());
  }

  /// Set the status code associated with the response.
  template<typename T> BOOST_CGI_INLINE
  basic_response<T>&
  basic_response<T>::set_status(const http::status_code& num)
  {
    http_status_ = num;
    return *this;
  }

  /// Get the status code associated with the response.
  template<typename T> BOOST_CGI_INLINE
  http::status_code& 
  basic_response<T>::status()
  {
    return http_status_;
  }

  /// Allow more headers to be added (WARNING: avoid using this).
  template<typename T> BOOST_CGI_INLINE
  void basic_response<T>::unterminate_headers()
  {
    headers_terminated_ = false;
  }

  /// Get the length of the body of the response
  template<typename T> BOOST_CGI_INLINE
  std::size_t basic_response<T>::content_length()
  {
    return rdbuf()->size();
  }

  /// Add a header after appending the CRLF sequence.
  template<typename T> BOOST_CGI_INLINE
  basic_response<T>&
  basic_response<T>::set_header(
        typename basic_response<T>::string_type const& value)
  {
    BOOST_ASSERT(!headers_terminated_);
    headers_.push_back(value + "\r\n");
    return *this;
  }

  /// Format and add a header given name and value, appending CRLF.
  template<typename T> BOOST_CGI_INLINE
  basic_response<T>&
  basic_response<T>::set_header(
      typename basic_response<T>::string_type const& name
    , typename basic_response<T>::string_type const& value)
  {
    BOOST_ASSERT(!headers_terminated_);
    headers_.push_back(name + ": " + value + "\r\n");
    return *this;
  }

  template<typename T> BOOST_CGI_INLINE
  void basic_response<T>::clear_headers()
    {
      BOOST_ASSERT(!headers_terminated_);
      headers_.clear();
    }

  template<typename T> BOOST_CGI_INLINE
  void basic_response<T>::reset_headers()
    {
      headers_.clear();
      headers_terminated_ = false;
    }

  template<typename T> BOOST_CGI_INLINE
  bool basic_response<T>::headers_terminated() const
    {
      return headers_terminated_;
    }

    // Is this really necessary?
  template<typename T> BOOST_CGI_INLINE
  void basic_response<T>::end_headers()
  {
    headers_terminated_ = true;
  }

    /// Get the ostream containing the response body.
  template<typename T> BOOST_CGI_INLINE
  typename basic_response<T>::ostream_type&
    basic_response<T>::ostream() { return ostream_; }
    
  // Send the response headers and mark that they've been sent.
  template<typename T>
  template<typename ConstBufferSequence> BOOST_CGI_INLINE
  void basic_response<T>::prepare_headers(ConstBufferSequence& headers)
  {
    BOOST_CGI_ADD_DEFAULT_HEADER

    // Terminate the headers.
    if (!headers_terminated_)
      headers_.push_back("\r\n");

    //{ Construct a ConstBufferSequence out of the headers we have.
    //std::vector<boost::asio::const_buffer> headers;
    typedef typename std::vector<string_type>::iterator iter;
    for (iter i(headers_.begin()), e(headers_.end()); i != e; ++i)
    {
      headers.push_back(common::buffer(*i));
    }
    //}

    headers_terminated_ = true;
    //return ec;
  }

 } // namespace common
} // namespace cgi

  /// Generic ostream template
  template<typename T, typename U> BOOST_CGI_INLINE
  cgi::common::basic_response<T>&
    operator<< (cgi::common::basic_response<T>& resp, const U& u)
  {
    resp.ostream()<< u;
    return resp;
  }

  /// You can stream a cgi::header into a response.
  /**
   * This is just a more convenient way of doing:
   *
   * ``
   * resp.set_header(header_content)
   * ``
   *
   * [tip
   * If you stream a default-constructed header to a response, it
   * 'terminates' the headers. ie. You can do this if you want to ensure
   * no further headers are added to the response. It has no other side-
   * effects; for instance, it won't write any data to the client.
   * ]
   */
  template<typename T> BOOST_CGI_INLINE
  cgi::common::basic_response<T>&
    operator<< (cgi::common::basic_response<T>& resp
               , cgi::common::basic_header<T> const& hdr)
  {
    if (hdr.content.empty()) {
      resp.end_headers();
      return resp;
    }else{
      // We shouldn't allow headers to be sent after they're explicitly ended.
      BOOST_ASSERT(!resp.headers_terminated());
      resp.set_header(hdr.content);
      return resp;
    }
  }

  /// You can stream a cgi::cookie into a response.
  /**
   * This is just a shorthand way of setting a header that will set a
   * client-side cookie.
   *
   * You cannot stream a cookie to a response after the headers have been
   * terminated. In this case, an alternative could be to use the HTML tag:
   * <meta http-equiv="Set-cookie" ...> (see http://tinyurl.com/3bxftv or
   * http://tinyurl.com/33znkj), but this is outside the scope of this
   * library.
   */
  template<typename T> BOOST_CGI_INLINE
  cgi::common::basic_response<T>& 
    operator<< (cgi::common::basic_response<T> const& resp
               , cgi::common::basic_cookie<T> ck)
  {
    BOOST_ASSERT(!resp.headers_terminated());
    resp.set_header("Set-cookie", ck.to_string());
    return resp;
  }

  template<typename T> BOOST_CGI_INLINE
  cgi::common::basic_response<T>& 
    operator<< (cgi::common::basic_response<T>& resp
               , cgi::common::basic_cookie<T> const& ck)
  {
    BOOST_ASSERT(!resp.headers_terminated());
    resp.set_header("Set-cookie", ck.to_string());
    return resp;
  }

  template<typename T> BOOST_CGI_INLINE
  cgi::common::basic_response<T>&
    operator<< (cgi::common::basic_response<T>& resp
               , cgi::common::http::status_code status)
  {
    BOOST_ASSERT(!resp.headers_terminated());
    return resp.set_status(status);
  }

#undef BOOST_CGI_ADD_DEFAULT_HEADER

#include "boost/cgi/detail/pop_options.hpp"

#endif // CGI_RESPONSE_HPP_INCLUDED__
