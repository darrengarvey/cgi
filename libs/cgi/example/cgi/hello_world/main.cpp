//                    -- main.hpp --
//
//           Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
//
//[cgi_hello_world
//
// A simple CGI program.
//
// Outputs only "Hello there, universe." and saves a session cookie "test=check" to the browser.
//

#include <iostream>
#include <boost/cgi/cgi.hpp>

namespace cgi = boost::cgi;

int main()
{
try {
  // Construct a request. Parses all GET, POST and environment data,
  // as well as cookies.
  cgi::request req;
  // Using a response is the simplest way to write data back to the client.
  cgi::response resp;
  
  // This is a minimal response. The cgi::cookie(...) may go before or after
  // the response text.
  resp<< "Hello there, universe."
      << cgi::cookie("test", "check")
      << cgi::charset("ascii")
      ;
  //resp.set(cgi::cookie("test", "check"));

  // Leave this function, after sending the response and closing the request.
  // Returns 0 on success.
  return cgi::commit(req, resp);
  
} catch(std::exception& e) {
  using namespace std;
  cout<< "Content-type: text/plain\r\n\r\n"
      << "Error: " << e.what() << endl;
  return 1;
} catch(...) {
  using namespace std;
  cout<< "Content-type: text/plain\r\n\r\n"
      << "Unexpected exception." << endl;
  return 1;
}
}
//]

