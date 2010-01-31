//                 -- server3/main.hpp --
//
//           Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
//
//[fcgi_server3
//
// This example simply echoes all variables back to the user. ie.
// the environment and the parsed GET, POST and cookie variables.
// Note that GET and cookie variables come from the environment
// variables QUERY_STRING and HTTP_COOKIE respectively.
//
// It is a demonstration of how a 'server' can be used to abstract
// away the differences between FastCGI and CGI requests.
//
// This is very similar to the fcgi_echo example.
//

#include <fstream>
///////////////////////////////////////////////////////////
#include <boost/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/program_options/environment_iterator.hpp>
///////////////////////////////////////////////////////////
#include "boost/cgi/fcgi.hpp"

using namespace std;
using namespace boost;
using namespace boost::fcgi;

/// Handle one request and return.
/**
 * If it returns != 0 then an error has occurred. Sets ec to the error_code
 * corresponding to the error.
 */
int handle_request(fcgi::request& req, boost::system::error_code& ec)
  {
    // Construct a `response` object (makes writing/sending responses easier).
    response resp;

    // Responses in CGI programs require at least a 'Content-type' header. The
    // library provides helpers for several common headers:
    resp<< content_type("text/plain")
    // You can also stream text to a response object. 
        << "Hello there, universe!";

    //log_<< "Handled request, handling another." << std::endl;

    return commit(req, resp);
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
  typedef fcgi::request                         request_type;
  typedef boost::function<
            int ( request_type&
                , boost::system::error_code&)
          >                                     function_type;

  server(const function_type& handler)
    : handler_(handler)
    , service_()
    , acceptor_(service_)
  {}

  int run()
  {
    // Create a new request (on the heap - uses boost::shared_ptr<>).
    request_type::pointer new_request = request_type::create(service_);
    // Add the request to the set of existing requests.
    requests_.insert(new_request);
    
    int ret(0);
    for (;;)
    {
      boost::system::error_code ec;

      acceptor_.accept(*new_request, ec);

      if (ec) 
      {
        std::cerr<< "Error accepting: " << ec.message() << std::endl;
        return 5;
      }
  
      // Load in the request data so we can access it easily.
      new_request->load(parse_all, ec); // Read and parse POST data and cookies.

      ret = handler_(*new_request, ec);

      if (ret)
        break;
    }
    return ret;
  }

private:
  function_type handler_;
  fcgi::service service_;
  fcgi::acceptor acceptor_;
  std::set<request_type::pointer> requests_;
};

int main()
try
{
  server s(&handle_request);

  // Run the server in ten threads.
  // => Handle ten requests simultaneously
  boost::thread_group tgroup;
  for(int i(10); i != 0; --i)
  {
    tgroup.create_thread(boost::bind(&server::run, &s));
  }
  tgroup.join_all();
  
}catch(boost::system::system_error& se){
  cerr<< "[fcgi] System error: " << se.what() << endl;
  return 1313;
}catch(std::exception* e){
  cerr<< "[fcgi] Exception: " << e->what() << endl;
  return 666;
}catch(...){
  cerr<< "[fcgi] Uncaught exception!" << endl;
  return 667;
}
//]
