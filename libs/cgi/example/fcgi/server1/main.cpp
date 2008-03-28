//                 -- server1/main.hpp --
//
//           Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
//
//[fcgi_server1
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
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/program_options/environment_iterator.hpp>

#include <boost/cgi/fcgi.hpp>

using namespace std;
using namespace boost::fcgi;

// This is a file to put internal logging info into
#define LOG_FILE "/var/www/log/fcgi_server1.txt"

// This function writes the title and map contents to the ostream in an
// HTML-encoded format (to make them easier on the eye).
template<typename Map, typename OStream>
void format_map(Map& m, OStream& os, const std::string& title)
{
  os<< "<h2>" << title << "</h2>";
  if (m.empty()) os<< "NONE<br />";
  for (typename Map::const_iterator i = m.begin(), end = m.end()
      ; i != end; ++i)
  {
    os<< "<b>" << i->first << "</b> = <i>" << i->second << "</i><br />";
  }
}

/// The handle_request member function is used to handle requests.
/**
 * A struct is used here just so a single log file can be shared between
 * requests. Note that access to the log file isn't synchronised (this doesn't
 * matter with this example).
 */
struct request_handler
{
  request_handler(const std::string& file_name)
    : log_(file_name.c_str())
  {
    if (!log_)
    {
      std::cerr<< "[fcgi] Couldn't open file: \"" LOG_FILE "\"." << endl;
      throw std::runtime_error("Couldn't open log file");
    }

    log_<< boost::posix_time::second_clock::local_time() << endl;
  }
  
  int handle_request(fcgi::request& req, boost::system::error_code& ec)
  {
    std::ofstream log_(LOG_FILE, std::ios::app);
    log_<< "Handling request" << endl
        << "QUERY_STRING := " << req.query_string() << std::endl;

    // Construct a `response` object (makes writing/sending responses easier).
    response resp;

    // Responses in CGI programs require at least a 'Content-type' header. The
    // library provides helpers for several common headers:
    resp<< content_type("text/html")
    // You can also stream text to a response object. 
        << "Hello there, universe!<p />";

    // Use the function defined above to show some of the request data.
    format_map(req.env(), resp, "Environment Variables");
    format_map(req.GET(), resp, "GET Variables");
    format_map(req.cookie(), resp, "Cookie Variables");

    // Response headers can be added at any time before send/flushing it:
    resp<< "<content-length == " << content_length(resp.content_length())
        << content_length(resp.content_length());

    log_<< "Handled request, handling another." << std::endl;

    // This funky macro finishes up:
    return_(resp, req, 0);
    // It is equivalent to the below, where the third argument is represented by
    // `program_status`:
    //
    // resp.send(req.client());
    // req.close(resp.status(), program_status);
    // return program_status;
    //
    // Note: in this case `program_status == 0`.
  }

private:
  std::ofstream log_;
};


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
      new_request->load(ec, true); // The 'true' means read and parse POST data.

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
  request_handler rh(LOG_FILE);

  server s(boost::bind(&request_handler::handle_request
                      , &rh, _1, _2)
          );

  return s.run();
  
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
