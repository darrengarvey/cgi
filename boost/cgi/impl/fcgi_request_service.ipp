//             -- fcgi/request_service.ipp --
//
//            Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_FCGI_REQUEST_SERVICE_IPP_INCLUDED__
#define CGI_FCGI_REQUEST_SERVICE_IPP_INCLUDED__

#include <boost/fusion/support.hpp>
#include <boost/system/error_code.hpp>
#include <boost/fusion/include/vector.hpp>
////////////////////////////////////////////////////////////////
#include "boost/cgi/common/map.hpp"
#include "boost/cgi/common/tags.hpp"
#include "boost/cgi/fcgi/client.hpp"
#include "boost/cgi/import/read.hpp"
#include "boost/cgi/http/status_code.hpp"
#include "boost/cgi/common/role_type.hpp"
#include "boost/cgi/import/io_service.hpp"
#include "boost/cgi/detail/throw_error.hpp"
#include "boost/cgi/common/form_parser.hpp"
#include "boost/cgi/common/source_enums.hpp"
#include "boost/cgi/common/request_base.hpp"
#include "boost/cgi/detail/service_base.hpp"
#include "boost/cgi/fcgi/request_service.hpp"

namespace cgi {

   namespace detail {
     
     template<typename T, typename Handler>
     struct async_load_helper
     {
       async_load_helper(T& t, typename T::implementation_type& impl
                        , bool parse_stdin, Handler h)
         : type(t)
         , impl_(impl)
         , parse_stdin_(parse_stdin)
         , handler_(h)
       {
       }

       void operator()()
       {
         boost::system::error_code ec;
         type.load(impl_, parse_stdin_, ec);
         handler_(ec);
       }

       T& type;
       typename T::implementation_type& impl_;
       bool parse_stdin_;
       Handler handler_;
     };

  } // namespace detail

  namespace fcgi {
 
    /// Close the request.
    BOOST_CGI_INLINE int
    fcgi_request_service::close(
        implementation_type& impl, ::cgi::common::http::status_code& hsc
      , int program_status)
    {
      impl.all_done_ = true;
      impl.client_.close(program_status);
      impl.request_status_ = common::closed;
      return program_status;
    }

    BOOST_CGI_INLINE int
    fcgi_request_service::close(
        implementation_type& impl, ::cgi::common::http::status_code& hsc
      , int program_status, boost::system::error_code& ec)
    {
      impl.all_done_ = true;
      impl.client_.close(program_status, ec);
      impl.request_status_ = common::closed;
      return program_status;
    }

    BOOST_CGI_INLINE void
    fcgi_request_service::clear(implementation_type& impl)
    {
      /*
      BOOST_ASSERT
      (   impl.request_status_ < common::activated
       && impl.request_status_ > common::ok
       && "Are you trying to clear() a request without closing it?"
      );
      */
                
      impl.post_buffer_.clear();
      impl.param_buffer_.clear();
      common::get_vars(impl.vars_).clear();
      common::post_vars(impl.vars_).clear();
      common::cookie_vars(impl.vars_).clear();
      common::env_vars(impl.vars_).clear();
      impl.stdin_parsed_ = false;
      impl.http_status_ = common::http::no_content;
      impl.request_status_ = common::null;
      impl.request_role_ = spec_detail::ANY;
      impl.all_done_ = false;

      impl.client_.status_ = common::none_;
      impl.client_.request_id_ = -1;
    }
      
