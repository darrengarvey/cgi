#ifndef CGI_CGI_SERVICE_IMPL_BASE_HPP_INCLUDED__
#define CGI_CGI_SERVICE_IMPL_BASE_HPP_INCLUDED__

#include "../detail/push_options.hpp"

#include <string>
#include <cstdlib>
#include <iostream>
#include <boost/assert.hpp>
#include <boost/tokenizer.hpp>
#include <boost/system/error_code.hpp>

#include "boost/cgi/map.hpp"
#include "boost/cgi/role_type.hpp"
#include "boost/cgi/status_type.hpp"
#include "boost/cgi/detail/extract_params.hpp"
#include "boost/cgi/detail/save_environment.hpp"
//#include "../connections/stdio.hpp"

namespace cgi {

 namespace detail {


 } // namespace detail

  using std::cerr;
  using std::endl;


  template<typename RequestImplType>
  class cgi_service_impl_base
  {
  public:
    typedef RequestImplType    implementation_type;
    typedef cgi::map           map_type;

    cgi_service_impl_base()
    {
    }

    template<typename T>
    cgi_service_impl_base(T&)
    {
    }

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
    typename implementation_type::connection_type&
    client(implementation_type& impl)
    {
      return *impl.connection();
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
      const std::string& request_method = impl.env_vars()["REQUEST_METHOD"];
      if (request_method == "GET")
        parse_get_vars(impl, ec);
      else
      if (request_method == "POST" && parse_stdin)
        parse_post_vars(impl, ec);

      if (ec) return ec;

      parse_cookie_vars(impl, ec);
      impl.status() = loaded;
      BOOST_ASSERT(impl.status() >= loaded);

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

    std::string& var(map_type& meta_data, const std::string& name
                   , boost::system::error_code& ec)
    {
      return meta_data[name];
    }

    std::string meta_get(implementation_type& impl, const std::string& name
                        , boost::system::error_code& ec)
    {
      BOOST_ASSERT(impl.status() >= loaded);
      return var(impl.get_vars(), name, ec);
    }

    map_type& meta_get(implementation_type& impl)
    {
      BOOST_ASSERT(impl.status() >= loaded);
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
    std::string meta_post(implementation_type& impl, const std::string& name
                         , boost::system::error_code& ec
                         , bool greedy = true)
    {
      BOOST_ASSERT(impl.status() >= loaded);
      const std::string& val = var(impl.post_vars(), name, ec);
      if (val.empty() && greedy && !ec)
      {

      }

      return val;
    }

    map_type& meta_post(implementation_type& impl)
    {
      BOOST_ASSERT(impl.status() >= loaded);
      return impl.post_vars();
    }


    /// Find the cookie meta-variable matching name
    std::string cookie(implementation_type& impl, const std::string& name
                      , boost::system::error_code& ec)
    {
      BOOST_ASSERT(impl.status() >= loaded);
      return var(impl.cookie_vars(), name, ec);
    }

    map_type& meta_cookie(implementation_type& impl)
    {
      BOOST_ASSERT(impl.status() >= loaded);
      return impl.cookie_vars();
    }


    /// Find the environment meta-variable matching name
    std::string meta_env(implementation_type& impl, const std::string& name
                        , boost::system::error_code& ec)
    {
      BOOST_ASSERT(impl.status() >= loaded);
      const char* c = ::getenv(name.c_str());
      return c ? c : impl.null_str();
    }

    map_type& meta_env(implementation_type& impl)
    {
      BOOST_ASSERT(impl.status() >= loaded);
      return impl.env_vars();
    }


    role_type get_role(implementation_type& impl)
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
    boost::system::error_code&
    parse_get_vars(RequestImpl& impl, boost::system::error_code& ec)
    {
      // Make sure the request is in a pre-loaded state
      BOOST_ASSERT (impl.status() <= unloaded);

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
    boost::system::error_code&
    parse_cookie_vars(RequestImpl& impl, boost::system::error_code& ec)
    {
      // Make sure the request is in a pre-loaded state
      BOOST_ASSERT (impl.status() <= unloaded);

      std::string& vars(impl.env_vars()["HTTP_COOKIE"]);
      if (vars.empty())
        return ec;

      detail::extract_params(vars, impl.cookie_vars()
                            , boost::char_separator<char>
                                ("", "=&", boost::keep_empty_tokens)
                            , ec);

      return ec;
    }

    /// Read and parse the cgi POST meta variables (greedily)
    template<typename RequestImpl>
    boost::system::error_code&
    parse_post_vars(RequestImpl& impl, boost::system::error_code& ec)
    {
      // Make sure this function hasn't already been called
      BOOST_ASSERT (!impl.stdin_parsed());

      std::istream& is(std::cin);
      char ch;
      std::string name;
      std::string str;
      cgi::map& post_map(impl.post_vars());

      const char* a = ::getenv("content_length");
      if (a) str = a;
      else std::cerr<< "no content-length";

      std::cerr<< "content length = ";
      //var(impl.get_vars(), "CONTENT_LENGTH", ec);
      std::cerr.flush();

      while( is.get(ch) )
      {
          std::cerr<< "; ch=" << ch << "; ";
          switch(ch)
          {
          case '%': // unencode a hex character sequence
              // note: function params are resolved in an undefined order!
              str += detail::url_decode(is);
              break;
          case '+':
              str += ' ';
              break;
          case ' ':
              continue;
          case '=': // the name is complete, now get the corresponding value
              name = str;
              str.clear();
              break;
          case '&': // we now have the name/value pair, so save it
              post_map[name] = str;
              str.clear();
              name.clear();
             break;
          default:
              str += ch;
          }
      }
      // save the last param (it won't have a trailing &)
      if( !name.empty() )
          post_map[name] = str;//.empty() ? "" : str;



      return ec;
    }

    /// Read and parse a single cgi POST meta variable (greedily)
    template<typename RequestImpl>
    boost::system::error_code&
    parse_one_post_var(RequestImpl& impl, boost::system::error_code& ec)
    {
      //#     error "Not implemented"
      return ec;
    }
  };

} // namespace cgi

#include "../detail/pop_options.hpp"

#endif // CGI_CGI_SERVICE_IMPL_HPP_INCLUDED__
