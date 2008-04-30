//                -- basic_request.hpp --
//
//            Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
//
// Defines the basic_request<> class; the main entry-point to the
// library.
//
////////////////////////////////////////////////////////////////
#ifndef CGI_BASIC_REQUEST_HPP_INCLUDED__
#define CGI_BASIC_REQUEST_HPP_INCLUDED__

#include "boost/cgi/detail/push_options.hpp"

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
#include "boost/cgi/common/source_enums.hpp"
#include "boost/cgi/status_type.hpp"
#include "boost/cgi/is_async.hpp"
#include "boost/cgi/connection_base.hpp"
#include "boost/cgi/http/status_code.hpp"
#include "boost/cgi/request_service.hpp"
#include "boost/cgi/basic_protocol_service_fwd.hpp"
#include "boost/cgi/basic_request_fwd.hpp"
#include "boost/cgi/basic_sync_io_object.hpp"
#include "boost/cgi/basic_io_object.hpp"
#include "boost/cgi/common/map.hpp"

namespace cgi {
 namespace common {

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
    : //public request_base
     public boost::mpl::if_c<is_async<typename RequestService::protocol_type>::value
                             , basic_io_object<RequestService>
                             , basic_sync_io_object<RequestService>
                             >::type
  {
  public:
    typedef basic_request<RequestService, ProtocolService
                         , Role, Allocator >             type;
    typedef ::cgi::common::map                                   map_type;
    typedef RequestService                               service_type;
    typedef typename service_type::protocol_type         protocol_type;
    typedef ProtocolService                              protocol_service_type;
    typedef boost::shared_ptr<type>                      pointer;
    typedef typename RequestService::implementation_type implementation_type;
    typedef typename implementation_type::client_type    client_type;


    // Throws
    basic_request(bool load_now = true, bool parse_post = true)
      : basic_sync_io_object<service_type>()
    {
      if (load_now) load(parse_post);//this->service.load(this->implementation, true, ec);
    }

		// Won't throw
    basic_request(boost::system::error_code& ec
                 , const bool load_now = true
                 , const bool parse_post = true)
      : basic_sync_io_object<service_type>()
    {
      if (load_now) load(ec, parse_post);//this->service.load(this->implementation, true, ec);
    }

		// Throws
    basic_request(protocol_service_type& s, const bool load_now = false
                 , const bool parse_post = false)
      : basic_io_object<service_type>(s.io_service())
    {
      set_protocol_service(s);
      if (load_now) load(parse_post);//this->service.load(this->implementation, false, ec);
    }

		// Won't throw
    basic_request(protocol_service_type& s
                 , boost::system::error_code& ec
                 , const bool load_now = false, const bool parse_post = false)
      : basic_io_object<service_type>(s.io_service())
    {
      set_protocol_service(s);
      if(load_now) load(ec, parse_post);//this->service.load(this->implementation, false, ec);
    }

    /// Make a new mutiplexed request from an existing connection.
    basic_request(implementation_type& impl)
      : basic_io_object<service_type>(impl.service_->io_service())
    {
      set_protocol_service(*impl.service_);
      boost::system::error_code ec;
      this->service.begin_request_helper(this->implementation
                                        , impl.header_buf_, ec);
      detail::throw_error(ec);
    }

    /// Make a new mutiplexed request from an existing connection.
    basic_request(implementation_type& impl, boost::system::error_code& ec)
      : basic_io_object<service_type>(impl.service_->io_service())
    {
      set_protocol_service(*impl.service_);
      this->service.begin_request_helper(this->implementation
                                        , impl.header_buf_, ec);
    }
      
    ~basic_request()
    {
      //if (is_open())
      //  close(http::internal_server_error, 0);
    }

    static pointer create(protocol_service_type& ps)
    {
      return pointer(new type(ps));
    }

    void set_protocol_service(protocol_service_type& ps)
    {
      this->service.set_service(this->implementation, ps);
    }

    /// Return `true` if the request is still open (ie. not aborted or closed)
    bool is_open()
    {
      return this->service.is_open(this->implementation);
    }

    /// Synchronously read/parse the request meta-data
    /**
     * Note: 'loading' including reading/parsing STDIN if parse_stdin == true
     */
    // Throwing semantics
    void load(bool parse_stdin = false)
    {
      boost::system::error_code ec;
      this->service.load(this->implementation, parse_stdin, ec);
      detail::throw_error(ec);
    }

    // Error-code semantics
    boost::system::error_code&
      load(boost::system::error_code& ec, bool parse_stdin = false)
    {
      return this->service.load(this->implementation, parse_stdin, ec);
    }


    // **FIXME**
    /// Asynchronously read/parse the request meta-data
    /**
     * Note: 'loading' including reading/parsing STDIN if parse_stdin == true
     */
    //template<typename Handler>
    //void async_load(Handler handler, bool parse_stdin = false)
    //{
    //  this->service.async_load(this->implementation, parse_stdin
    //                          , handler);
    //}

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
    int close(http::status_code http_status = http::ok
             , int program_status = 0)
    {
      //BOOST_ASSERT( request_status_ != status_type::ended );

      //this->service.set_status(this->implementation, http_status);
      return this->service.close(this->implementation, http_status
                                , program_status);
    }

    int close(http::status_code http_status
             , int program_status
             , boost::system::error_code& ec)
    {
      return this->service.close(this->implementation, http_status
                                , program_status, ec);
    }

    /// Reject the request with a standard '500 Internal Server Error' error
    int reject()
    {
      this->service.set_status(this->implementation, aborted);
      return this->service.close(this->implementation
                                , http::internal_server_error);
    }

    /// Abort a request
    void abort()
    {
      this->service.set_status(this->implementation, aborted);
    }

    /// Clear the data for the request, for reusing this object.
    // I'd imagine clearing and re-loading a request is quicker than 
    // destroying/re-creating one. **Unverified claims** **FIXME**
    void clear()
    {
      this->service.clear(this->implementation);
    }

    /// Get the client connection associated with the request
    /**
     * You use the client for read/write calls. Y
		 */
    client_type& client()
    {
      return this->service.client(this->implementation);
    }

    template<typename MutableBufferSequence>
    void read_some(const MutableBufferSequence& buf)
    {
      boost::system::error_code ec;
      this->service.read_some(this->implementation, buf, ec);
      detail::throw_error(ec);
    }

    template<typename MutableBufferSequence>
    boost::system::error_code
      read_some(const MutableBufferSequence& buf
               , boost::system::error_code& ec)
    {
      return this->service.read_some(this->implementation, buf, ec);
    }

    /// Set the output for the request
    /**
     * Not Implemented Yet ******************
     *
     * Set the output sink as `stdout_`, `stderr_`, or `stdout_ | stderr_`
     */
    /*
    void set_output(cgi::sink dest, boost::system::error_code& ec)
    {
      this->service(this->implementation, dest, ec);
    }
    */

    /// Find the get meta-variable matching name
    /**
     * @throws `boost::system::system_error` if an error occurred. This may
     * fail with `cgi::error::request_aborted` if the request has been aborted
     * by the client.
     */
    std::string GET(const std::string& name)
    {
      boost::system::error_code ec;
      std::string ret = this->service.GET(this->implementation, name, ec);
      detail::throw_error(ec);
      return ret;
    }

    /// Find the get meta-variable matching name
    /**
     * @param ec Set such that `(!ec == false)` if an error occurred. This may
     * fail with `ec == cgi::error::request_aborted` if the request has been
     * aborted by the client.
     */
    std::string GET(const std::string& name, boost::system::error_code& ec)
    {
      return this->service.GET(this->implementation, name, ec);
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
    std::string POST(const std::string& name, bool greedy = true)
    {
      boost::system::error_code ec;
      std::string ret
        = this->service.POST(this->implementation, name, ec, greedy);
      detail::throw_error(ec);
      return ret;
    }

    /**
     * @param ec Set such that `(!ec == false)` if an error occurred. This may
     * fail with `ec == cgi::error::request_aborted` if the request has been
     * aborted by the client.
     */
    std::string POST(const std::string& name, boost::system::error_code& ec
                          , bool greedy = true)
    {
      return this->service.POST(this->implementation, name, ec, greedy);
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
    std::string form(const std::string& name, bool greedy = true)
    {
      boost::system::error_code ec;
      std::string ret = form(name, ec, greedy);
      detail::throw_error(ec);
      return ret;
    }

    /**
     * @param ec Set such that `(!ec == false)` if an error occurred. This may
     * fail with `ec == cgi::error::request_aborted` if the request has been
     * aborted by the client.
     */
    std::string form(const std::string& name, boost::system::error_code& ec
                         , bool greedy = true)
    {
      std::string rm(request_method());
      if (rm == "GET")
        return this->service.GET(this->implementation, name, ec);
      else
      if (rm == "POST")
        return this->service.POST(this->implementation, name, ec, greedy);
      else
        return "";
    }

    /// Find the cookie meta-variable matching name
    /**
     * @throws `boost::system::system_error` if an error occurred. This may
     * fail with `cgi::error::request_aborted` if the request has been aborted
     * by the client.
     */
    std::string cookie(const std::string& name)
    {
      boost::system::error_code ec;
      std::string ret
        = this->service.cookie(this->implementation, name, ec);
      detail::throw_error(ec);
      return ret;
    }

    /// Find the cookie meta-variable matching name
    /**
     * @param ec Set such that `(!ec == false)` if an error occurred. This may
     * fail with `ec == cgi::error::request_aborted` if the request has been
     * aborted by the client.
     */
    std::string cookie(const std::string& name, boost::system::error_code& ec)
    {
      return this->service.cookie(this->implementation, name, ec);
    }

    /// Find the environment meta-variable matching name
    /**
     * @throws `boost::system::system_error` if an error occurred. This may
     * fail with `cgi::error::request_aborted` if the request has been aborted
     * by the client.
     */
    std::string env(const std::string& name)
    {
      boost::system::error_code ec;
      std::string ret = this->service.env(this->implementation, name, ec);
      detail::throw_error(ec);
      return ret;
    }

    /// Find the environment meta-variable matching name
    /**
     * @param ec Set such that `(!ec == false)` if an error occurred. This may
     * fail with `ec == cgi::error::request_aborted` if the request has been
     * aborted by the client.
     */
    std::string env(const std::string& name, boost::system::error_code& ec)
    {
      return this->service.env(this->implementation, name, ec);
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
    std::string var(std::string const& name, bool greedy = false)
    {
      boost::system::error_code ec;
      std::string ret = var(name, ec, greedy);
      return this->service.var(this->implementation, name, greedy);
      std::string request_method( env("REQUEST_METHOD") );

      std::string tmp;

      // If it's not a POST request search meta_get first (to save time)
      if (request_method.empty() || request_method == "GET")
      {
        tmp = GET(name);
        if (!tmp.empty())
          return tmp;
      }

      tmp = cookie(name);
      if (!tmp.empty())
	      return tmp;

      tmp = env(name);
      if (!tmp.empty())
	      return tmp;

      if (!request_method.empty() && request_method == "POST")
      {
        tmp = POST(name);
        if (!tmp.empty())
          return tmp;
      }

      tmp = GET(name);
      return tmp.empty() ? "" : tmp;
    }

    // [helper-functions for the basic CGI 1.1 meta-variables.
    std::string auth_type()
    { return env("AUTH_TYPE"); }

    std::string content_length()
    { return env("CONTENT_LENGTH"); }

    std::string content_type()
    { return env("CONTENT_TYPE"); }

    std::string gateway_interface()
    { return env("GATEWAY_INTERFACE"); }

    std::string path_info()
    { return env("PATH_INFO"); }

    std::string path_translated()
    { return env("PATH_TRANSLATED"); }

    std::string query_string()
    { return env("QUERY_STRING"); }

    std::string remote_addr()
    { return env("REMOTE_ADDR"); }

    std::string remote_host()
    { return env("REMOTE_HOST"); }

    std::string remote_ident()
    { return env("REMOTE_IDENT"); }

    std::string remote_user()
    { return env("REMOTE_USER"); }

    std::string request_method()
    { return env("REQUEST_METHOD"); }

    std::string script_name()
    { return env("SCRIPT_NAME"); }

    std::string server_name()
    { return env("SERVER_NAME"); }

    std::string server_port()
    { return env("SERVER_PORT"); }

    std::string server_protocol()
    { return env("SERVER_PROTOCOL"); }

    std::string server_software()
    { return env("SERVER_SOFTWARE"); }
    // -- end helper-functions]

    /// Get the charset from the CONTENT_TYPE header
    std::string charset()
    {
      // Not sure if regex is needlessly heavy-weight here.
      boost::regex re(";[ ]?charset=([-\\w]+);");
      boost::smatch match;
      if (!boost::regex_match(this->content_type(), match, re))
        return ""; // A default could go here.

      return match[1];
    }

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
      return this->service.get_role(this->implementation);
    }

    void set_status(http::status_code const& status)
    {
      this->service.set_status(this->implementation, status);
    }

    ////////////////////////////////////////////////////////////
    // Note on operator[]
    // ------------------
    // It is overloaded on different enum types to allow
    // compile-time (I hope) retrieval of different data
    // maps.
    //
    /// Get a `common::env_map&` of all the environment variables.
    env_map& operator[](common::env_data_type const&)
    {
      return this->implementation.env_vars();
    }

    /// Get a `common::get_map&` of all the GET variables.
    get_map& operator[](common::get_data_type const&)
    {
      return this->implementation.get_vars();
    }

    /// Get a `common::post_map&` of all the POST variables.
    post_map& operator[](common::post_data_type const&)
    {
      return this->implementation.post_vars();
    }

    /// Get a `common::cookie_map&` of all the cookies.
    cookie_map& operator[](common::cookie_data_type const&)
    {
      return this->implementation.cookie_vars();
    }

    /// Get a `common::form_map&` of either the GET or POST variables.
    form_map& operator[](common::form_data_type const&)
    {
      if (request_method() == "GET")
        return this->implementation.get_vars();
      else
      if (request_method() == "POST")
        return this->implementation.post_vars();
      else
        return this->implementation.env_vars();
    }
    ////////////////////////////////////////////////////////////

    /// The id of this request.
    /**
     * This is 1 for CGI/aCGI requests, but may be != 1 for FastCGI requests.
     * Note that for FastCGI requests, the id's are assigned on a
     * *per-connection* policy, so in one application you may have several
     * requests with the same id.
     */
    int id()
    {
      return this->service.request_id(this->implementation);
    }
  };

 } // namespace common
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

