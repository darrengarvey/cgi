//                  -- sessions/main.hpp --
//
//            Copyright (c) Darren Garvey 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
//
//[cgi_sessions
//
// A simple example, using the default session type of a
// std::map<string,string>. Session support is optional and
// you should define BOOST_CGI_ENABLE_SESSIONS to use it.
//

#include <boost/cgi/cgi.hpp>
#include <iostream>

using namespace std;
namespace cgi = boost::cgi;

int main(int, char**)
{
  try
  {
    cgi::request req;
    cgi::response resp;
    
    // You can use pick() to return a default value when the item
    // is not found in the request data.
    if (req.get.pick("clear", "") == "1") {
      req.stop_session();
      resp<< "Cleared session";
      return cgi::commit(req, resp);
    }

    // Start the session. It's safe to call `start_session()` if a session
    // is already open.
    req.start_session();

    // Output the current session data.
    resp<< cgi::content_type("text/plain")
        << "one = " << req.session["one"]
        << ", two = " << req.session["two"]
        << ", ten = " << req.session["ten"];

    // Modify the request session.
    req.session["one"] = "1";
    req.session["two"] = "2";
    req.session["ten"] = "10";

    // The session is saved by `commit()`.
    return cgi::commit(req, resp);
  
  } catch (std::exception& e) {
    cerr<< "Error: " << e.what() << endl;
  }

  cout<< "Content-type: text/html\r\n\r\nAn error occurred.";
}

//]
