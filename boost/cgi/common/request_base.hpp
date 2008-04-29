#ifndef CGI_COMMON_REQUEST_BASE_HPP_INCLUDE_
#define CGI_COMMON_REQUEST_BASE_HPP_INCLUDE_

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
  protected:
    
    /// Read and parse the cgi GET meta variables
    template<typename ImplType>
    boost::system::error_code
    parse_get_vars(ImplType& impl, boost::system::error_code& ec)
    {
      // Make sure the request is in a pre-loaded state
      //BOOST_ASSERT (impl.status() <= unloaded);

      std::string& vars(impl.env_vars_["QUERY_STRING"]);
      if (vars.empty())
        return ec;

      detail::extract_params(vars, impl.get_vars_
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

      std::string& vars(impl.env_vars_["HTTP_COOKIE"]);
      if (vars.empty())
        return ec;

      detail::extract_params(vars, impl.cookie_vars_
                            , boost::char_separator<char>
                                ("", "=;", boost::keep_empty_tokens)
                            , ec);

      return ec;
    }

  };

 } // namespace common
} // namespace cgi

#endif // CGI_COMMON_REQUEST_BASE_HPP_INCLUDE_

