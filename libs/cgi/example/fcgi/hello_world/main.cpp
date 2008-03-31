//                    -- main.hpp --
//
//           Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
//
//[fcgi_hello_world
//
// The simplest FastCGI program, outputs only "Hello there, universe."
//

#include <boost/cgi/fcgi.hpp>

using namespace std;
using namespace boost::fcgi;

int main()
{
try
{
  service s;        // This becomes useful with async operations.
  acceptor a(s);    // This is used to accept requests from the server.
  request req(s);   // Our request.

  for (int i(50); i != 0; --i) // Handle 50 requests then exit.
  {
    a.accept(req);
    response resp;    // A response object to make our lives easier.

    // This is a minimal response. The content_type(...) may go before or after
    // the response text.
    resp<< content_type("text/plain")
        << "Hello there, universe.";

    resp.send(req.client());  // Send the response.
    req.close(resp.status()); // Close the request (we can reuse this object now).
  }
  return 0;
}
catch(boost::system::system_error& err)
{
  std::cerr<< "System error " << err.code() << ": "
           << err.what() << std::endl;
  return 1;
}
catch(...)
{
  std::cerr<< "Unknown error!" << std::endl;
  return 2;
}
}
//]

