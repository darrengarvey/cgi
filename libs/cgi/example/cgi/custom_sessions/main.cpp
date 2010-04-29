
//[custom_sessions

#include <boost/cgi/cgi.hpp>
#include <iostream>

using namespace std;
using namespace boost::cgi;

int main(int, char**)
{
  try
  {
    request req;
    response resp;
    
    if (req.get.pick("clear", "") == "1") {
      req.stop_session();
      resp<< "Cleared session";
      return commit(req, resp);
    }

    // Start the session. It is safe to call `start_session()` if a session
    // is already open, but you shouldn't have to do that...
    req.start_session();

    resp<< content_type("text/plain")
        << "one = " << req.session["one"]
        << ", two = " << req.session["two"]
        << ", ten = " << req.session["ten"];

    // Modify the request session.
    req.session["one"] = "1";
    req.session["two"] = "2";
    req.session["ten"] = "10";

    // The session is saved by `commit()`.
    return commit(req, resp);
  
  } catch (std::exception& e) {
    cerr<< "Error: " << e.what() << endl;
  }

  cout<< "Content-type: text/html\r\n\r\nAn error occurred.";
}

//]
