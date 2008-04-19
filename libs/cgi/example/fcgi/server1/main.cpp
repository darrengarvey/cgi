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

#include <boost/cgi/fcgi.hpp>
#include "server.hpp"

using namespace std;
using namespace boost::fcgi;

// This function writes the title and map contents to the ostream in an
// HTML-encoded format (to make them easier on the eye).
template<typename OStream, typename Map>
void format_map(OStream& os, Map& m, const std::string& title)
{
  os<< "<h2>" << title << "</h2>";
  if (m.empty()) os<< "NONE<br />";
  for (typename Map::const_iterator i = m.begin(), end = m.end()
      ; i != end; ++i)
  {
    os<< "<b>" << i->first << "</b> = <i>" << i->second << "</i><br />";
  }
}

/// The handle_request function handles a single request.
int handle_request(fcgi::request& req, boost::system::error_code& ec)
{
  // Construct a `response` object (makes writing/sending responses easier).
  response resp;

  // Responses in CGI programs require at least a 'Content-type' header. The
  // library provides helpers for several common headers:
  resp<< content_type("text/html")
      // You can also stream text to a response object.
      << "Hello there, universe!<p />";

  // Use the function defined above to show some of the request data.
  format_map(resp, req[env_data],    "Environment Variables");
  format_map(resp, req[get_data],    "GET Variables");
  format_map(resp, req[cookie_data], "Cookie Variables");
   // Response headers can be added at any time before send/flushing it:
  resp<< "<h3>Response Length</h3>" << resp.content_length()
      // response::content_length() returns the length of the *body*
      // of the response (ie. not including the headers).
      << content_length(resp.content_length());

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

///////////////////////////////////////////////////////////
int main()
///////////////////////////////////////////////////////////
try
{
  server4 s(&handle_request);

  return s.run();

}
catch(boost::system::system_error& se){
  cerr<< "[fcgi] System error (" << se.code() << "): "
      << se.what() << endl;
  return 1313;
}
catch(exception& e){
  cerr<< "[fcgi] Exception: " << e.what() << endl;
  return 666;
}
catch(...){
  cerr<< "[fcgi] Unknown exception!" << endl;
  return 667;
}
//]

