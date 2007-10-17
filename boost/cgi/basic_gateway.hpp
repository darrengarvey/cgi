//                 -- basic_gateway.hpp --
//
//            Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_BASIC_GATEWAY_HPP_INCLUDED__
#define CGI_BASIC_GATEWAY_HPP_INCLUDED__

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>

#include "basic_protocol_service_fwd.hpp"
//#include "basic_connection.hpp"
#include "detail/protocol_traits.hpp"

namespace cgi {

  /// The gateway class manages connections
  /**
   * New connections are accepted through here, via a call to accept();
   * used/corrupted connections are closed via a call to stop(conn_ptr);
   * all connections are closed via a call to stop_all().
   *
   * If you want to use the gateway after a call to stop_all(), you must
   * call reset() and pass it the maximum number of connections the gateway
   * can take.
   */
  template<typename Protocol>
  class basic_gateway
  {
  public:
    //typedef CommonGatewayService                            service_type;
    //typedef typename CommonGatewayService::protocol_type    protocol_type;
    typedef Protocol                                     protocol_type;
    typedef basic_protocol_service<protocol_type>        protocol_service_type;

    typedef detail::protocol_traits<protocol_type>       traits;
    typedef typename traits::connection_type             connection_type;
    typedef typename traits::gateway_impl_type           impl_type;
    typedef typename traits::gateway_service_type        service_type;
    typedef boost::shared_ptr<connection_type>           conn_ptr;

    /// Constructors
    explicit basic_gateway(protocol_service_type& pservice)
      : service_(pservice)
    {
      service_.construct(impl_);
    }

    explicit basic_gateway()
    {
      service_.construct(impl_);
    }

    /// Destructor
    ~basic_gateway()
    {
      service_.destroy(impl_);
    }

    /// Start a sychronous accept
    /**
     * This returns false unless the connection is already established. This is
     * true with standard CGI, for example, where the connection is simply a
     * wrapper over standard input/output.
     *
     * If there is a need to differentiate between a fail and a 'waiting' accept
     * then a tribool could be returned from here.
     */
/* ** NOT BELONGING HERE ** (should be encapsulated in the acceptor classes)
    template<typename Request>
    boost::system::error_code&
    accept(Request& request, boost::system::error_code& ec)
    {
      if( service_.accept(request.connection(), ec) )
      {
        connections_.insert(request.connection());
      }

      return ec;

      //conn_ptr new_conn(connection_type::create());
      //acceptor_.accept(new_conn->socket()
      //                , boost::bind(&cgi::gateway::handle_accept
      //                             , this, new_conn
      //                             , boost::placeholders::error));
      //return false;
    }

    /// Start an asynchronous accept
    template<typename Handler>
    void async_accept(conn_ptr conn, Handler handler)
    {
      service_.async_accept(handler);
    }
*/
    /// Cleanly start the connection
    void start(conn_ptr cptr)
    {
      service_.start(impl_, cptr);
      //cptr->start();
      //protocol_service_.connections_.insert(cptr);
    }

    /// Cleanly stop the connection
    void stop(conn_ptr cptr)
    {
      //service_.stop(impl_, cptr);
      //BOOST_ASSERT(protocol_service_.connections_.find(cptr));

      //cptr->stop();
      //protocol_service_.connections_.erase(cptr);
    }

    /// Cleanly stop all connections
    void stop()
    {
      service_.stop(impl_);
      //std::for_each(protocol_service_.connection_.begin()
      //             , protocol_service_.connections_.end()
      //             , boost::bind(&connection_type::stop, _1));
      //protocol_service_.connections_.clear();
    }

    /// Reset the gateway
    /**
     * All connections are gracefully closed and then the gateway is set up for
     * reuse.
     *
     * @param max_connections the available slots is reset to this value
     */
    void reset(int max_connections)
    {
      service_.reset(impl_);
      //stop();
      //available_slots_ = max_connections;
    }

  private:
    //protocol_service_type& protocol_service_;
    service_type service_;
    impl_type impl_;
    //std::set<conn_ptr> connections_;
  };



  //template<>
  //class gateway<cgi_service>
  //{
  //public:
	 // gateway()
	 // {
	 // }
  //private:
	 // 
  //};

    /*
  template<>
  class gateway::acceptor<tags::fastcgi>
  {
  public:
	  gateway::acceptor
	  {
	  }
  private:
	  boost::asio::ip::tcp::acceptor<
  };
    */

} // namespace cgi

#endif // CGI_BASIC_GATEWAY_HPP_INCLUDED__

/*
 * Notes:
 *
 * 1. A FastCGI server doesn't have to use tcp sockets: it can use pipes
 * instead. Support for these will only come in when Boost.Asio supports them.
 *
 * 2. For now each request object could hold a pointer/reference to the
 * connection that it's associated with. For the forseable future that's going
 * to be enough. However, since the FastCGI spec doesn't say otherwise,
 * eventually it could happen that the response can be sent back via ANY open
 * connection. In that case, the request would have to query the gateway to
 * find an available connection before sending through it. For now, that's
 * unneccesary.
 */
