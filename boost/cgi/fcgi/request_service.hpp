//             -- fcgi/request_service.hpp --
//
//            Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_FCGI_REQUEST_SERVICE_HPP_INCLUDED__
#define CGI_FCGI_REQUEST_SERVICE_HPP_INCLUDED__

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
#include "boost/cgi/common/source_enums.hpp"
#include "boost/cgi/common/request_base.hpp"
#include "boost/cgi/detail/service_base.hpp"
#include "boost/cgi/common/form_parser.hpp"

namespace cgi {
 namespace fcgi {
 
  /// The IoObjectService class for a FCGI basic_request<>s
  class fcgi_request_service
    : public detail::service_base<fcgi_request_service>
    , public common::request_base<fcgi_request_service>
  {
  public:
    /// The actual implementation date for an FCGI request.
    struct implementation_type
      : common::request_base<fcgi_request_service>::impl_base
    {
      typedef ::cgi::common::fcgi_              protocol_type;
      typedef ::cgi::fcgi::client               client_type;
      typedef client_type::connection_type      connection_type;
      typedef client_type::header_buffer_type   header_buffer_type;
      typedef detail::protocol_traits<
        protocol_type
      >::protocol_service_type                  protocol_service_type;
      typedef detail::protocol_traits<
        protocol_type
      >::request_type                           request_type;

      implementation_type()
        : client_()
        , stdin_parsed_(false)
        , http_status_(::cgi::common::http::no_content)
        , request_status_(common::unloaded)
        , request_role_(spec_detail::ANY)
        , all_done_(false)
      {
      }

      protocol_service_type* service_;

      client_type client_;

      bool stdin_parsed_;
      ::cgi::common::http::status_code http_status_;
      common::status_type request_status_;
      fcgi::spec_detail::role_t request_role_;

      bool all_done_;

      mutable_buffers_type prepare(std::size_t size)
      {
        using namespace std;
        std::size_t bufsz( buffer_.size() );
        //cerr<< "bufsz    = " << bufsz << endl;

        // Reserve more space if it's needed.
        // (this could be safer, referencing this against CONTENT_LENGTH)
        //if ( (bufsz + size) >= buf_.capacity() )
        //{
          buffer_.resize(bufsz + size);
        //}
        return boost::asio::buffer(&buffer_[bufsz], size);
      }


     /************** New stuff *****************/
      header_buffer_type header_buf_;
      boost::uint16_t id_;
      typedef detail::form_parser<implementation_type> form_parser_type;

      boost::scoped_ptr<form_parser_type> fp_;
     };

    typedef fcgi_request_service                      type;
    typedef ::cgi::fcgi::fcgi_request_service         full_type;
    typedef type::implementation_type::protocol_type  protocol_type;
    typedef type::implementation_type::request_type   request_type;

    fcgi_request_service(::cgi::common::io_service& ios)
      : detail::service_base<fcgi_request_service>(ios)
    {
    }

    ~fcgi_request_service()
    {
    }

    void construct(implementation_type& impl)
    {
      impl.client_.set_connection(//new implementation_type::connection_type(this->io_service()));
        implementation_type::connection_type::create(this->io_service())
      );
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

    void set_service(implementation_type& impl
                    , implementation_type::protocol_service_type& ps)
    {
      impl.service_ = &ps;
    }

    bool is_open(implementation_type& impl)
    {
      return !impl.all_done_ && impl.client_.is_open();
    }

    /// Close the request.
    int close(implementation_type& impl, ::cgi::common::http::status_code& hsc
              , int program_status);

    int close(implementation_type& impl, ::cgi::common::http::status_code& hsc
             , int program_status, boost::system::error_code& ec);

    void clear(implementation_type& impl);

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
    boost::system::error_code&
      load(implementation_type& impl, bool parse_stdin
          , boost::system::error_code& ec);

    // **FIXME**
    template<typename Handler>
    void async_load(implementation_type& impl, bool parse_stdin, Handler handler);

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
    /// Read and parse the cgi POST meta variables (greedily)
    boost::system::error_code&
    parse_post_vars(implementation_type& impl, boost::system::error_code& ec);

    /// Read and parse a single cgi POST meta variable (greedily)
    template<typename RequestImpl>
    boost::system::error_code&
    parse_one_post_var(implementation_type& impl, boost::system::error_code& ec);

  /***************************************************************************/
  public: // Reading stuff goes under here

    // **FIXME**
    /// Read some data from the client.
    template<typename MutableBufferSequence>
    std::size_t
      read_some(implementation_type& impl, const MutableBufferSequence& buf
               , boost::system::error_code& ec);
      
    /// Read a single header, buf do nothing with it.
    boost::system::error_code
      read_header(implementation_type& impl, boost::system::error_code& ec);

    /*** Various handlers go below here; they might find a
     * better place to live ***/

    // **FIXME**
    void handle_admin_request(implementation_type& impl);

    // **FIXME**
    void handle_other_request_header(implementation_type& impl);

    // **FIXME**    
    boost::system::error_code
      process_begin_request(implementation_type& impl, boost::uint16_t id
                           , const unsigned char* buf, boost::uint16_t
                           , boost::system::error_code& ec);

    boost::system::error_code
      process_abort_request(implementation_type& impl, boost::uint16_t id
                           , const unsigned char* buf, boost::uint16_t
                           , boost::system::error_code& ec);

    boost::system::error_code
      process_params(implementation_type& impl, boost::uint16_t id
                    , const unsigned char* buf, boost::uint16_t len
                    , boost::system::error_code& ec);

    boost::system::error_code
      process_stdin(implementation_type& impl, boost::uint16_t id
                   , const unsigned char* buf, boost::uint16_t len
                   , boost::system::error_code& ec);

    /// Parse the current header
    boost::tribool parse_header(implementation_type& impl);

    // Mammoth typedef corresponding to function signature of process_*
    // functions.
    typedef boost::system::error_code
      ( full_type::* proc_func_t)
      (implementation_type& impl, boost::uint16_t, const unsigned char*
      , boost::uint16_t, boost::system::error_code&);

    static const proc_func_t proc_funcs[];
  
    boost::system::error_code
      parse_packet(implementation_type& impl, boost::system::error_code& ec);
      
    /// Read the body of the current packet; do nothing with it.
    template<typename MutableBuffersType>
    boost::system::error_code
      read_body(implementation_type& impl, const MutableBuffersType& buffer
               , boost::system::error_code& ec);

    template<typename MutableBuffersType>
    boost::system::error_code
      parse_body(implementation_type& impl, const MutableBuffersType& buffer
                , boost::system::error_code& ec);

    boost::system::error_code
      begin_request_helper(implementation_type& impl
                          , implementation_type::header_buffer_type& header
                          , boost::system::error_code& ec);
  };

 } // namespace fcgi
} // namespace cgi

#include "boost/cgi/fcgi/request.hpp"
#include "boost/cgi/basic_request.hpp"

#if !defined( BOOST_CGI_BUILD_LIB )
#    include "boost/cgi/impl/fcgi_request_service.ipp"
#endif

#endif // CGI_FCGI_REQUEST_SERVICE_HPP_INCLUDED__

