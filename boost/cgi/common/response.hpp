//                   -- response.hpp --
//
//            Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_RESPONSE_HPP_INCLUDED__
#define CGI_RESPONSE_HPP_INCLUDED__

#include "boost/cgi/detail/push_options.hpp"

#include <string>
///////////////////////////////////////////////////////////
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/shared_ptr.hpp>
///////////////////////////////////////////////////////////
#include "boost/cgi/common/cookie.hpp"
#include "boost/cgi/common/header.hpp"
#include "boost/cgi/http/status_code.hpp"
#include "boost/cgi/import/streambuf.hpp"
#include "boost/cgi/detail/throw_error.hpp"
#include "boost/cgi/fwd/basic_request_fwd.hpp"

/// This mess outputs a default Content-type header if the user hasn't set any.
/** **FIXME** Not implemented; not sure if it should be...
 * BOOST_CGI_ADD_DEFAULT_HEADER should not persiste beyond this file.
 *
 * It basically works like (default first):
 *
 * Debug mode:
 * - Append a "Content-type: text/plain" header;
 * - If BOOST_CGI_DEFAULT_CONTENT_TYPE is defined, set that as the
 *   content-type;
 * - If BOOST_CGI_NO_DEFAULT_CONTENT_TYPE is defined, do nothing.
 *
 * Release mode:
 * - Do nothing.
 */
#if !defined(NDEBUG) && !defined(BOOST_CGI_NO_DEFAULT_CONTENT_TYPE)
//{
#  if !defined(BOOST_CGI_DEFAULT_CONTENT_TYPE)
#    define BOOST_CGI_DEFAULT_CONTENT_TYPE "Content-type: text/plain"
#  endif // !defined(BOOST_CGI_DEFAULT_CONTENT_TYPE)
//}
#  define BOOST_CGI_ADD_DEFAULT_HEADER   \
      if (headers_.empty())              \
        headers_.push_back(BOOST_CGI_DEFAULT_CONTENT_TYPE"\r\n");
#else
#  define BOOST_CGI_ADD_DEFAULT_HEADER
#endif // !defined(NDEBUG) && !defined(BOOST_CGI_NO_DEFAULT_CONTENT_TYPE)


namespace cgi {
 namespace common {

  /// The response class: a helper for responding to requests.
  template<typename T>
  class basic_response
  {
  public:
    typedef T                              char_type;
    typedef typename std::basic_string<T>  string_type;
    typedef typename std::basic_ostream<T> ostream_type;

    basic_response(common::http::status_code sc = common::http::ok);

    /// Construct with a particular buffer
    /**
     * Takes the buffer and uses it internally, does nothing with it on
     * destruction.
     */
    basic_response(::cgi::common::streambuf* buf,
        common::http::status_code sc = common::http::ok);

    ~basic_response();

    /// Clear the response buffer.
    void clear();

    /// Return the response to the 'just constructed' state.
    void reset();

    // provide this too?
    std::size_t write(const char_type* str, std::size_t len);

    std::size_t write(string_type const& str);

    template<typename ConstBufferSequence>
    std::size_t write(const ConstBufferSequence& buf);

    /// Synchronously flush the data to the supplied SyncWriteStream
    /**
     * This call uses throwing semantics. ie. an exception will be thrown on
     * any failure.
     * If there is no error, the buffer is cleared.
     */
    template<typename SyncWriteStream>
    void flush(SyncWriteStream& sws);

    /// Synchronously flush the data via the supplied request
    /**
     * This call uses error_code semantics. ie. ec is set if an error occurs.
     * If there is no error, the buffer is cleared.
     */
    template<typename SyncWriteStream>
    boost::system::error_code
      flush(SyncWriteStream& sws, boost::system::error_code& ec);

    /// Synchronously send the data via the supplied request.
    /**
     * This call uses throwing semantics. ie. an exception will be thrown on
     * any failure.
     * Note: The data in the stream isn't cleared after this call.
     */
    template<typename SyncWriteStream>
    void send(SyncWriteStream& sws);

    /// Synchronously send the data via the supplied request.
    /**
     * This call will not throw, but will set `ec` such that `ec == true` if
     * an error occurs. Details of the error are held in the `error_code`
     * object.
     */
    template<typename SyncWriteStream>
    boost::system::error_code
      send(SyncWriteStream& sws, boost::system::error_code& ec);

    /// Resend headers + content regardless of value of `headers_terminated_`.
    template<typename SyncWriteStream>
    void resend(SyncWriteStream& sws);

    /// Asynchronously send the data through the supplied request
    /**
     * Note: This is quite crude at the moment and not as asynchronous as
     *       it could/should be. The data in the stream isn't cleared after
     *       this call.
     */
    template<typename AsyncWriteStream, typename Handler>
    void async_send(AsyncWriteStream& aws, Handler handler);

    template<typename AsyncWriteStream, typename Handler>
    void do_async_send(AsyncWriteStream& aws, Handler handler);

    /// Get the buffer associated with the stream
    common::streambuf*
      rdbuf();

    /// Set the status code associated with the response.
    basic_response<char_type>&
      set_status(const http::status_code& num);

    /// Get the status code associated with the response.
    http::status_code& status();

    /// Allow more headers to be added (WARNING: avoid using this).
    void unterminate_headers();

    /// Get the length of the body of the response
    std::size_t content_length();

    /// Add a header after appending the CRLF sequence.
    basic_response<char_type>&
      set_header(const string_type& value);

    /// Format and add a header given name and value, appending CRLF.
    basic_response<char_type>&
      set_header(string_type const& name, string_type const& value);
    
    void clear_headers();

    void reset_headers();

    bool headers_terminated() const;

    // Is this really necessary?
    void end_headers();

    /// Get the ostream containing the response body.
    ostream_type& ostream();
  protected:
    // Vector of all the headers, each followed by a CRLF
    std::vector<string_type> headers_;

    // The buffer is a shared_ptr, so you can keep it cached elsewhere.
    boost::shared_ptr<common::streambuf> buffer_;

    ostream_type ostream_;

    http::status_code http_status_;

    // True if no more headers can be appended. 
    bool headers_terminated_;

  private:
    // Send the response headers and mark that they've been sent.
    template<typename ConstBufferSequence>
    void prepare_headers(ConstBufferSequence& headers);
   };

   /// Typedefs for typical usage.
   typedef basic_response<char>    response;
   typedef basic_response<wchar_t> wresponse; // **FIXME** (untested)

 } // namespace common
} // namespace cgi

  /// Generic ostream template
  template<typename CharT, typename T>
  cgi::common::basic_response<CharT>&
    operator<< (cgi::common::basic_response<CharT>& resp, const T& t);

  template<typename CharT>
  cgi::common::basic_response<CharT>&
    operator<< (cgi::common::basic_response<CharT>& resp
               , cgi::common::basic_header<CharT> const& hdr);

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
  template<typename charT, typename T>
  cgi::common::basic_response<charT>& 
    operator<< (cgi::common::basic_response<charT>&
               , cgi::common::basic_cookie<T>&);

  template<typename charT, typename T>
  cgi::common::basic_response<charT>&
    operator<< (cgi::common::basic_response<charT>&
               , cgi::common::http::status_code);

#include "boost/cgi/detail/pop_options.hpp"

#if !defined(BOOST_CGI_BUILD_LIB)
#  include "boost/cgi/impl/response.ipp"
#endif

#endif // CGI_RESPONSE_HPP_INCLUDED__