    /// Load the request to a point where it can be usefully used.
    /**
     * FastCGI:
     * --------
     *
     *  - Calls client::construct() to claim a request id from the server.
     *  - Reads, parses and handles all packets until the closing `PARAMS`
     *    packet for this request arrives from the server.
     *
     */
/*    
    /// Synchronously read/parse the request meta-data
    BOOST_CGI_INLINE boost::system::error_code
    fcgi_request_service::load(
        implementation_type& impl, common::parse_options parse_opts
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
*/
    BOOST_CGI_INLINE boost::system::error_code
    fcgi_request_service::load(
        implementation_type& impl, bool parse_stdin
      , boost::system::error_code& ec)
    {
      //int header_len( get_length_of_header(impl, ec) );
      BOOST_ASSERT(!ec && "Can't load request due to previous errors.");

      impl.client_.construct(impl, ec);
      BOOST_ASSERT(impl.client_.connection_->is_open());

      for(;;)
      {
        if (read_header(impl, ec))
          break;
        int id(fcgi::spec::get_request_id(impl.header_buf_));
        if (id == fcgi::spec::null_request_id::value)
          handle_admin_request(impl);
        else
        if (fcgi::spec::get_type(impl.header_buf_) 
            == fcgi::spec::begin_request::value)
        {
          impl.id_ = id;
          impl.client_.request_id_ = id;
          if (read_header(impl, ec))
            break;
          impl.request_role_ = fcgi::spec::begin_request::get_role(impl.header_buf_);
          //std::cerr<< "[hw] New request role: " << impl.request_role_
          //    << " (" << fcgi::spec::role_type::to_string(impl.header_buf_) << ")"
          //    << std::endl;
          impl.client_.keep_connection_
            = fcgi::spec::begin_request::get_flags(impl.header_buf_)
              & fcgi::spec::keep_connection;
          //std::cerr<< "keep connection := " << impl.client_.keep_connection_ << std::endl;
          break;
        }else
          handle_other_request_header(impl);
      }

      read_env_vars(impl, ec);

      if (parse_stdin)
      {
        while(!ec 
          && impl.client_.status() < common::stdin_read
          && impl.request_status_ != common::loaded)
        {
          parse_packet(impl, ec);
        }
      }
      const std::string& request_method = env_vars(impl.vars_)["REQUEST_METHOD"];
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

    // **FIXME**
    template<typename Handler> BOOST_CGI_INLINE
    void fcgi_request_service::async_load(
        implementation_type& impl, bool parse_stdin, Handler handler)
    {
      this->io_service().post(
        detail::async_load_helper<self_type, Handler>(this, parse_stdin, handler)
      );
    }

    BOOST_CGI_INLINE
    bool fcgi_request_service::params_read(implementation_type& impl)
    {
      return impl.client_.status() >= common::params_read;
    }

    BOOST_CGI_INLINE
    void fcgi_request_service::set_status(
        implementation_type& impl, common::request_status status)
    {
      impl.request_status_ = status;
    }

    /// Read and parse the cgi POST meta variables (greedily)
    BOOST_CGI_INLINE boost::system::error_code&
    fcgi_request_service::parse_post_vars(
        implementation_type& impl
      , boost::system::error_code& ec)
    {
      // Make sure this function hasn't already been called
      //BOOST_ASSERT( impl.post_vars().empty() );
	  
      //#     error "Not implemented"
      // **FIXME** should use form_parser, when it's ready.
      // Only works with url-encoded forms (ie. not with multipart forms)
    /*
      std::string name;
      std::string str;

      char ch;
      char ch1;
      while( impl.client_.bytes_left_ )
      {
        ch = getchar();
        --impl.client_.bytes_left_;

        switch(ch)
        {
        case '%': // unencode a hex character sequence
          if (impl.client_.bytes_left_ >= 2)
          {
            ch = getchar();
            ch1 = getchar();
            if (std::isxdigit(ch) && std::isxdigit(ch1))
            {
              str.append(1, detail::hex_to_char(ch, ch1));
            }
            else // we don't have a hex sequence
            {
              str.append(1, '%').append(1, ch).append(1, ch1);
            }
            impl.client_.bytes_left_ -= 2;
          }
          else // There aren't enough characters to make a hex sequence
          {
            str.append(1, '%');
            --impl.client_.bytes_left_;
          }
          break;
        case '+':
            str.append(1, ' ');
            break;
        case ' ': // skip spaces
            break;
        case '=': // the name is complete, now get the corresponding value
            name.swap(str);
            break;
        case '&': // we now have the name/value pair, so save it
            // **FIXME** have to have .c_str() ?
            ::cgi::common::post_vars(impl.vars_)[name.c_str()] = str;
            str.clear();
            name.clear();
           break;
        default:
            str.append(1, ch);
        }
      }
      // save the last param (it won't have a trailing &)
      if( !name.empty() )
          // **FIXME** have to have .c_str() ?
          ::cgi::common::post_vars(impl.vars_)[name.c_str()] = str;
    */

      impl.fp_.reset
      (
        new implementation_type::form_parser_type
                ( impl.post_buffer_
                , post_vars(impl.vars_)
                , env_vars(impl.vars_)["CONTENT_TYPE"]
                , callback_functor<self_type>(impl, this)
                , impl.client_.bytes_left_
                , impl.stdin_parsed_
                )
      );
      impl.fp_->parse(ec);

      return ec;
    }

    /// Read and parse a single cgi POST meta variable (greedily)
    template<typename RequestImpl> BOOST_CGI_INLINE
    boost::system::error_code&
    fcgi_request_service::parse_one_post_var(
        implementation_type& impl, boost::system::error_code& ec)
    {
      //#     error "Not implemented"
      return ec;
    }

    // **FIXME**
    /// Read some data from the client.
    template<typename MutableBufferSequence>
    BOOST_CGI_INLINE std::size_t
    fcgi_request_service::read_some(
        implementation_type& impl, const MutableBufferSequence& buf
      , boost::system::error_code& ec)
    {
      if (impl.client_.status_ == common::closed_)
      {
        ec = error::client_closed;
        return 0;
      }

      //if (read_header(ec))
        return -1;

      //boost::tribool state = parse_header(impl);
      //std::size_t bytes_read;//( connection_->read_some(buf, ec) );
      //return bytes_read;
    }

    /// Read in the FastCGI (env) params
    // **FIXME**
    BOOST_CGI_INLINE boost::system::error_code
    fcgi_request_service::read_env_vars(
        implementation_type& impl, boost::system::error_code& ec)
    {
      while(!ec && !params_read(impl))
      {
        if (this->read_header(impl, ec))
          return ec;
          
        boost::tribool state = this->parse_header(impl);

        if (state)
          // the header has been handled and all is ok; continue.
          return ec;
        else
        if (!state)
          // The header is confusing; something's wrong. Abort.
          return error::bad_header_type;

        // else => (state == boost::indeterminate)

        implementation_type::mutable_buffers_type buf
            = impl.prepare_misc(fcgi::spec::get_length(impl.header_buf_));

        if (this->read_body(impl, buf, ec))
          return ec;

        this->parse_body(impl, buf, ec);

      } // while(!ec && !params_read(impl))
      return ec;
    }

    /// Read a single header, but do nothing with it.
    BOOST_CGI_INLINE boost::system::error_code
    fcgi_request_service::read_header(
        implementation_type& impl
      , boost::system::error_code& ec)
    {
      // clear the header first (might be unneccesary).
      impl.header_buf_ = implementation_type::header_buffer_type();

      if (8 != read(*impl.client_.connection_, buffer(impl.header_buf_)
                   , boost::asio::transfer_all(), ec) || ec)
        return ec;
      
      //if (ec) return ec;

      /*
      std::cerr<< std::endl
          << "[hw] Header details {" << std::endl
          << "  RequestId := " << fcgi::spec::get_request_id(impl.header_buf_) << std::endl
          << "  FastCGI version := " << fcgi::spec::get_version(impl.header_buf_) << std::endl
          << "  Type := " << fcgi::spec::get_type(impl.header_buf_)
          << " (" << fcgi::spec::request_type::to_string(impl.header_buf_) << ")" << std::endl
          << "  Content-length := " << fcgi::spec::get_content_length(impl.header_buf_) << std::endl
          << "}" << std::endl;
      */

      return ec;
    }

    /*** Various handlers go below here; they might find a
     * better place to live ***/

    // **FIXME**
    BOOST_CGI_INLINE void
    fcgi_request_service::handle_admin_request(implementation_type& impl)
    {
      //std::cerr<< std::endl << "**FIXME** " << __FILE__ << ":" << __LINE__ 
      //  << " handle_admin_request()" << std::endl;
    }

    // **FIXME**
    BOOST_CGI_INLINE void
    fcgi_request_service::handle_other_request_header(implementation_type& impl)
    {
      //std::cerr<< std::endl << "**FIXME** " << __FILE__ << ":" << __LINE__ 
      //  << " handle_other_request_header()" << std::endl;
    }

    BOOST_CGI_INLINE boost::system::error_code
    fcgi_request_service::process_abort_request(
        implementation_type& impl, boost::uint16_t id
      , const unsigned char* buf, boost::uint16_t
      , boost::system::error_code& ec)
    {
      if (id == fcgi::spec::get_request_id(impl.header_buf_))
      {
        impl.request_status_ = common::aborted;
        return ec;
      }
      try {
        //std::cerr<< "**FIXME** request aborted (id = " << id
        //  << ") but request not notified." << std::endl;
        //impl.client_.connection_->requests_.at(id - 1)->abort();
      }catch(...){
        ec = error::abort_request_record_recieved_for_invalid_request;
      }
/*
      connection_type::request_map_type::iterator i
        = connection_->find(id);

      if (i == connection_type::request_map_type::iterator())
      {
        return bad_request_id;
      }

      //lookup_request(id).abort();
*/
      return ec;
    }

    BOOST_CGI_INLINE boost::system::error_code
    fcgi_request_service::process_params(
        implementation_type& impl, boost::uint16_t id
      , const unsigned char* buf, boost::uint16_t len
      , boost::system::error_code& ec)
    {
      if (0 == len)
      { // This is the final param record.
        
        impl.client_.status_ = common::params_read;

        //std::cerr<< "[hw] Final PARAM record found." << std::endl;
        return ec;
      }

      while(len)
      {
        boost::uint32_t   name_len, data_len;
        std::string name, data;
        if (*buf >> 7 == 0)
        {
            name_len = *(buf++);
            --len;
        }
        else
        {
            name_len = ((buf[0] & 0x7F) << 24)
                     +  (buf[1]         << 16)
                     +  (buf[2]         <<  8)
                     +   buf[3];
            buf += 4;
            len -= 4;
        }

        if (*buf >> 7 == 0)
        {
            data_len = *(buf++);
            --len;
        }
        else
        {
            data_len = ((buf[0] & 0x7F) << 24)
                     +  (buf[1]         << 16)
                     +  (buf[2]         <<  8)
                     +   buf[3];
            buf += 4;
            len -= 4;
        }
        name.assign(reinterpret_cast<const char*>(buf), name_len);
        data.assign(reinterpret_cast<const char*>(buf)+name_len, data_len);
        buf += (name_len + data_len);
        len -= (name_len + data_len);

        //std::cerr<< "[hw] name := " << name << std::endl;
        //std::cerr<< "[hw] data := " << data << std::endl;

        // **FIXME**
        env_vars(impl.vars_)[name.c_str()] = data;
      }

      return ec;
    }


    BOOST_CGI_INLINE boost::system::error_code
    fcgi_request_service::process_stdin(
        implementation_type& impl, boost::uint16_t id
      , const unsigned char* buf, boost::uint16_t len
      , boost::system::error_code& ec)
    {
      if (0 == len)
      {
        impl.client_.status_ = common::stdin_read;

        // **FIXME**
        //std::cerr<< "[hw] Final STDIN record found." << std::endl;
        return ec;
      }

      // **FIXME**
      //std::cerr<< "[hw] Found some STDIN stuff." << std::endl;
      return ec;
    }

    /// Parse the current header
    BOOST_CGI_INLINE boost::tribool
    fcgi_request_service::parse_header(implementation_type& impl)
    {
      BOOST_ASSERT(fcgi::spec::get_version(impl.header_buf_) == 1
                   && "This library is only compatible with FastCGI 1.0");

      using namespace fcgi::spec_detail;

      switch(fcgi::spec::get_type(impl.header_buf_))
      {
      case BEGIN_REQUEST:
      case PARAMS:
      case STDIN:
      case DATA:
      case GET_VALUES:
        return boost::indeterminate;
      case ABORT_REQUEST:
        return false;
      case UNKNOWN_TYPE:
      default:
        return true;
      }
    }

    BOOST_CGI_INLINE boost::system::error_code
    fcgi_request_service::parse_packet(
        implementation_type& impl, boost::system::error_code& ec)
    {
      //current_request_ = &req;

      if (this->read_header(impl, ec))
        return ec;

      boost::tribool state = this->parse_header(impl);

      if (state)
      { // the header has been handled and all is ok; continue.
        return ec;
      }else
      if (!state)
      { // The header is confusing; something's wrong. Abort.
        return error::bad_header_type;
      }
      // else route (ie. state == boost::indeterminate)

      implementation_type::mutable_buffers_type buf
        = impl.prepare(fcgi::spec::get_length(impl.header_buf_));

      if (this->read_body(impl, buf, ec))
        return ec;

      this->parse_body(impl, buf, ec);

      return ec;
    }

    /// Read the body of the current packet; do nothing with it.
    template<typename MutableBuffersType> BOOST_CGI_INLINE
    boost::system::error_code
    fcgi_request_service::read_body(
        implementation_type& impl, const MutableBuffersType& buf
      , boost::system::error_code& ec)
    {
      std::size_t bytes_read
        = read(*impl.client_.connection_, buf, boost::asio::transfer_all(), ec);

      BOOST_ASSERT(bytes_read == fcgi::spec::get_length(impl.header_buf_)
                   && "Couldn't read all of the record body.");
      return ec;
    }

    template<typename MutableBuffersType> BOOST_CGI_INLINE
    boost::system::error_code
    fcgi_request_service::parse_body(
        implementation_type& impl, const MutableBuffersType& buf
      , boost::system::error_code& ec)
    {
      return
        (this->* proc_funcs[fcgi::spec::get_type(impl.header_buf_)])
            (impl, fcgi::spec::get_request_id(impl.header_buf_)
            , boost::asio::buffer_cast<unsigned char*>(buf)
            , boost::asio::buffer_size(buf), ec);
    }


    BOOST_CGI_INLINE boost::system::error_code
    fcgi_request_service::begin_request_helper(
        implementation_type& impl
      , implementation_type::header_buffer_type& header
      , boost::system::error_code& ec)
    {
       impl.client_.request_id_ = fcgi::spec::get_request_id(header);

       BOOST_STATIC_ASSERT((
        fcgi::spec::begin_request::body::size::value
        == fcgi::spec::header_length::value));
       
       // A begin request body is as long as a header, so we can optimise:
       if (read_header(impl, ec))
         return ec;
        
       impl.request_role_
         = fcgi::spec::begin_request::get_role(impl.header_buf_);
       // **FIXME** (rm impl.request_role_)
       impl.client_.role_ = impl.request_role_;
       impl.client_.keep_connection_
         = fcgi::spec::begin_request::get_flags(impl.header_buf_)
           & fcgi::spec::keep_connection;
       impl.client_.status_ = common::constructed;
       
       return ec;
    }

  //template<>
  const fcgi_request_service::proc_func_t fcgi_request_service::proc_funcs[] =
    { 0
    , &fcgi_request_service::process_begin_request
    , &fcgi_request_service::process_abort_request
    , 0
    , &fcgi_request_service::process_params
    , &fcgi_request_service::process_stdin
    , 0
    , 0
    };

 } // namespace fcgi
} // namespace cgi

#include "boost/cgi/fcgi/request.hpp"
#include "boost/cgi/basic_request.hpp"

namespace cgi {
 namespace fcgi {

/*
    fdetail::request_type&
      get_or_make_request(implementation_type& impl, boost::uint16_t id)
    {
      implementation_type::client_type::connection_type::request_vector_type&
        requests = impl.client_.connection_->requests_;

      if (!requests.at(id-1))
      {
        if (requests.size() < (id-1))
          requests.resize(id);
        requests.at(id-1) = fdetail::request_type::create(*impl.service_);
      }

      return *requests.at(id-1);
    }
*/

    BOOST_CGI_INLINE boost::system::error_code
    fcgi_request_service::process_begin_request(
        implementation_type& impl, boost::uint16_t id
      , const unsigned char* buf, boost::uint16_t
      , boost::system::error_code& ec)
    {
      if (impl.client_.request_id_ == 0) // ie. hasn't been set yet.
      {
        begin_request_helper(impl, impl.header_buf_, ec);
      }
      else
      {
        //std::cerr<< "**FIXME** Role: " 
        //  << fcgi::spec::begin_request::get_role(impl.header_buf_) << std::endl;

        implementation_type::client_type::connection_type&
          conn = *impl.client_.connection_;

        if (conn.get_slot(id, ec))
        { // error
          return ec;
        }

        // **FIXME** THIS LEAKS MEMORY!!!!!!!
        //requests.at(id-1)
        request_type* new_request = new request_type(impl, ec);

        conn.add_request(id, new_request, true, ec);

        return ec;//error::multiplexed_request_incoming;
      }

      return ec;
    }


 } // namespace fcgi
} // namespace cgi

#endif // CGI_FCGI_REQUEST_SERVICE_IPP_INCLUDED__

