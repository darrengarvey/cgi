//                -- basic_request.hpp --
//
//            Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_BASIC_REQUEST_HPP_INCLUDED__
#define CGI_BASIC_REQUEST_HPP_INCLUDED__

#include "boost/cgi/detail/push_options.hpp"

#include <iostream>
#include <boost/noncopyable.hpp>
#include <boost/mpl/if.hpp>
#include <boost/assert.hpp>
#include <boost/system/error_code.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/basic_io_object.hpp>

#include "boost/cgi/detail/throw_error.hpp"
#include "boost/cgi/detail/protocol_traits.hpp"
#include "boost/cgi/request_base.hpp"
#include "boost/cgi/role_type.hpp"
#include "boost/cgi/data_sink.hpp"
#include "boost/cgi/status_type.hpp"
#include "boost/cgi/is_async.hpp"
#include "boost/cgi/connection_base.hpp"
#include "boost/cgi/http/status_code.hpp"
#include "boost/cgi/request_service.hpp"
#include "boost/cgi/basic_protocol_service_fwd.hpp"
#include "boost/cgi/basic_request_fwd.hpp"
#include "boost/cgi/basic_sync_io_object.hpp"
#include "boost/cgi/basic_io_object.hpp"
#include "boost/cgi/map.hpp"

namespace cgi {

