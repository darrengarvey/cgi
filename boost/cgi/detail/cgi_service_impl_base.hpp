//        -- detail/cgi_service_impl_base.hpp --
//
//           Copyright (c) Darren Garvey 2007-2008.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_CGI_SERVICE_IMPL_BASE_HPP_INCLUDED__
#define CGI_CGI_SERVICE_IMPL_BASE_HPP_INCLUDED__

#include "boost/cgi/detail/push_options.hpp"

#include <string>
#include <cstdlib>
#include <boost/assert.hpp>
#include <boost/regex.hpp>
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/system/error_code.hpp>
#include <boost/algorithm/string/find.hpp>

#include "boost/cgi/common/map.hpp"
#include "boost/cgi/basic_client.hpp"
#include "boost/cgi/role_type.hpp"
#include "boost/cgi/status_type.hpp"
#include "boost/cgi/detail/extract_params.hpp"
#include "boost/cgi/detail/save_environment.hpp"
#include "boost/cgi/common/form_part.hpp"
#include "boost/cgi/detail/throw_error.hpp"

#include "boost/cgi/common/form_parser.hpp"

namespace cgi {

 namespace detail {


 } // namespace detail


  template<typename RequestImplType>
  class cgi_service_impl_base
  {
  public:
    //typedef RequestImplType     implementation_type;
    typedef ::cgi::common::map          map_type;

    cgi_service_impl_base()
    {
    }

    template<typename T>
    cgi_service_impl_base(T&)
    {
    }

    struct implementation_type
      : RequestImplType
    {
      typedef boost::asio::const_buffers_1             const_buffers_type;
      typedef boost::asio::mutable_buffers_1           mutable_buffers_type;
      typedef typename RequestImplType::client_type    client_type;
      typedef std::vector<char>                        buffer_type;
      typedef detail::form_parser<implementation_type> form_parser_type;

      implementation_type()
        : fp_(NULL)
      {
      }

      client_type client_;

      // The number of characters left to read (ie. "content_length - bytes_read")
      std::size_t characters_left_;
      
      buffer_type buf_;

      std::vector<common::form_part> form_parts_;
      
      boost::scoped_ptr<form_parser_type> fp_;

    };

    /// Return if the request is still open
    /**
     * For CGI, this always returns true. However, in the case that a
     * "Location: xxx" header is sent and the header is terminated, the
     * request can be taken to be 'closed'.
     */
    bool is_open(implementation_type& impl)
    {
      return impl.status() >= aborted;
    }

    /// Return the connection associated with the request
    typename implementation_type::client_type&
      client(implementation_type& impl)
    {
      return impl.client_;
    }

    int close(implementation_type& impl, http::status_code& http_s, int status)
    {
      impl.status() = closed;
      impl.http_status() = http_s;
      return status;
    }

    /// Synchronously read/parse the request meta-data
    /**
     * @param parse_stdin if true then STDIN data is also read/parsed
     */
    boost::system::error_code&
      load(implementation_type& impl, bool parse_stdin
          , boost::system::error_code& ec)
    {
      detail::save_environment(impl.env_vars());
      const std::string& cl = var(impl.env_vars(), "CONTENT_LENGTH", ec);
      impl.characters_left_ = cl.empty() ? 0 : boost::lexical_cast<std::size_t>(cl);
      impl.client_.bytes_left() = impl.characters_left_;

      const std::string& request_method = var(impl.env_vars(), "REQUEST_METHOD", ec);
      if (request_method == "GET")
        parse_get_vars(impl, ec);
      else
      if (request_method == "POST" && parse_stdin)
        parse_post_vars(impl, ec);

      if (ec) return ec;

      parse_cookie_vars(impl, ec);
      impl.status() = loaded;

      //BOOST_ASSERT(impl.status() >= loaded);

      return ec;
    }

    std::string&
      var(map_type& meta_data, const std::string& name
         , boost::system::error_code& ec)
    {
      return meta_data[name.c_str()];
    }

    std::string
      GET(implementation_type& impl, const std::string& name
         , boost::system::error_code& ec)
    {
      //BOOST_ASSERT(impl.status() >= loaded);
      return var(impl.get_vars(), name, ec);
    }

    map_type&
      GET(implementation_type& impl)
    {
      //BOOST_ASSERT(impl.status() >= loaded);
      return impl.get_vars();
    }

    /// Find the post meta-variable matching name
    /**
     * @param greedy This determines whether more data can be read to find
     * the variable. The default is true to cause least surprise in the common
     * case of having not parsed any of the POST data.

     -----------------------------------------------
     Should this return a pair of iterators instead?
     What about url_decoding?
     -----------------------------------------------

     */
    std::string
      POST(implementation_type& impl, const std::string& name
          , boost::system::error_code& ec, bool greedy = true)
    {
      //BOOST_ASSERT(impl.status() >= loaded);
      const std::string& val = var(impl.post_vars(), name, ec);
      if (val.empty() && greedy && !ec)
      {

      }

      return val;
    }

