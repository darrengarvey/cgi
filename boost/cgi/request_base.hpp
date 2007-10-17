#ifndef CGI_REQUEST_BASE_HPP_INCLUDE_
#define CGI_REQUEST_BASE_HPP_INCLUDE_

//#include "role_type.hpp"

namespace cgi {

  /// ABC that defines the basic interface for cgi::basic_cgi_request<>s
  /**
   * This class also allows for the general (rather than generic) verision
   * of cgi::request, which can take any type of request.
   */
  class request_base
  {
  public:
    /// Notify the server that the request has been handled
    /**
     * Under normal circumstances {{ideally}}, this won't be called internally
     * but a user may safely call it anyway, if (for example) they have a lot
     * of post-request-handling housekeeping to do but want the server to
     * finish with that request.
     *
     * @param return_code This is equivalent to the return value from main() in
     * a standard cgi library, or sub_main() in the provided examples. 0 should
     * denote success, anything else an error, however the number will
     * generally be ignored by the server.
     */
    //virtual void end( int return_code ) = 0;

    /// Returns the number of bytes available in the read buffer
    //virtual std::size_t available() = 0;

    /// Get the streambuf associated with the request
    /**
     * You may want to construct your own i/ostream instead of using the
     * request directly.
     *
     * @note You should NOT use std::cout/std::cin for writes, as they are
     * not thread aware and in some cases won't exist (eg. in a strict fastcgi
     * impletentation).
     *
     * {{should this return an asio::streambuf instead?}}
     */
    //virtual std::streambuf* rdbuf() = 0;

    /// Flush the output buffer now
    /**
     * Since the output for a request is buffered until the request has
     * completed (see Design notes), under low-memory or large-output
     * conditions, incrementally flushing the buffer may be necessary
     *
     * @note If something goes awry after calling this, the user will
     * be left with half a reply, rather than a (cleaner) error page.
     */
    //virtual void flush() = 0;

    /// Write a message to the error output
    /**
     * In a standard cgi program, this is equivalent to writing to std::cerr,
     * which should be closed in fastcgi (although many implementations leave
     * it writable). In order to write an error message in a cross-platform
     * way, you should use this function.
     *
     * @note On Apache and lighttpd the messages are appended to the server
     * log file, although there is no guarantee that other platforms will
     * recognise it.
     */
    //virtual void log( const std::string& error_message ) = 0;

    /// Get the role of the current request
    /**
     * The FastCGI 1.0 protocol specifies three types of request:
     *
     * @li Responder: the familiar type
     *
     * @li Authorizer: the server provides client information and the
     * application responds with a true/false response, plus optional data
     * about where to redirect the client and with what extra variables
     *
     * @li Filter: the program is provided with data and a file, which it is
     * expected to 'filter' and then return to the server. This is poorly
     * supported in production servers {{AFAIK}}
     */
    //virtual role_type role() = 0;

    /* SyncReadStream function */
    //template<typename MutableBufferSequence>
    //virtual std::size_t read_some(MutableBufferSequence) = 0;

    //template<typename MutableBufferSequence>
    //virtual std::size_t read_some(MutableBufferSequence, error_code&) = 0;

  protected:
    /// Prevent deletion through this type
    ~request_base()
    {
    }
  };

} // namespace cgi

#endif // CGI_REQUEST_BASE_HPP_INCLUDE_