  /// The basic_request class, primary entry point to the library
  /**
   * Note: This class is supposed to make simple use of the library easy.
   * This comes with some restrictions, such as being noncopyable, and also
   * providing copies of the meta-variables rather than references. This makes
   * sure the internal data is protected.
   * The underlying impl_type classes aren't like this (and as such aren't
   * quite as 'safe' to use) but they may be more suited to certain needs.
   *
   * Note: By default, synchronous protocols (ie. cgi) auto-load AND parse STDIN,
   * whereas async protocols don't.
   * 
   * Note: This class isn't thread safe: carrying around a mutex-per-request
   * seems prohibitively expensive. There could be functions which take a mutex
   * as an arguement and lock it. (Async calls could get messy if you need a
   * protected request object)
   *
   * Note: From 10/07/07 this class is going to require a protocol_service be
   * passed to it. This is good for FastCGI/SCGI and asynchronous CGI. A full
   * specialisation of this class for sync CGI can be provided... The reasoning
   * for this is that the added complexity for allowing a non-async CGI request
   * to use the acceptor and this class doesn't seem worth it: it makes the
   * whole library much simpler to do it this way.
   */
  template<typename RequestService
          , typename ProtocolService
          , role_type Role
          , typename Allocator>
  class basic_request
    : public request_base
    , public boost::mpl::if_c<is_async<typename RequestService::protocol_type>::value
                             , basic_io_object<RequestService>
                             , basic_sync_io_object<RequestService>
                             >::type
  {
  public:
    typedef basic_request<RequestService, ProtocolService
                         , Role, Allocator >  type;
    typedef RequestService                               service_type;
    typedef typename service_type::protocol_type         protocol_type;
    typedef ProtocolService                              protocol_service_type;
    typedef boost::shared_ptr<type>                      pointer;
    typedef typename RequestService::implementation_type implementation_type;


    // Throws
    basic_request(bool load_now = true, bool parse_post = true)
      : basic_sync_io_object<service_type>()
    {
      if (load_now) load(parse_post);//this->service.load(this->impl, true, ec);
    }

    basic_request(boost::system::error_code& ec
                 , const bool load_now = true
                 , const bool parse_post = true)
      : basic_sync_io_object<service_type>()
    {
      if (load_now) load(ec, parse_post);//this->service.load(this->impl, true, ec);
    }

    basic_request(protocol_service_type& s, const bool load_now = false
                 , const bool parse_post = false)
      : basic_io_object<service_type>(s.io_service())
    {
      if (load_now) load(parse_post);//this->service.load(this->impl, false, ec);
    }

    basic_request(protocol_service_type& s
                 , boost::system::error_code& ec
                 , const bool load_now = false, const bool parse_post = false)
      : basic_io_object<service_type>(s.io_service())
    {
      if(load_now) load(ec, parse_post);//this->service.load(this->impl, false, ec);
    }

    ~basic_request()
    {
      if (is_open())
        close(http::internal_server_error, 0);
    }

    /// Return `true` if the request is still open (ie. not aborted or closed)
    bool is_open()
    {
      return this->service.is_open(this->impl);
    }

    /// Set a header
    void set_header(const std::string& name, const std::string& val
                   , const std::string& expires)
    {
      this->service.set_header(this->impl, name, val, expires);
    }

    /// Set a cookie
    void set_cookie(const std::string& name, const std::string& val
                   , const std::string& expires = "")
    {
      this->service.set_cookie(this->impl, name, val, expires);
    }

    /// Delete a cookie
    void del_cookie(const std::string& name)
    {
      this->service.del_cookie(this->impl, name);
    }

    /// Delete all cookies
    void del_cookies()
    {
      this->service.del_cookies();
    }

    /// Synchronously read/parse the request meta-data
    /**
     * Note: 'loading' including reading/parsing STDIN if parse_stdin == true
     */
    // Throwing semantics
    void load(bool parse_stdin = false)
    {
      boost::system::error_code ec;
      this->service.load(this->impl, parse_stdin, ec);
      detail::throw_error(ec);
    }

    // Error-code semantics
    boost::system::error_code& load(boost::system::error_code& ec
                                   , bool parse_stdin = false)
    {
      return this->service.load(this->impl, parse_stdin, ec);
    }


    /// Asynchronously read/parse the request meta-data
    /**
     * Note: 'loading' including reading/parsing STDIN if parse_stdin == true
     */
    template<typename Handler>
    void async_load(Handler handler, bool parse_stdin = false)
    {
      this->service.async_load(this->impl, parse_stdin, handler);
    }

    /// Notify the server the request has finished being handled
    /**
     * In certain situations (such as a Proactor client using the async read
     * functions) it will be necessary to call end, rather than just returning
     * from the sub_main function.
     *
     * @param program_status This value is returned to the server indicating the
     * state of the request after it was finished handling. It is
     * implementation defined how the server deals with this, and it may have
     * no effect on the http status code returned to the client (eg. 200 OK).
     *
     * @returns The value of program_status
     */
    int close(http::status_code http_status, int program_status)
    {
      //BOOST_ASSERT( request_status_ != status_type::ended );

      //this->service.set_status(this->impl, http_status);
      return this->service.close(this->impl, http_status, program_status);
    }

    /// Reject the request with a standard '500 Internal Server Error' error
    int reject()
    {
      this->service.set_status(this->impl, aborted);
      return this->service.close(this->impl, http::internal_server_error);
    }

    /// Abort a request
    void abort()
    {
      this->service.set_status(this->impl, aborted);
    }

    /// Set the output for the request
    /**
     * Not Implemented Yet ******************
     *
     * Set the output sink as `stdout_`, `stderr_`, or `stdout_ | stderr_`
     */
    void set_output(cgi::sink dest = stdout_)
    {
      boost::system::error_code ec;
      this->service(this->impl, dest, ec);
      detail::throw_error(ec);
    }

    /// Set the output for the request
    /**
     * Not Implemented Yet ******************
     *
     * Set the output sink as `stdout_`, `stderr_`, or `stdout_ | stderr_`
     */
    void set_output(cgi::sink dest, boost::system::error_code& ec)
    {
      this->service(this->impl, dest, ec);
    }

    /// Read some data from the client
    template<typename MutableBufferSequence/*, typename Source*/>
    std::size_t read_some(const MutableBufferSequence& buf)
    {
      boost::system::error_code ec;
      std::size_t s = this->service.read_some(this->impl, buf, ec);
      detail::throw_error(ec);
      return s;
    }

    /// Read some data from the client
    template<typename MutableBufferSequence/*, typename Source*/>
    std::size_t read_some(const MutableBufferSequence& buf
                         , boost::system::error_code& ec)
    {
      return this->service.read_some(this->impl, buf, ec);
    }

    /// Write some data to the client
    /**
     * Note: on the first write, the header block is closed (with a blank
     * line).
     */
    template<typename ConstBufferSequence/*, typename Sink*/>
    std::size_t write_some(const ConstBufferSequence& buf)
    {
      boost::system::error_code ec;
      std::size_t s = this->service.write_some(this->impl, buf, ec);
      detail::throw_error(ec);
      return s;
    }

    /// Write some data to the client
    /**
     * Note: on the first write, the header block is closed (with a blank
     * line).
     */
    template<typename ConstBufferSequence/*, typename Sink*/>
    std::size_t write_some(const ConstBufferSequence& buf
                          , boost::system::error_code& ec)
    {
      return this->service.write_some(this->impl, buf, ec);
    }

    /// Get a `cgi::map&` corresponding to all of the GET variables
    cgi::map& get_()
    {
      return this->service.meta_get(this->impl);
    }

    /// Find the get meta-variable matching name
    /**
     * @throws `boost::system::system_error` if an error occurred. This may
     * fail with `cgi::error::request_aborted` if the request has been aborted
     * by the client.
     */
    std::string get_(const std::string& name)
    {
      boost::system::error_code ec;
      std::string ret = this->service.meta_get(this->impl, name, ec);
      detail::throw_error(ec);
      return ret;
    }

    /// Find the get meta-variable matching name
    /**
     * @param ec Set such that `(!ec == false)` if an error occurred. This may
     * fail with `ec == cgi::error::request_aborted` if the request has been
     * aborted by the client.
     */
    std::string get_(const std::string& name, boost::system::error_code& ec)
    {
      return this->service.meta_get(this->impl, name, ec);
    }

    /// Get a `cgi::map&` corresponding to all of the POST variables
    cgi::map& post_()
    {
      return this->service.meta_post(this->impl);
    }

    /// Find the post meta-variable matching name
    /**
     * @param greedy This determines whether more data can be read to find
     * the variable. The default is true to cause least surprise in the common
     * case of having not parsed any of the POST data.
     *
     * @throws `boost::system::system_error` if an error occurred. This may
     * fail with `cgi::error::request_aborted` if the request has been aborted
     * by the client.
     */
    std::string post_(const std::string& name, bool greedy = true)
    {
      boost::system::error_code ec;
      std::string ret = this->service.meta_post(this->impl, name, ec, greedy);
      detail::throw_error(ec);
      return ret;
    }

    /**
     * @param ec Set such that `(!ec == false)` if an error occurred. This may
     * fail with `ec == cgi::error::request_aborted` if the request has been
     * aborted by the client.
     */
    std::string post_(const std::string& name, boost::system::error_code& ec
                          , bool greedy = true)
    {
      return this->service.meta_post(this->impl, name, ec, greedy);
    }

    /// Get a `cgi::map&` corresponding to all of the form variables
    cgi::map& form_()
    {
      return this->service.meta_form(this->impl);
    }

    /// Find the form variable matching name
    /**
     * Depending on the request's request_method, either the GET or the POST
     * meta-variables are searched.
     *
     * @throws `boost::system::system_error` if an error occurred. This may
     * fail with `cgi::error::request_aborted` if the request has been aborted
     * by the client.
     */
    std::string form_(const std::string& name, bool greedy = true)
    {
      boost::system::error_code ec;
      std::string ret = form_(name, ec, greedy);
      detail::throw_error(ec);
      return ret;
    }

    /**
     * @param ec Set such that `(!ec == false)` if an error occurred. This may
     * fail with `ec == cgi::error::request_aborted` if the request has been
     * aborted by the client.
     */
    std::string form_(const std::string& name, boost::system::error_code& ec
                         , bool greedy = true)
    {
      std::string rm(request_method());
      if (rm == "GET")
        return this->service.meta_get(this->impl, name, ec);
      else
      if (rm == "POST")
        return this->service.meta_post(this->impl, name, ec, greedy);
      else
        return "";
    }

    /// Get a `cgi::map&` corresponding to all of the HTTP_COOKIE variables
    cgi::map& cookie_()
    {
      return this->service.meta_cookie(this->impl);
    }

    /// Find the cookie meta-variable matching name
    /**
     * @throws `boost::system::system_error` if an error occurred. This may
     * fail with `cgi::error::request_aborted` if the request has been aborted
     * by the client.
     */
    std::string cookie_(const std::string& name)
    {
      boost::system::error_code ec;
      std::string ret = this->service.cookie(this->impl, name, ec);
      detail::throw_error(ec);
      return ret;
    }

    /// Find the cookie meta-variable matching name
    /**
     * @param ec Set such that `(!ec == false)` if an error occurred. This may
     * fail with `ec == cgi::error::request_aborted` if the request has been
     * aborted by the client.
     */
    std::string cookie_(const std::string& name, boost::system::error_code& ec)
    {
      return this->service.cookie(this->impl, name, ec);
    }

    /// Get a `cgi::map&` corresponding to all of the environment variables
    cgi::map& env_()
    {
      return this->service.meta_env(this->impl);
    }

    /// Find the environment meta-variable matching name
    /**
     * @throws `boost::system::system_error` if an error occurred. This may
     * fail with `cgi::error::request_aborted` if the request has been aborted
     * by the client.
     */
    std::string env_(const std::string& name)
    {
      boost::system::error_code ec;
      std::string ret = this->service.meta_env(this->impl, name, ec);
      detail::throw_error(ec);
      return ret;
    }

    /// Find the environment meta-variable matching name
    /**
     * @param ec Set such that `(!ec == false)` if an error occurred. This may
     * fail with `ec == cgi::error::request_aborted` if the request has been
     * aborted by the client.
     */
    std::string env_(const std::string& name, boost::system::error_code& ec)
    {
      return this->service.meta_env(this->impl, name, ec);
    }

    /// Search through all meta vars for the meta-variable matching name
    /**
     * The policy w.r.t. POST data (ie. whether it should all
     * be read/parsed and included in this search or not) is 
     * to be decided.
     *
     * Notes:
     * One option is to parse everything, making this option
     * very inefficient.
     * Another is to leave this function as a 'lazy' search:
     * it'll search with what it's got and no more. Then, also
     * provide a meta_var_all() function which is greedy; the
     * ugly/long name there to discourage use.
     */
    std::string var_(const std::string& name, bool greedy = false)
    {
      boost::system::error_code ec;
      std::string ret = var_(name, ec, greedy);
      return this->service.meta_var(this->impl, name, greedy);
      std::string request_method( env_("REQUEST_METHOD") );

      std::string tmp;

      // If it's not a POST request search meta_get first (to save time)
      if (request_method.empty() || request_method == "GET")
      {
        tmp = get_(name);
        if (!tmp.empty())
          return tmp;
      }

      tmp = cookie_(name);
      if (!tmp.empty())
	      return tmp;

      tmp = env_(name);
      if (!tmp.empty())
	      return tmp;

      if (!request_method.empty() && request_method == "POST")
      {
        tmp = post_(name);
        if (!tmp.empty())
          return tmp;
      }

      tmp = get_(name);
      return tmp.empty() ? "" : tmp;
    }

    // Some helper functions for the basic CGI 1.1 meta-variables
    std::string auth_type()
    { return env_("AUTH_TYPE"); }

    std::string content_length()
    { return env_("CONTENT_LENGTH"); }

    std::string content_type()
    { return env_("CONTENT_TYPE"); }

    std::string gateway_interface()
    { return env_("GATEWAY_INTERFACE"); }

    std::string path_info()
    { return env_("PATH_INFO"); }

    std::string path_translated()
    { return env_("PATH_TRANSLATED"); }

    std::string query_string()
    { return env_("QUERY_STRING"); }

    std::string remote_addr()
    { return env_("REMOTE_ADDR"); }

    std::string remote_host()
    { return env_("REMOTE_HOST"); }

    std::string remote_ident()
    { return env_("REMOTE_IDENT"); }

    std::string remote_user()
    { return env_("REMOTE_USER"); }

    std::string request_method()
    { return env_("REQUEST_METHOD"); }

    std::string script_name()
    { return env_("SCRIPT_NAME"); }

    std::string server_name()
    { return env_("SERVER_NAME"); }

    std::string server_port()
    { return env_("SERVER_PORT"); }

    std::string server_protocol()
    { return env_("SERVER_PROTOCOL"); }

    std::string server_software()
    { return env_("SERVER_SOFTWARE"); }


    /// The role that the request is playing
    /**
     * The default role type is responder.
     *
     * In some cases - for instance with FastCGI - the role type can be
     * different
     * eg. authorizer, or filter.
     */
    role_type& role()
    {
      return this->service.get_role(this->impl);
    }

    /// Get the strand associated with the request (if any)
    // Not sure if the strand concept should be kept separate or a member
    // function like basic_request<>::wrap() should be provided: in the case of
    // a synchronous request type the wrapping would still function as expected
    // and there would be no need for protocol-specific code in user programs.
      /*    boost::asio::strand* strand()
    {
      return this->impl.strand();
    }
      */

    /// Get the implementation type for the request
    //impl_type& impl() const
    //{
    //  return this->impl;
    //}

    void set_status(http::status_code status)
    {
      this->service.set_status(this->impl, status);
    }

    /// Get the client connection associated with the request
    typename implementation_type::connection_type&
    client()
    {
      return this->service.client(this->impl);
    }

    /// Set a user cookie
    /**
     * Note: this must be called before you have finished outputting
     * the reply headers or it will just appear as normal data
     *
     * ** Is this actually necessary? **
     */
    void set_cookie(const std::string& name, const std::string& value)
    {
      return this->service.set_cookie(this->impl, name, value);
    }
  };

} // namespace cgi

#include "boost/cgi/detail/pop_options.hpp"

#endif // CGI_BASIC_REQUEST_HPP_INCLUDED__

/*
NOTES::future_plans:
* When a request is aborted (eg. when the client closes the connection)
  the library can call an abort_handler() function stored in the request.
  - The user should supply an abort_handler-derived function if they want
    any special handling of aborted requests
*/