    map_type&
      POST(implementation_type& impl)
    {
      //BOOST_ASSERT(impl.status() >= loaded);
      return impl.post_vars();
    }

    // prefer this to the above
    map_type&
      POST(implementation_type& impl, boost::system::error_code& ec)
    {
      //BOOST_ASSERT(impl.status() >= loaded);
      return impl.post_vars();
    }

    // TODO: use `greedy`
    std::string
      form(implementation_type& impl, const std::string& name
		      , boost::system::error_code& ec, bool greedy)
		{
		  std::string rm(env(impl, "REQUEST_METHOD", ec));
			//BOOST_ASSERT(!ec && !rm.empty());
			if (rm == "GET")
			  return GET(impl, name, ec);
		  else
			if (rm == "POST")
			  return POST(impl, name, ec);
			else
        // **FIXME** What about HEAD, PUT ???
			  return "***BOOST_CGI_ERROR_INVALID_REQUEST_METHOD***";
		}
		
    // TODO: use `greedy`
		map_type&
      form(implementation_type& impl, boost::system::error_code& ec
		      , bool greedy)
		{
		  std::string rm(env(impl, "REQUEST_METHOD", ec));
			//BOOST_ASSERT(!ec && !rm.empty());
			if (rm == "GET")
			  return GET(impl);
		  else
			if (rm == "POST")
			  return POST(impl, ec);
		}

    /// Find the cookie meta-variable matching name
    std::string
      cookie(implementation_type& impl, const std::string& name
            , boost::system::error_code& ec)
    {
      //BOOST_ASSERT(impl.status() >= loaded);
      return var(impl.cookie_vars(), name, ec);
    }

    map_type&
      cookie(implementation_type& impl)
    {
      //BOOST_ASSERT(impl.status() >= loaded);
      return impl.cookie_vars();
    }


    /// Find the environment meta-variable matching name
    std::string
      env(implementation_type& impl, const std::string& name
         , boost::system::error_code& ec)
    {
      //BOOST_ASSERT(impl.status() >= loaded);
      const char* c = ::getenv(name.c_str());
      return c ? c : std::string();
    }

    map_type&
      env(implementation_type& impl)
    {
      //BOOST_ASSERT(impl.status() >= loaded);
      return impl.env_vars();
    }


    role_type
      get_role(implementation_type& impl)
    {
      return responder;
    }

    /// Set the http status (this does nothing for aCGI)
    void set_status(implementation_type& impl, http::status_code&)
    {
    }

  protected:
    /// Read and parse the cgi GET meta variables
    template<typename RequestImpl>
    boost::system::error_code
    parse_get_vars(RequestImpl& impl, boost::system::error_code& ec)
    {
      // Make sure the request is in a pre-loaded state
      //BOOST_ASSERT (impl.status() <= unloaded);

      std::string& vars = impl.env_vars()["QUERY_STRING"];
      if (vars.empty())
        return ec;

      detail::extract_params(vars, impl.get_vars()
                            , boost::char_separator<char>
                                ("", "=&", boost::keep_empty_tokens)
                            , ec);

      return ec;
    }

    /// Read and parse the HTTP_COOKIE meta variable
    template<typename RequestImpl>
    boost::system::error_code
    parse_cookie_vars(RequestImpl& impl, boost::system::error_code& ec)
    {
      // Make sure the request is in a pre-loaded state
      //BOOST_ASSERT (impl.status() <= unloaded);

      std::string& vars(impl.env_vars()["HTTP_COOKIE"]);
      if (vars.empty())
        return ec;

      detail::extract_params(vars, impl.cookie_vars()
                            , boost::char_separator<char>
                                ("", "=;", boost::keep_empty_tokens)
                            , ec);

      return ec;
    }

    /// Read and parse the cgi POST meta variables (greedily)
    template<typename RequestImpl>
    boost::system::error_code
    parse_post_vars(RequestImpl& impl, boost::system::error_code& ec)
    {
      // Make sure this function hasn't already been called
      //BOOST_ASSERT (!impl.stdin_parsed());

      impl.fp_.reset
      (
        new typename implementation_type::form_parser_type
                ( impl )
      );
      impl.fp_->parse(ec);
      return ec;
    }
  };

} // namespace cgi

#include "boost/cgi/detail/pop_options.hpp"

#endif // CGI_CGI_SERVICE_IMPL_HPP_INCLUDED__

