//              -- common/request_base.hpp --
//
//            Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
//
// Defines `request_base` - a base class for all CGI requests.
// Contains common data members and functionality.
//
#ifndef CGI_COMMON_REQUEST_BASE_HPP_INCLUDE_
#define CGI_COMMON_REQUEST_BASE_HPP_INCLUDE_

#include <string>
////////////////////////////////////////////////////////////////
#include <boost/asio/buffer.hpp>
#include <boost/system/error_code.hpp>
////////////////////////////////////////////////////////////////
#include "boost/cgi/common/map.hpp"
#include "boost/cgi/common/parse_options.hpp"
#include "boost/cgi/detail/extract_params.hpp"

namespace cgi {
 namespace common {

  /// ABC that defines the common interface for basic_request<>s
  /**
   * This class provides generic member functions that can be used by any
   * request type.
   */
  template<typename T>
  class request_base
  {
  public:

    /// Get the request ID of a FastCGI request, or 1.
    template<typename ImplType>
    int request_id(ImplType& impl)
    {
      return impl.client_.request_id_;
    }

    // impl_base is the common base class for all request types'
    // implementation_type and should be inherited by it.
    struct impl_base
    {
      typedef char                              char_type; // **FIXME**
      typedef std::basic_string<char_type>      string_type;
      typedef std::vector<char_type>            buffer_type;
      typedef boost::asio::const_buffers_1      const_buffers_type;
      typedef boost::asio::mutable_buffers_1    mutable_buffers_type;
 
      typedef boost::fusion::vector<
          common::env_map, common::get_map
        , common::post_map, common::cookie_map
      >   var_map_type;

      var_map_type vars_;
      buffer_type post_buffer_;

      mutable_buffers_type prepare(std::size_t size)
      {
        std::size_t bufsz(post_buffer_.size());
        post_buffer_.resize(bufsz + size);
        return boost::asio::buffer(&post_buffer_[bufsz], size);
      }
    };
     
    /// Read some data into the internal buffer.
    template<typename ImplType>
    std::size_t
    read_some(ImplType& impl, boost::system::error_code& ec)
    {
      return impl.client_.read_some(impl.prepare(64), ec);
    }

    /// Read some data from the client into the supplied buffer.
    template<typename ImplType, typename MutableBufferSequence>
    std::size_t
    read_some(ImplType& impl, const MutableBufferSequence& buf
             , boost::system::error_code& ec)
    {
      return impl.client_.read_some(buf,ec);
    }

    /// Synchronously read/parse the request meta-data
    template<typename ImplType>
    boost::system::error_code
    load(ImplType& impl, common::parse_options parse_opts
        , boost::system::error_code& ec)
    {
      if (parse_opts & common::parse_env)
      {
        if (!read_env_vars(impl, ec)) // returns an error_code
          return ec;
      }

      std::string const& cl = env_vars(impl.vars_)["CONTENT_LENGTH"];
      impl.characters_left_ = cl.empty() ? 0 : boost::lexical_cast<std::size_t>(cl);
      impl.client_.bytes_left() = impl.characters_left_;

      std::string const& request_method = env_vars(impl.vars_)["REQUEST_METHOD"];

      if (request_method == "GET" && parse_opts & common::parse_get)
      {
        parse_get_vars(impl, ec);
      }
      else
      if (request_method == "POST" && parse_opts & common::parse_post)
      {
        parse_post_vars(impl, ec);
      }

      if (ec) return ec;

      if (parse_opts & common::parse_cookie)
      {
        if (!parse_cookie_vars(impl, ec)) // returns an error_code
          return ec;
      }

      set_status(impl, common::loaded);

      return ec;
    }

    /// Read and parse the cgi GET meta variables
    template<typename ImplType>
    boost::system::error_code
    parse_get_vars(ImplType& impl, boost::system::error_code& ec)
    {
      // Make sure the request is in a pre-loaded state
      //BOOST_ASSERT (impl.status() <= unloaded);

      std::string const& vars (env_vars(impl.vars_)["QUERY_STRING"]);
      if (!vars.empty())
        detail::extract_params(vars, get_vars(impl.vars_)
                              , boost::char_separator<char>
                                 ("", "=&", boost::keep_empty_tokens)
                              , ec);

      return ec;
    }

    /// Read and parse the HTTP_COOKIE meta variable
    template<typename ImplType>
    boost::system::error_code
    parse_cookie_vars(ImplType& impl, boost::system::error_code& ec)
    {
      // Make sure the request is in a pre-loaded state
      //BOOST_ASSERT (impl.status() <= unloaded);

      std::string const& vars (env_vars(impl.vars_)["HTTP_COOKIE"]);
      if (!vars.empty())
        detail::extract_params(vars, cookie_vars(impl.vars_)
                              , boost::char_separator<char>
                                  ("", "=;", boost::keep_empty_tokens)
                              , ec);

      return ec;
    }
  };

 } // namespace common
} // namespace cgi

#endif // CGI_COMMON_REQUEST_BASE_HPP_INCLUDE_

