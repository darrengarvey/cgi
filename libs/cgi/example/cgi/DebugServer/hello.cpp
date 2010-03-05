// hello.cpp : Defines the entry point for the application.
//

#include <iostream>

// Include the CGI headers.
#include <boost/cgi/cgi.hpp>
// Uses a server class that catches and reports errors in your handler.
#include "TracingServer.hpp"

using boost::cgi::request;
using boost::cgi::response;
using boost::cgi::header;
using boost::cgi::content_type;


int cgi_handler(request& req, response& resp)
{
    resp<< header("X-Custom-Header", "some value")
        << content_type("text/html")
        << "<html>"
           "<head>"
             "<title>Debug Server</title>"
           "</head>"
           "<body>"
           "<h1>Debugging Server Example</h1>"
           "<p>"
             "The server used in this example will catch exceptions thrown by "
             "your request handler, or a non-zero return value. It will print "
             "some presumably helpful info about what might have caused the "
             "problem - obviously this is just an example, which you'd "
             "probably want to ammend."
           "</p>"
           "<p>"
             "The handler in this example will throw a std::runtime_error if "
             "you pass 'badger=bait!' to the script, for example by following "
             "<a href=\"?badger=bait%21\">this link</a>."
           "</p>"
           "<p>"
             "The handler in this example will also return false if you pass "
             "pass 'spam' to the script, for example by clicking "
             "<a href=\"?spam\">here</a>."
           "</p>"
           "<p>"
             "Finally, you can simulate an error and get traceback details "
             "regardless of whether the script completed by passing 'debug=1',"
             " or by following <a href=\"?debug=1\">this link</a>."
           "</p>"               
           "</body>"
           "</html>";
    if (req.get["badger"] == "bait!")      throw std::runtime_error("You asked for an error, you got one.");
    else if (req.get.count("spam"))   return 33;
    return 0;
}

int main()
{
    TracingServer server;;
    server.run(&cgi_handler);
    return 0;
}
