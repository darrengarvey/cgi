#pragma once

#include <iostream>
#include <boost/function.hpp>
#include <boost/cgi/cgi.hpp>
#undef min
#undef max
#include <boost/chrono/process_times.hpp>

//#include <boost/chrono/chrono.hpp>

/// A server which catches some runtime errors in your app.
/**
 * This server traces each request and dumps helpful info to a browser if
 * an error occurs. The errors which are acknowledged at the moment are
 * exceptions and your handler returning non-zero. You can fake an error
 * by appending 'debug=1' to the passed form variables.
 *
 * When an error is caught, details about that, the request that caused it and
 * the response (along with headers) are dumped to the client in a browser-
 * friendly way, 
 *
 */
class TracingServer
{
    typedef boost::cgi::request request_type;
    typedef boost::cgi::response response_type;
    typedef boost::function<int (request_type&, response_type&)> callback_type;
    //typedef Timer<boost::chrono::high_resolution_clock> timer_type;
    typedef boost::chrono::process_timer timer_type;

    timer_type timer_;
public:
    TracingServer(void);
    ~TracingServer(void);

    bool run(callback_type const& callback);

    void bomb_out(std::string const& error, response_type&, request_type&);
    void bomb_out(boost::system::system_error* err, response_type&, request_type&);
    void bomb_out(std::exception& e, response_type&, request_type&);
};
