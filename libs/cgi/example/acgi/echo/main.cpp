//                    -- main.hpp --
//
//           Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
//
// This example simply echoes all variables back to the user. ie.
// the environment and the parsed GET, POST and cookie variables.
// Note that GET and cookie variables come from the environment
// variables QUERY_STRING and HTTP_COOKIE respectively.
//

#include <boost/cgi/acgi.hpp>
#include <fstream>
//#include <google/template.h>

using namespace std;
using namespace boost::acgi;

// This function writes the title and map contents to the ostream in an
// HTML-encoded format (to make them easier on the eye).
template<typename MapT, typename OStreamT>
void show_map_contents(MapT& m, OStreamT& os, const std::string& title)
{
  os<< "<h3>" << title << "</h3>";
  if (m.empty()) os<< "NONE<br />";
  for (typename MapT::iterator i = m.begin(); i != m.end(); ++i)
  {
    os<< "<b>" << i->first << "</b> = <i>" << i->second << "</i><br />";
  }
}

int main()
{
  try{
    //std::ofstream of("c:/cc/log/cgi.error");
    //if (!of)
    //  throw std::runtime_error("Couldn't open file for writing");
    //std::cerr.rdbuf() = of.rdbuf();
    service s;
    request req(s);

    try {

      boost::system::error_code ec;
      req.load(ec, true);

      if (ec)
      {
        response resp;
        resp
        << content_type("text/plain")
        << "Error " << ec.value() << ": " << ec.message() << "<p />"
           "--Original message follows--"
           "<p />";
        resp.send(req.client());
      }

      response resp;
      resp<< content_type("text/html")
          << "<form method=POST enctype='multipart/form-data'>"
              "<input type=text name=name value='" << req.POST("name") << "' />"
              "<br />"
              "<input type=text name=hello value='" << req.POST("hello") << "' />"
              "<br />"
              "<input type=file name=user_file />"
              "<input type=hidden name=cmd value=multipart_test />"
              "<br />"
              "<input type=submit value=submit />"
             "</form><p />"
             "boundary marker = " << req.boundary_marker() << "<p />";

      show_map_contents(req.GET(), resp, "GET Variables");
      show_map_contents(req.POST(), resp, "POST Variables");
      show_map_contents(req.cookie(), resp, "Cookie Variables");
      show_map_contents(req.env(), resp, "Environment Variables");

      //resp<< content_type("text/html");
      resp.send(req.client());

    }catch(boost::system::error_code& ec){
      response resp;
      resp<< content_type("text/plain") << "Error " << ec.value() << ": "
          << ec.message();
    }catch(std::exception& e){
      response resp;
      resp<< content_type("text/plain") << "Error: " << e.what();
      return_(resp, req, 2);
    }

  }catch(std::exception* e){
    std::cout
    << content_type("text/plain").content
    << "Exception: " << e->what();
    //resp.send(req.client());
    return 3;
  }catch(...){
    std::cout<< content_type("text/plain").content
             << "Unknown error.";
    return 4;
  }
 return 0;
}
