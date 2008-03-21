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

//#include "boost/cgi/scgi/request_impl.hpp"
#include "boost/cgi/map.hpp"
#include "boost/cgi/tags.hpp"
#include "boost/cgi/read.hpp"
#include "boost/cgi/role_type.hpp"
#include "boost/cgi/io_service.hpp"
#include "boost/cgi/basic_client.hpp"
#include "boost/cgi/connections/tcp_socket.hpp"
#include "boost/cgi/detail/throw_error.hpp"
#include "boost/cgi/detail/service_base.hpp"
#include "boost/cgi/detail/extract_params.hpp"

namespace cgi {
 namespace scgi {

  /// The IoObjectService class for a SCGI basic_request<>s
  class scgi_request_service
    : public detail::service_base<scgi_request_service>
  {
  public:
    /// The actual implementation date for an SCGI request.
    struct implementation_type
    {
      typedef ::cgi::map                        map_type;
      typedef tcp_connection                    connection_type;
      typedef ::cgi::scgi_                      protocol_type;
      typedef basic_client<
        connection_type, protocol_type
      >                                         client_type;

      implementation_type()
        : client_()
        , stdin_parsed_(false)
        , http_status_(http::no_content)
        , request_status_(unloaded)
        , all_done_(false)
      {
      }

      client_type client_;

      bool stdin_parsed_;
      http::status_code http_status_;
      status_type request_status_;

      map_type env_vars_;
      map_type get_vars_;
      map_type post_vars_;
      map_type cookie_vars_;

      std::string null_str_;
      bool all_done_;
    };

    typedef scgi_request_service                      type;
    typedef type::implementation_type::protocol_type  protocol_type;
    typedef type::implementation_type::map_type       map_type;

    scgi_request_service(::cgi::io_service& ios)
      : detail::service_base<scgi_request_service>(ios)
    {
    }

    ~scgi_request_service()
    {
    }

    void construct(implementation_type& impl)
    {
      //std::cerr<< "request_service.hpp:83 Creating connection" << std::endl;
      impl.client_.set_connection(//new implementation_type::connection_type(this->io_service()));
        implementation_type::connection_type::create(this->io_service())
      );
      //std::cerr<< "conn.is_open() == " << impl.client_.is_open() << std::endl;
    }

    void destroy(implementation_type& impl)
    {
      //if (!impl.all_done_)
      //  detail::abort_impl(impl); // this function isn't implemented yet!
      //impl.set_state(aborted);
    }

    void shutdown_service()
    {
    }

    bool is_open(implementation_type& impl)
    {
      return !impl.all_done_ && impl.client_.is_open();
    }

    /// Close the request.
    int close(implementation_type& impl, http::status_code& hsc
              , int program_status)
    {
      impl.all_done_ = true;
      impl.client_.close();
      return program_status;
    }

    boost::system::error_code&
      load(implementation_type& impl, bool parse_stdin
          , boost::system::error_code& ec)
    {
      //int header_len( get_length_of_header(impl, ec) );
      BOOST_ASSERT(!ec);

      std::vector<char> buf;
      // read the header content
      //::cgi::read(impl.client_, buffer(buf, header_len), ec);
/*
      const std::string& request_method = env(impl, "REQUEST_METHOD", ec);
      if (request_method == "GET")
        if (parse_get_vars(impl, ec))
	      return ec;
      else
      if (request_method == "POST" && parse_stdin)
        if (parse_post_vars(impl, ec))
	      return ec;

      parse_cookie_vars(impl, ec);
  */    return ec;
    }

    /* These Don't Belong Here.
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
   ********************************************/

    std::string GET(implementation_type& impl, const std::string& name
                   , boost::system::error_code& ec)
    {
      return var(impl.get_vars_, name, ec);
    }

    map_type& GET(implementation_type& impl)
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
    std::string POST(implementation_type& impl, const std::string& name
                    , boost::system::error_code& ec
                    , bool greedy = true)
    {
      const std::string& val = var(impl.post_vars_, name, ec);
      if (val.empty() && greedy && !ec)
      {

      }

      return val;
    }

    map_type& POST(implementation_type& impl)
    {
      return impl.post_vars_;
    }


    /// Find the cookie meta-variable matching name
    std::string cookie(implementation_type& impl, const std::string& name
                      , boost::system::error_code& ec)
    {
      return var(impl.cookie_vars_, name, ec);
    }

    map_type& cookie(implementation_type& impl)
    {
      return impl.cookie_vars_;
    }


    /// Find the environment meta-variable matching name
    std::string env(implementation_type& impl, const std::string& name
                   , boost::system::error_code& ec)
    {
      return var(impl.env_vars_, name, ec);
    }


    role_type get_role(implementation_type& impl)
    {
      return responder;
    }

    implementation_type::client_type&
      client(implementation_type& impl)
    {
      return impl.client_;
    }

  protected:
    /// Extract the var value from 
    std::string var(map_type& _data, const std::string& _name
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

      if( _data.find(_name) != _data.end() )
        return _data[_name];
      return "";
    }

    /// Read and parse the cgi GET meta variables
    boost::system::error_code&
    parse_get_vars(implementation_type& impl, boost::system::error_code& ec)
    {
      detail::extract_params(env(impl, "QUERY_STRING", ec)
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

      std::string vars = env(impl, "HTTP_COOKIE", ec);
      if (vars.empty())
        return ec;

      detail::extract_params(env(impl, "HTTP_COOKIE", ec)
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
    //cgi::io_service& io_service_;
  };

 } // namespace scgi
} // namespace cgi

#endif // CGI_SCGI_REQUEST_SERVICE_HPP_INCLUDED__
