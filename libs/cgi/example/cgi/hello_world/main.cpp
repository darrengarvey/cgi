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
// The simplest CGI program, outputs only "Hello there, universe."
//

#include <iostream>
#include <boost/cgi/cgi.hpp>

using namespace boost::cgi;

int main()
{
  request req;      // Our request (POST data won't be parsed yet).
  response resp;    // A response object to make our lives easier.

  // This is a minimal response. The content_type(...) may go before or after
  // the response text.
  resp<< content_type("text/plain")
      << "Hello there, universe.";

  // Leave this function, after sending the response and closing the request.
  return_(resp, req, 0); // Note the underscore: returns "0" to the OS.
}
//]

