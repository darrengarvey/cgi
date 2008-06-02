//                    -- main.hpp --
//
//           Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
//
//[xcgi_server2
//
// A protocol-independent program, outputs only
// 
//   "Hello there, universe." 
//
// This one is similar to the xcgi/basic example and equally simple, but here
// the protocol-dependent bits done by `Server` class (see "Server.hpp").
//
//   example/xcgi/basic$ `bjam install`
// 
// will install the example to your cgi-bin and fcgi-bin. Look
// [link ../../doc.qbk here] for more information, including how to set these.
//

#include <boost/cgi/fcgi.hpp>
#include <boost/cgi.hpp>
#include "Server.hpp"

using namespace std;
using namespace boost::cgi::common;

// Our request handler 
struct request_handler
{
  // Handle any request type.
  template<typename Request, typename Response>
  int operator()(Request& req, Response& resp)
  {
    // This is a minimal response. The content_type(...) may go before or after
    // the response text.
    resp<< content_type("text/plain")
        << "Hello there, universe.";
 
    return_(resp, req, 0);
  }
};

int main()
{
try
{
  Server server;
  return server.run(request_handler());
  //return server.run<request_handler>();      // Equivalent to the line above.
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

