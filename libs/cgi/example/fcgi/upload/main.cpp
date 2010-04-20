//                    -- main.hpp --
//
//           Copyright (c) Darren Garvey 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
//
//[fcgi_upload
//

#include <boost/cgi/fcgi.hpp>
#include <boost/cgi/utility/stencil.hpp>

namespace cgi = boost::fcgi;
namespace http = boost::fcgi::http;

int main()
{
try {
  cgi::service service;
  // Construct a request. Parses all GET, POST and environment data,
  // as well as cookies.
  cgi::request req(service);
  
  cgi::acceptor acceptor(service);
  
  int ret(0);
  
  for(;;)
  {
    acceptor.accept(req);
    
    try {
      req.load(cgi::parse_all);
  
      // Using a response is the simplest way to write data back to the client.
      cgi::stencil resp;
  
      resp<< cgi::content_type("text/html");
      resp.set("filename", req.post["file"]);
      resp.set("path", req.uploads["file"].path.parent_path().string());
      resp.set("filename_check", req.uploads["file"].filename);
      resp.set("content_type", req.uploads["file"].content_type);
      resp.set("content_disposition", req.uploads["file"].content_disposition);
  
      resp.expand("upload.html");
    
      ret = cgi::commit(req, resp);
      
    } catch(boost::system::system_error& e) {
      using namespace std;
      cerr<< "Error " << e.code() << ": " << e.what() << endl;
      boost::system::error_code ec;
      ret = req.close(http::internal_server_error, 1, ec);
    } catch(std::exception& e) {
      using namespace std;
      cerr<< "Error: " << e.what() << endl;
      boost::system::error_code ec;
      ret = req.close(http::internal_server_error, 1, ec);
    }
  }

  // Leave this function, after sending the response and closing the request.
  // Returns 0 on success.
  return ret;
  
} catch(std::exception& e) {
  using namespace std;
  cerr<< "Error: " << e.what() << endl;
  cin.get();
  return 1;
} catch(...) {
  using namespace std;
  cerr<< "Unexpected exception." << endl;
  cin.get();
  return 1;
}
}
//]

