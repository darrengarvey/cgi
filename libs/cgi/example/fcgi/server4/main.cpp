//                 -- server4/main.hpp --
//
//           Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
//
//[fcgi_server4
//
// This example simply echoes all variables back to the user. ie.
// the environment and the parsed GET, POST and cookie variables.
// Note that GET and cookie variables come from the environment
// variables QUERY_STRING and HTTP_COOKIE respectively.
//
// It is a demonstration of how a 'server' can be used to abstract
// away the differences between FastCGI and CGI requests.
//
// This is very similar to the fcgi_echo and fcgi_server1 examples.
// Unlike in the server1 example, the server class in this example uses
// asynchronous functions, to increase throughput.
//

#include <fstream>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/program_options/environment_iterator.hpp>

#include <boost/cgi/fcgi.hpp>

using namespace std;
using namespace boost::fcgi;

/// Handle one request and return.
/**
 * If it returns != 0 then an error has occurred. Sets ec to the error_code
 * corresponding to the error.
 */
int handle_request(fcgi::request& req, fcgi::response& resp
                  , boost::system::error_code& ec)
{
  // Responses in CGI programs require at least a 'Content-type' header. The
  // library provides helpers for several common headers:
  resp<< content_type("text/html")
  // You can also stream text to a response object. 
      << "Hello there, universe!<p />";

  // Use the function defined above to show some of the request data.
  //format_map(req.env(), resp, "Environment Variables");
  //format_map(req.GET(), resp, "GET Variables");
  //format_map(req.cookie(), resp, "Cookie Variables");

  // Response headers can be added at any time before send/flushing it:
  resp<< "content-length == " << content_length(resp.content_length())
      << content_length(resp.content_length());

  //log_<< "Handled request, handling another." << std::endl;
  //
  return 0;
}

/// The server is used to abstract away protocol-specific setup of requests.
/**
 * This server only works with FastCGI, but as you can see in the
 * request_handler::handle_request() function above, the request in there could
 * just as easily be a cgi::request.
 *
 * Later examples will demonstrate making protocol-independent servers.
 * (**FIXME**)
 */
class server
{
public:
  typedef fcgi::service                         service_type;
  typedef fcgi::acceptor                        acceptor_type;
  typedef fcgi::request                         request_type;
  typedef fcgi::response                        response_type;
  typedef boost::function<
            int ( request_type&, response_type&
                , boost::system::error_code&)
          >                                     function_type;

  server(const function_type& handler)
    : handler_(handler)
    , service_()
    , acceptor_(service_)
  {}

  void run(int num_threads = 1)
  {
    // Create a new request (on the heap - uses boost::shared_ptr<>).
    request_type::pointer new_request = request_type::create(service_);
    // Add the request to the set of existing requests.
    requests_.insert(new_request);

    start_accept(new_request);

    // Run all asynchronous functions in `num_threads` threads.
    boost::thread_group tgroup;
    for(int i(num_threads); i != 0; --i)
    {
      tgroup.create_thread(boost::bind(&service_type::run, &service_));
    }
    // Make sure we don't leave this function until all threads have exited.
    tgroup.join_all();
  }

  void start_accept(request_type::pointer& new_request)
  {
    acceptor_.async_accept(*new_request, boost::bind(&server::handle_accept
                                                    , this, new_request
                                                    , boost::asio::placeholders::error)
                          );
  }

  void handle_accept(request_type::pointer req  
                    , boost::system::error_code ec)
  {
    if (ec)
    {
      //std::cerr<< "Error accepting request: " << ec.message() << std::endl;
      return;
    }

    req->load(ec, true);

    boost::shared_ptr<response_type> new_response(new response_type);
    responses_.insert(new_response);

    //req->async_load(boost::bind(&server::handle_request, this
    //                           , req, boost::asio::placeholders::error)
    //               , true);

    service_.post(boost::bind(&server::handle_request, this
                             , req, new_response, ec)
                 );

    // Create a new request (on the heap - uses boost::shared_ptr<>).
    request_type::pointer new_request = request_type::create(service_);
    // Add the request to the set of existing requests.
    requests_.insert(new_request);

    start_accept(new_request);
  }

  void handle_request(request_type::pointer req
                     , boost::shared_ptr<response_type> resp
                     , boost::system::error_code ec)
  {
    int program_status( handler_(*req, *resp, ec) );
    if (ec
     || resp->send(*req, ec)
     || req->close(resp->status(), program_status, ec))
    {
      std::cerr<< "Couldn't send response/close request." << std::endl;
    }
    else
      start_accept(req);
  }

private:
  function_type handler_;
  service_type service_;
  acceptor_type acceptor_;
  std::set<request_type::pointer> requests_;
  std::set<boost::shared_ptr<response_type> > responses_;
};

int main()
try
{
  server s(&handle_request);

  // Run the server.
  s.run();

  return 0;
  
}catch(boost::system::system_error& se){
  cerr<< "[fcgi] System error: " << se.what() << endl;
  return 1313;
}catch(exception& e){
  cerr<< "[fcgi] Exception: " << e.what() << endl;
  return 666;
}catch(...){
  cerr<< "[fcgi] Uncaught exception!" << endl;
  return 667;
}
//]
