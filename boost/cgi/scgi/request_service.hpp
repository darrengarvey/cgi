//             -- scgi/request_service.hpp --
//
//            Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_SCGI_REQUEST_SERVICE_HPP_INCLUDED__
#define CGI_SCGI_REQUEST_SERVICE_HPP_INCLUDED__

#include <boost/system/error_code.hpp>

#include "request_impl.hpp"
#include "../map.hpp"
#include "../tags.hpp"
#include "../role_type.hpp"
#include "../io_service.hpp"
#include "../detail/throw_error.hpp"
#include "../detail/service_base.hpp"
#include "../detail/extract_params.hpp"

namespace cgi {

  class scgi_request_service
    : public detail::service_base<scgi_request_service>
  {
  public:
    typedef tags::scgi          protocol_type;
    typedef scgi_request_impl   implementation_type;
    typedef cgi::map            map_type;

    scgi_request_service(cgi::io_service& ios)
      : detail::service_base<scgi_request_service>(ios)
      , io_service_(ios)
    {
    }

    ~scgi_request_service()
    {
    }

    void construct(implementation_type& impl)
    {
      impl.connection()
        = implementation_type::connection_type::create(this->io_service());
    }

    void destroy(implementation_type& impl)
    {
      //impl.set_state(aborted);
    }

    boost::system::error_code& load(implementation_type& impl, bool parse_stdin
                                   , boost::system::error_code& ec)
    {
      const std::string& request_method = meta_env(impl, "REQUEST_METHOD", ec);
      if (request_method == "GET")
        if (parse_get_vars(impl, ec))
	      return ec;
      else
      if (request_method == "POST" && parse_stdin)
        if (parse_post_vars(impl, ec))
	      return ec;

      parse_cookie_vars(impl, ec);
      return ec;
    }

    template<typename MutableBufferSequence>
    std::size_t read_some(implementation_type& impl
                         , const MutableBufferSequence& buf
                         , boost::system::error_code& ec)
    {
      std::size_t s = impl.connection()->read_some(buf, ec);
      return s;
    }

    template<typename ConstBufferSequence>
    std::size_t write_some(implementation_type& impl
                          , const ConstBufferSequence& buf
                          , boost::system::error_code& ec)
    {
      return impl.connection()->write_some(buf, ec);
    }

    //template<typename VarType> map_type& var(implementation_type&) const;

	std::string meta_get(implementation_type& impl, const std::string& name
                        , boost::system::error_code& ec)
    {
      return var(impl.get_vars_, name, ec);
    }

    map_type& meta_get(implementation_type& impl)
    {
      return impl.get_vars_;
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
    std::string meta_post(implementation_type& impl, const std::string& name
                         , boost::system::error_code& ec
                         , bool greedy = true)
    {
      const std::string& val = var(impl.post_vars_, name, ec);
      if (val.empty() && greedy && !ec)
      {

      }

      return val;
    }

    map_type& meta_post(implementation_type& impl)
    {
      return impl.post_vars_;
    }


    /// Find the cookie meta-variable matching name
    std::string cookie(implementation_type& impl, const std::string& name
                      , boost::system::error_code& ec)
    {
      return var(impl.cookie_vars_, name, ec);
    }

    map_type& meta_cookie(implementation_type& impl)
    {
      return impl.cookie_vars_;
    }


    /// Find the environment meta-variable matching name
    std::string meta_env(implementation_type& impl, const std::string& name
                        , boost::system::error_code& ec)
    {
      return var(impl.env_vars_, name, ec);
    }


    role_type get_role(implementation_type& impl)
    {
      return responder;
    }

  protected:
    /// Extract the var value from 
    std::string var(map_type& meta_data, const std::string& name
                   , boost::system::error_code& ec)
    {
      /* Alt:
      if ((typename map_type::iterator pos = meta_data.find(name))
             != meta_data.end())
      {
        return *pos;
      }
      return std::string();
      **/

      if( meta_data.find(name) != meta_data.end() )
        return meta_data[name];
      return "";
    }

    /// Read and parse the cgi GET meta variables
    boost::system::error_code&
    parse_get_vars(implementation_type& impl, boost::system::error_code& ec)
    {
      detail::extract_params(meta_env(impl, "QUERY_STRING", ec)
                    , impl.get_vars_
                    , boost::char_separator<char>
                        ("", "=&", boost::keep_empty_tokens)
                    , ec);

      return ec;
    }

    /// Read and parse the HTTP_COOKIE meta variable
    template<typename RequestImpl>
    boost::system::error_code&
    parse_cookie_vars(implementation_type& impl, boost::system::error_code& ec)
    {
      // Make sure this function hasn't already been called
      //BOOST_ASSERT( impl.cookie_vars_.empty() );

      std::string vars = meta_env(impl, "HTTP_COOKIE", ec);
      if (vars.empty())
        return ec;

      detail::extract_params(meta_env(impl, "HTTP_COOKIE", ec)
                            , impl.cookie_vars_
                            , boost::char_separator<char>
                                ("", "=&", boost::keep_empty_tokens)
                            , ec);

      return ec;
    }

    /// Read and parse the cgi POST meta variables (greedily)
    template<typename RequestImpl>
    boost::system::error_code&
    parse_post_vars(implementation_type& impl, boost::system::error_code& ec)
    {
      // Make sure this function hasn't already been called
      //BOOST_ASSERT( impl.post_vars_.empty() );
	  
      //#     error "Not implemented"

      if (impl.stdin_parsed_)
      {
      }

      return ec;
    }

    /// Read and parse a single cgi POST meta variable (greedily)
    template<typename RequestImpl>
    boost::system::error_code&
    parse_one_post_var(implementation_type& impl, boost::system::error_code& ec)
    {
      //#     error "Not implemented"
      return ec;
    }

  private:
    cgi::io_service& io_service_;
  };

} // namespace cgi

#endif // CGI_SCGI_REQUEST_SERVICE_HPP_INCLUDED__
