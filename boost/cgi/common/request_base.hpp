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
    /// Find the environment meta-variable matching name
    template<typename ImplType>
    std::string& env(ImplType& impl, std::string const& name
                    , boost::system::error_code& ec)
    {
      return impl.env_vars()[name.c_str()];
    }

    template<typename ImplType>
    std::string& GET(ImplType& impl, std::string const& name
                    , boost::system::error_code& ec)
    {
      return impl.get_vars()[name.c_str()];
    }

    /// Find the post meta-variable matching name
    /**
     * @param greedy This determines whether more data can be read to find
     * the variable. The default is true to cause least surprise in the common
     * case of having not parsed any of the POST data.
     */
    template<typename ImplType>
    std::string& POST(ImplType& impl, std::string const& name
                     , boost::system::error_code& ec, bool greedy)
    {
      // **FIXME** greedy isn't used: it may be unnecessary now though...
      return impl.post_vars()[name.c_str()];
    }

    /// Find the cookie meta-variable matching name
    template<typename ImplType>
    std::string&
      cookie(ImplType& impl, std::string const& name
            , boost::system::error_code& ec)
    {
      return impl.cookie_vars()[name.c_str()];
    }

    template<typename ImplType>
    int request_id(ImplType& impl)
    {
      return impl.client_.request_id_;
    }

  protected:
    // impl_base is the common base class for all request types'
    // implementation_type and should be inherited by it.
    struct impl_base
    {
      typedef std::vector<char>                 buffer_type;
      typedef boost::asio::const_buffers_1      const_buffers_type;
      typedef boost::asio::mutable_buffers_1    mutable_buffers_type;
 
      common::env_map& env_vars()       { return boost::fusion::at_c<0>(vars_); }
      common::get_map& get_vars()       { return boost::fusion::at_c<1>(vars_); }
      common::post_map& post_vars()     { return boost::fusion::at_c<2>(vars_); }
      common::cookie_map& cookie_vars() { return boost::fusion::at_c<3>(vars_); }

      typedef boost::fusion::vector<
          common::env_map, common::get_map
        , common::post_map, common::cookie_map
      >   var_map_type;

      var_map_type vars_;
      buffer_type buffer_;
    };
    
    /// Read and parse the cgi GET meta variables
    template<typename ImplType>
    boost::system::error_code
    parse_get_vars(ImplType& impl, boost::system::error_code& ec)
    {
      // Make sure the request is in a pre-loaded state
      //BOOST_ASSERT (impl.status() <= unloaded);

      std::string const& vars(impl.env_vars()["QUERY_STRING"]);
      if (vars.empty())
        return ec;

      detail::extract_params(vars, impl.get_vars()
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

      std::string const& vars(impl.env_vars()["HTTP_COOKIE"]);
      if (vars.empty())
        return ec;

      detail::extract_params(vars, impl.cookie_vars()
                            , boost::char_separator<char>
                                ("", "=;", boost::keep_empty_tokens)
                            , ec);

      return ec;
    }
  };

 } // namespace common
} // namespace cgi

#endif // CGI_COMMON_REQUEST_BASE_HPP_INCLUDE_

