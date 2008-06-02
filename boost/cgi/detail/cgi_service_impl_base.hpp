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
///////////////////////////////////////////////////////////
#include <boost/assert.hpp>
#include <boost/regex.hpp>
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/system/error_code.hpp>
#include <boost/algorithm/string/find.hpp>
///////////////////////////////////////////////////////////
#include "boost/cgi/common/map.hpp"
#include "boost/cgi/basic_client.hpp"
#include "boost/cgi/common/role_type.hpp"
#include "boost/cgi/common/form_part.hpp"
#include "boost/cgi/detail/throw_error.hpp"
#include "boost/cgi/common/status_type.hpp"
#include "boost/cgi/common/form_parser.hpp"
#include "boost/cgi/common/request_base.hpp"
#include "boost/cgi/detail/extract_params.hpp"
#include "boost/cgi/detail/save_environment.hpp"

namespace cgi {
// **FIXME** Wrong namespace (should be cgi::detail?).

  template<typename RequestImplType>
  class cgi_service_impl_base
    : public common::request_base<cgi_service_impl_base<RequestImplType> >
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
      , common::request_base<
            cgi_service_impl_base<RequestImplType>
        >::impl_base
    {
      typedef typename RequestImplType::client_type    client_type;
      typedef detail::form_parser<implementation_type> form_parser_type;

      implementation_type()
        : fp_(NULL)
      {
      }

      client_type client_;

      // The number of characters left to read (ie. "content_length - bytes_read")
      std::size_t characters_left_;
      
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
      return impl.status() >= common::aborted;
    }

    /// Return the connection associated with the request
    typename implementation_type::client_type&
      client(implementation_type& impl)
    {
      return impl.client_;
    }

    int request_id(implementation_type& impl) { return 1; }

    // **FIXME** should return error_code
    int close(implementation_type& impl, common::http::status_code& http_s
      , int status, boost::system::error_code& ec)
    {
      impl.status() = common::closed;
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
      detail::save_environment(env_vars(impl.vars_));
      std::string const& cl = env_vars(impl.vars_)["CONTENT_LENGTH"];
      impl.characters_left_ = cl.empty() ? 0 : boost::lexical_cast<std::size_t>(cl);
      impl.client_.bytes_left() = impl.characters_left_;

      std::string const& request_method = env_vars(impl.vars_)["REQUEST_METHOD"];
      if (request_method == "GET")
        this->parse_get_vars(impl, ec);
      else
      if (request_method == "POST" && parse_stdin)
        parse_post_vars(impl, ec);

      if (ec) return ec;

      this->parse_cookie_vars(impl, ec);
      impl.status() = common::loaded;

      //BOOST_ASSERT(impl.status() >= loaded);

      return ec;
    }
/*
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
*/
    role_type
      get_role(implementation_type& impl)
    {
      return responder;
    }

    /// Set the http status (this does nothing for aCGI)
    void set_status(implementation_type& impl, common::http::status_code&)
    {
    }

    /// Read some data from the client.
    template<typename MutableBufferSequence>
    std::size_t
      read_some(implementation_type& impl, const MutableBufferSequence& buf
               , boost::system::error_code& ec)
    {
      return impl.client_.read_some(buf,ec);
    }

  protected:
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

