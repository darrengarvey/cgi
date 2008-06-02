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
#include <boost/cgi.hpp>

using namespace std;
using namespace boost::fcgi;

=======
/// Handle a FastCGI request
template<typename Acceptor>
int handle_request(Acceptor& a)
{
  int ret = 0;
  for (;;) // Handle requests until something goes wrong
           // (an exception will be thrown).
>>>>>>> .r46066

int main()
{
try
{
  service s;        // This becomes useful with async operations.
  acceptor a(s);
  
  for (;;)
  {
    request req(s);   // Our request.
  
    for (;;) // Handle requests until something goes wrong
             // (an exception will be thrown).
    {
      a.accept(req);
      response resp;    // A response object to make our lives easier.
      ret = handle_request(req, resp);
      if (ret) break; // Use a new request if something went wrong.
    }
    if (!a.is_open()) break; // Quit completely if the acceptor bails out.
  }

  return ret;
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

