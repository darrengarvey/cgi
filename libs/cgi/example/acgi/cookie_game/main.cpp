#include <boost/cgi/acgi.hpp>
#include <boost/cgi/response.hpp>
#include <iostream>

#define SCRIPT_NAME "acgi_cookie_game"

//
// ISSUES:
// Currently won't work sometimes because the request data checks aren't case-insensitive,
// like they should be.
//

// class handler
// {
// public:
//   handler(cgi::response& resp, boost::acgi::request& req)
//     : resp_(resp)
//     , req_(req)
//   {
//   }
//
//   void operator()(boost::system::error_code& ec)
//   {
//     if (!ec)
//     {
//       resp_<< "All ok";
//       resp_.flush(req_);
//     }
//   }
// private:
//   cgi::response& resp_;
//   boost::acgi::request& req_;
// };

/**
 * The following example has a few stages.
 * It is best understood by compiling and testing it, and then looking at
 * the source code.
 */

int main()
{
  using namespace boost::acgi;

  service srv;
  request req(srv);

  // Load up the request data
  req.load(true);

  response resp;

  if (req.GET("reset") == "true")
  {
    resp<< cookie("name")
        << location(req.script_name())
        << content_type("text/plain");
    resp.send(req.client());
    return 0;
  }

  // First, see if they have a cookie set
  std::string name = req[cookie_data]["name"];
  if (!name.empty())
  {
    resp<< header("Content-type", "text/html")
        << "Hello again " << name << "<p />"
        << "<a href='?reset=true'><input type='submit' value='Reset' /></a>";
    resp.send(req.client());
    return 0;
  }

  // Now we'll check if they sent us a name in a form
  name = req[form_data]["name"];
  if (!name.empty())
  {
    resp<< header("Content-type", "text/html")
        << cookie("name", name)
        << "Hello there, " << "<a href=''>" << name << "</a>";
    resp.send(req.client());
    return 0;
  }

  //std::string buf("Content-type: text/html\r\n\r\nHello there, Universe.<br />");
  //cgi::write(req, cgi::buffer(buf.c_str(), buf.size()));

  resp<< content_type("text/html")
      << "Hello there, Universe.<p />"
      << "What's your name?<br />";

  std::cerr<< std::endl << "name = " << req.POST("name") << std::endl;
  std::cerr.flush();

  resp<< "<form method='POST'>"
         "<input name='name' type='text' value='" << req[form_data]["name"] << "'>"
         "</input>"
         "<input type='submit'></input>"
         "</form>"
      << "<p><b>SCRIPT_NAME</b> = <i>" << req.script_name() << "</i></p>";

  resp.send(req.client());

  return 0;
}
