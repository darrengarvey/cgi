//            -- basic_protocol_service.hpp --
//
//            Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_BASIC_PROTOCOL_SERVICE_HPP_INCLUDED__
#define CGI_BASIC_PROTOCOL_SERVICE_HPP_INCLUDED__

#include <set>
#include <queue>
#include <boost/shared_ptr.hpp>
#include <boost/asio/strand.hpp>
#include <boost/detail/workaround.hpp>

#include "boost/cgi/io_service.hpp"
#include "boost/cgi/io_service_provider.hpp"
#include "boost/cgi/basic_request_fwd.hpp"
#include "boost/cgi/detail/protocol_traits.hpp"

namespace cgi {

  /// Basic Protocol Service
  /**
   * Holds the request queue and the connection queue.
   * It is also a wrapper around asio::io_service
   */
  template<typename Protocol, typename IoServiceProvider>
  class basic_protocol_service
    //: public protocol_traits<Protocol> // do this!
  {
  public:
    typedef Protocol                                         protocol_type;
    typedef IoServiceProvider                                ios_provider_type;
    typedef typename detail::protocol_traits<Protocol>::type traits;
    typedef typename traits::request_type                    request_type;
    typedef typename boost::shared_ptr<request_type>         request_ptr;
    typedef std::set<request_ptr>                            set_type;
      //typename boost::mpl::if_<
      //  boost::is_same<protocol_type, tags::acgi>::value
      //  , request_type::pointer
      //  , std::set<request_type::pointer>
      //  >::type

    typedef std::queue<request_ptr>                          queue_type;

    basic_protocol_service(int pool_size_hint = 1)
      : ios_provider_(pool_size_hint)
                          //, strand_(ios_provider_.io_service())
                          //, gateway_(*this)
    {
    }

    basic_protocol_service(boost::asio::io_service& ios)
      : ios_provider_(ios)
                          //, strand_(ios)
                          //, gateway_(*this)
    {
    }

    ~basic_protocol_service()
    {
      //gateway_.stop();
    }

    /// Run all the io_services contained by this service
    /**
     * This is equivalent to calling run() on each of the io_services held by
     * ios_provider_
     */
    void run()
    {
      ios_provider_.run();
    }

    /// Stop all the io_services contained by this service
    /**
     * This is equivalent to calling stop() on each of the io_services held by
     * ios_provider_
     */
    void stop()
    {
      //gateway_.stop();
      ios_provider_.stop();
    }

    /// Reset all the io_services contained by this service
    /**
     * This deletes the request queue(s), aborts all running requests and then
     * calls reset() on each of the io_services held by ios_provider_. There is
     * no guarantee that requests will terminate immediately.
     */
    void reset()
    {
      request_queue_.clear();
      //std::for_each(request_set_.begin(), request_set_.end()
      //              , boost::bind(&request_type::abort, boost::arg<1>()));
      request_set_.clear();
      ios_provider_.reset();
    }

    /// Return an available io_service from the IoServiceProvider
    /**
     * The order in which the underlying io_services are returned is determined
     * by what policy the IoServiceProvider uses.
     */
    ::cgi::io_service& io_service()
    {
      return ios_provider_.get_io_service();
    }

    /// Post the handler through an available io_service
    template<typename Handler>
    void post(Handler handler)
    {
      ios_provider_.get_io_service().post(handler);
    }

    /// Dispatch a handler through an available io_service
    template<typename Handler>
    void dispatch(Handler handler)
    {
      ios_provider_.get_io_service().dispatch(handler);
    }

  private:
    ios_provider_type ios_provider_;

    /// A strand is used for guaranteeing handlers are dispatched sequentially
    //boost::asio::strand strand_;

    set_type request_set_;
    queue_type request_queue_;

    //gateway_type gateway_;

    //friend class basic_gateway<protocol_type>;//gateway_type;
    //friend class basic_acceptor<protocol_type>;//class acceptor_type;
#if BOOST_WORKAROUND(BOOST_MSVC, BOOST_TESTED_AT(1400))
    friend typename traits::request_type;//typename request_type;
#else
    friend class traits::request_type;
#endif
    //friend class request_type;
  };

} // namespace cgi

#endif // CGI_BASIC_PROTOCOL_SERVICE_HPP_INCLUDED__
