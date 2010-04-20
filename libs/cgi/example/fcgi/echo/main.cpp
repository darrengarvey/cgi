//                    -- main.hpp --
//
//         Copyright (c) Darren Garvey 2007-2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
//
//[fcgi_echo
//
// This example simply echoes all variables back to the user. ie.
// the environment and the parsed GET, POST and cookie variables.
// Note that GET and cookie variables come from the environment
// variables QUERY_STRING and HTTP_COOKIE respectively.
//
///////////////////////////////////////////////////////////
#include <iostream>
///////////////////////////////////////////////////////////
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/program_options/environment_iterator.hpp>
///////////////////////////////////////////////////////////
#include "boost/cgi/fcgi.hpp"

//using namespace std;
using std::cerr;
using std::endl;
using namespace boost::fcgi;

// The styling information for the page, just to make things look nicer.
static const char* gCSS_text =
"body { padding: 0; margin: 3%; border-color: #efe; }"
"ul.data-map .title"
    "{ font-weight: bold; font-size: large; }"
"ul.data-map"
    "{ border: 1px dotted; padding: 2px 3px 2px 3px; margin-bottom: 3%; }"
"ul.data-map li"
    "{ border-top: 1px dotted; overflow: auto; padding: 0; margin: 0; }"
"ul.data-map div.name"
    "{ position: relative; float: left; width: 30%; font-weight: bold; }"
"ul.data-map div.value"
    "{ position: relative; float: left; width: 65%; left: 1%;"
     " border-left: 1px solid; padding: 0 5px 0 5px;"
     " overflow: auto; white-space: pre; }"
".clear"
    "{ clear: both; }"
;

//
// This function writes the title and map contents to the ostream in an
// HTML-encoded format (to make them easier on the eye).
//
template<typename OStream, typename Request, typename Map>
void format_map(OStream& os, Request& req, Map& m, const std::string& title)
{
  os<< "<ul class=\"data-map\">"
         "<div class=\"title\">"
    <<       title
    <<   "</div>";

  if (m.empty())
    os<< "<li>EMPTY</li>";
  else
    for (typename Map::const_iterator i = m.begin(); i != m.end(); ++i)
    {
      os<< "<li>"
             "<div class=\"name\">"
        <<       i->first
        <<   "</div>"
             "<div class=\"value\">"
        <<       i->second
        <<   "</div>"
           "</li>";
    }
  os<< "<div class=\"clear\"></div></ul>";
}

std::size_t process_id()
{
#if defined(BOOST_WINDOWS)
  return _getpid();
#else
  return getpid();
#endif
}


int handle_request(request& req)
{
  boost::system::error_code ec;
  
  //
  // Load in the request data so we can access it easily.
  //
  req.load(parse_all); // Read and parse STDIN (ie. POST) data.

  //
  // Construct a `response` object (makes writing/sending responses easier).
  //
  response resp;

  //
  // Responses in CGI programs require at least a 'Content-type' header. The
  // library provides helpers for several common headers:
  //
  resp<< content_type("text/html");
  
  // You can also stream text to a response. 
  // All of this just prints out the form 
  resp<< "<html>"
         "<head>"
           "<title>FastCGI Echo Example</title>"
           "<style type=\"text/css\">"
      <<       gCSS_text <<
           "</style>"
         "<head>"
         "<body>"
           "Request ID = " << req.id() << "<br />"
           "Request Hash = " << req.hash() << "<br />"
           "Process ID = " << process_id() << "<br />"
           "<form method=post enctype=\"multipart/form-data\">"
             "<input type=text name=name value='"
      <<         req.post.pick("name", "") << "' />"
             "<br />"
             "<input type=text name=hello value='"
      <<         req.post.pick("hello", "") << "' />"
             "<br />"
             "<input type=file name=user_file />"
             "<input type=hidden name=cmd value=multipart_test />"
             "<br />"
             "<input type=submit value=submit />"
           "</form><p />";

  //
  // Use the function defined above to show some of the request data.
  // (this function isn't part of the library)
  //
  format_map(resp, req, req.env, "Environment Variables");
  format_map(resp, req, req.get, "GET Variables");
  format_map(resp, req, req.post, "POST Variables");
  format_map(resp, req, req.uploads, "File Uploads");
  format_map(resp, req, req.cookies, "Cookie Variables");

  // Print the buffer containing the POST data and the FastCGI params.
  resp<< "<pre>";
  resp<< std::string(req.post_buffer().begin(), req.post_buffer().end());
  resp<< "</pre>";

  //
  // Response headers can be added at any time before send/flushing it:
  //
  resp<< "Response content-length == "
      << resp.content_length(); // the content-length (returns std::size_t)

  // This function finishes up. The optional third argument
  // is the program status (default: 0).
  return commit(req, resp);
}

int main()
{
try {

  // Make a `service` (more about this in other examples).
  service s;
  
  // Make an `acceptor` for accepting requests through.
  acceptor a(s);

  //
  // After the initial setup, we can enter a loop to handle one request at a
  // time until there's an error of some sort.
  //
  int ret(0);
  for (int i(5);i;--i)
  {
    //
    // An acceptor can take a request handler as an argument to `accept` and it
    // will accept a request and pass the handler the request. The return value
    // of `accept` when used like this is the result of the handler.
    //
    // Note that a request handler is any function or function object with the
    // signature:
    //  boost::function<int (boost::fcgi::request&)>
    // See the documentation for Boost.Function and Boost.Bind for more.
    //
    // The acceptor maintains an internal queue of requests and will reuse a
    // dead request if one is waiting.
    //
    ret = a.accept(&handle_request);
    if (ret)
      break;
  }
  
  std::cerr<< "Processing finished. Press enter to continue..." << std::endl;
  std::cin.get();
  
  return ret;

}catch(boost::system::system_error const& se){
  // This is the type of error thrown by the library.
  std::cerr<< "[fcgi] System error: " << se.what() << std::endl;
  return -1;
}catch(std::exception const& e){
  // Catch any other exceptions
  std::cerr<< "[fcgi] Exception: " << e.what() << std::endl;
  return -2;
}catch(...){
  std::cerr<< "[fcgi] Uncaught exception!" << std::endl;
  return -3;
}
  std::cerr<< "Press enter to continue." << std::endl;
  std::cin.get();
}
//]
