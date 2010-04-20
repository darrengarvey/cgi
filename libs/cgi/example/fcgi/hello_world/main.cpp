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

template<typename Request, typename Response>
int handle_request(Request& req, Response& resp)
{
  // This is a minimal response. The content_type(...) may go before or after
  // the response text.
  req.load(parse_env);
  resp<< content_type("text/plain")
      << header("X-Protocol", "FastCGI")
      << "Hello there, universe.";

  return commit(req, resp, 0);
}


//
template<typename OStreamT, typename MapT>
void show_map_contents(OStreamT& os, MapT& m, const std::string& title)
{
  os<< "<h3>" << title << "</h3>";
  
  if (m.empty())
    os<< "NONE<br />";
  else
    for (typename MapT::const_iterator i = m.begin(); i != m.end(); ++i)
      os<< "<b>" << i->first << "</b> = <i>" 
                 << i->second << "</i><br />";
}


int main()
{
  int ret = 0; // the return value

    //for(long i=1000000000; i != 0; --i)
    //{
    //    std::sqrt(1236.456L); // waste time
    //}

  try
  {
    service s;        // This becomes useful with async operations.
    acceptor a(s);

    for (;;)
    {
      // Reusing a request object when possible saves repeated
		  // construction/destruction of the request's memory.
      request req(s);
    
      for (;;) // Handle requests until something goes wrong
              // (an exception will be thrown).
      {
        a.accept(req);
        response resp;    // Use the response class to make our lives easier.
        ret = handle_request(req, resp); // The class defined above.
        if (ret) break;   // Use a new request if something went wrong.
      }
      if (!a.is_open()) break; // Quit completely if the acceptor bails out.
    }
  }catch(boost::system::system_error const& err){
    std::cerr<< "System Error: [" << err.code() << "] - " << err.what() << std::endl;
  }catch(std::exception const& e){
    std::cerr<< "Exception: [" << typeid(e).name() << "] - " << e.what() << std::endl;
  }catch(...){
    std::cerr<< "boom<blink>!</blink>";
  }
  cin.get();
  return ret;
}
//]

