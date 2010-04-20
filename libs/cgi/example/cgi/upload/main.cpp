//                    -- main.hpp --
//
//           Copyright (c) Darren Garvey 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
//
//[cgi_upload
//

#include <boost/cgi/cgi.hpp>
#include <boost/cgi/utility/stencil.hpp>

namespace cgi = boost::cgi;

int main()
{
try {
  // Construct a request. Parses all GET, POST and environment data,
  // as well as cookies.
  cgi::request req;
  // Using a response is the simplest way to write data back to the client.
  cgi::stencil resp("../stencils/");
  
  resp<< cgi::content_type("text/html");
  resp.set("filename", req.post["file"]);
  resp.set("path", req.uploads["file"].path.parent_path().string());
  resp.set("filename_check", req.uploads["file"].filename);
  resp.set("content_type", req.uploads["file"].content_type);
  resp.set("content_disposition", req.uploads["file"].content_disposition);
  
  resp.expand("cgi_upload.html");

  // Leave this function, after sending the response and closing the request.
  // Returns 0 on success.
  return cgi::commit(req, resp);
  
} catch(std::exception& e) {
  using namespace std;
  cout<< "Content-type: text/plain\r\n\r\n"
      << "Error: " << e.what() << endl;
  return 1;
} catch(...) {
  using namespace std;
  cout<< "Content-type: text/plain\r\n\r\n"
      << "Unexpected exception." << endl;
  return 1;
}
}
//]

