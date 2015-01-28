//                    -- main.hpp --
//
//           Copyright (c) Darren Garvey 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
//
//[cgi_file_browser
//
// This example is a simple browser-based file browser.
//
///////////////////////////////////////////////////////////
#include <istream>
#include <fstream>
#include <boost/filesystem.hpp>
#include <boost/shared_array.hpp>
#include <boost/algorithm/string/find.hpp>
#include <boost/algorithm/string/case_conv.hpp>
///////////////////////////////////////////////////////////
#include "boost/cgi/cgi.hpp"

using std::cerr;
using std::endl;
using std::ifstream;
using namespace boost::cgi;
namespace fs = boost::filesystem;
namespace algo = boost::algorithm;


/// Get the MIME type of the file. 
/**
 * @returns The MIME type as a string. Returns an empty string if the
 *          file type isn't recognised / supported.
 */
std::string get_mime_type(fs::path const& file)
{
  fs::path ext = file.extension();
  if (ext.empty())
    return "";
  // Note: we want the string after the '.'
  std::size_t pos(ext.string().rfind("."));
  std::string filetype (ext.string().substr(pos));
  algo::to_lower(filetype);
  
  /// Ordinary text files.
  if (filetype == "ini" || filetype == "txt" || filetype == "conf")
    return "text/plain";
  else
  if (filetype == "js")
    return "application/javascript";
  else
  if (filetype == "json")
    return "application/json";
  else
  if (filetype == "css")
    return "text/css";
  else
  /// Structured text files.
  if (filetype == "html" || filetype == "htm")
    return "text/html";
  else
  if (filetype == "xml")
    return "text/xml";
  else
  if (filetype == "csv")
    return "text/csv";
  else
  if (filetype == "rtf")
    return "text/rtf";
  else
  /// Image files.
  if (filetype == "jpg" || filetype == "jpeg")
    return "image/jpeg";
  else
  if (filetype == "gif")
    return "image/gif";
  else
  if (filetype == "bmp")
    return "image/x-ms-bmp";
  else
  if (filetype == "png")
    return "image/png";
  else  
  if (filetype == "tiff")
    return "image/tiff";
  else  
  /// Audio files.
  if (filetype == "ogg")
    return "audio/ogg";
  else
  if (filetype == "mp3")
    return "audio/mpeg";
  else
  /// Video files.
  if (filetype == "avi")
    return "video/x-msvideo";
  else
  /// Rich media files.
  if (filetype == "pdf")
    return "application/pdf";
  else
  if (filetype == "doc")
    return "application/msword";
  else
  if (filetype == "swf")
    return "application/x-shockwave-flash";
  else
  if (filetype == "xls")
    return "application/vnd.ms-excel";
  /// Compressed files.
  else
  if (filetype == "zip")
    return "application/zip";
  else
  if (filetype == "tar")
    return "application/x-tar";
  /// Other files.
  else
  if (filetype == "pl")
    return "application/x-perl";
  else
  if (filetype == "py")
    return "application/x-python";
  else
  if (filetype == "exe" || filetype == "dll" || filetype == "sys" ||
      filetype == "chm" || filetype == "lib" || filetype == "pdb" ||
      filetype == "obj" || filetype == "dep" || filetype == "idb" ||
      filetype == "pyd" || filetype == "sqm" || filetype == "idb" ||
      filetype == "asm" || filetype == "suo" || filetype == "sbr")
    return ""; // Not allowed to download these file types.

  return "text/plain";
}

/// Show the file to the user.
/**
 * This will send the file to the user using an appropriate content-type
 * header. Some browsers (eg. Firefox) will be able to handle many file types
 * directly, while others (eg. Internet Explorer) will prompt the user to
 * save / open the file externally.
 *
 * This function actually buffers the entire file before sending it to the user,
 * so this isn't very scalable. You could choose to stream the file directly
 * instead, but make sure your HTTP server won't buffer the response.
 *
 * Files over 200MB won't be displayed.
 */
template<typename Response, typename Client>
void show_file(Response& resp, Client& client, fs::path const& file)
{
  if (!fs::exists(file))
    resp<< "File not found.";
  else
  {
    boost::uintmax_t size (fs::file_size(file));
    if (size > 200000000L) // Files must be < 200MB
      resp<< "File too large: " << file.string() << " [" << size << ']';
    else
    {
      /// Check the file type is allowed.
      std::string mime_type (get_mime_type(file));
      if (!mime_type.empty())
      {
        std::string ctype (content_type(mime_type).content + "\r\n\r\n");
        /// Open the file and read it as binary data.
        ifstream ifs (file.string().c_str(), std::ios::binary);
        if (ifs.is_open())
        {
          resp<< content_type(mime_type);
          boost::uintmax_t bufsize = 500;
          boost::uintmax_t read_bytes;
          char buf[500];
          ifs.seekg(0, std::ios::beg);
          while (!ifs.eof() && size > 0)
          {
            ifs.read(buf, size < bufsize ? size : bufsize);
            read_bytes = ifs.gcount();
            size -= read_bytes;
            resp.write(buf, read_bytes);
          }
        }
      }
      else
        resp<< "File type not allowed.";
    }
  }
}
    
template<typename Response>
void show_paths(Response& resp, fs::path const& parent, bool recursive = true)
{
  if (!fs::exists(parent))
  {
    resp<< "File does not exist\n";
    return;
  }

  resp<< "<ul>";
  if (fs::is_directory(parent))
  {
    resp<< parent.string() << "\n";
    resp<< "<li class=\"directory\"><a href=\"?dir="
        << parent.string() << "\">.</a></li>\n";
    if (fs::is_directory(parent.parent_path()))
    {

      resp<< "<li class=\"directory\"><a href=\"?dir="
          << parent.parent_path().string() << "\">..</a></li>\n";
    }
    boost::system::error_code ec0, ec1;
    for (fs::directory_iterator iter(parent, ec0), end; iter != end; ++iter)
    {
      const bool is_dir = fs::is_directory(*iter, ec1);
      if (ec0.value() || ec1.value())
      {
        resp << "<li>" << iter->path().filename().string()
             << " ("
             << (ec0.value() ? ec0.message() : ec1.message())
             << ")</li>\n";
      }
      else if (is_dir)
      {
        resp<< "<li class=\"directory\"><a href=\"?dir="
            << iter->path().string() << "\">"
	    << iter->path().filename().string()
            << iter->path().preferred_separator << "</a></li>\n";
        if (recursive && !fs::is_symlink(*iter))
          show_paths(resp, iter->path(), recursive);
      }
      else
      {
        // display filename only.
        resp<< "<li class=\"file\"><a href=\"?file="
            << iter->path().string() << "\">"
            << iter->path().filename().string()
            << "</a>";
        //if (fs::is_regular_file(iter->status()))
        //  resp<< " [" << fs::file_size(iter->path()) << " bytes]";
        resp<< "</li>\n";
      }
    }
  }
  else
  {
    resp<< "<li class=\"file\">" << "<a href=\"?file="
            << parent.string() << "\">" << parent << "</li>\n";
  }
  resp<< "</ul>";
}

/// This function accepts and handles a single request.
template<typename Request>
int handle_request(Request& req)
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

  if (req.get.count("file"))
  {
    show_file(resp, req.client(), req.get["file"]);
    //return req.close(http::ok, 0);
  }
  else
  if (req.get.count("dir"))
  {
    //
    // Responses in CGI programs require at least a 'Content-type' header.
    // The library provides helpers for several common headers:
    //
    resp<< content_type("text/html");
    
    // You can also stream text to a response. 
    // All of this just prints out the form 
    resp<< "<html>"
           "<head><title>CGI File Browser Example</title><head>"
           "<body>";

    show_paths(resp, req.get["dir"], req.get["recurse"] == "1");

    resp<< "</body></html>";
  }
  else
    resp<< content_type("text/html")
        << "No path specified. Search for a path using, eg. "
        << " <a href=\"?dir=/\">" << req.script_name() << "?dir=/some/path</a>\n";

  resp<< header("CGI-client", "fcgi_file_browser");
  return commit(req, resp);
}

int main()
{
try {

  request req;
  return handle_request(req);

}catch(boost::system::system_error const& se){
  // This is the type of error thrown by the library.
  cerr<< "[fcgi] System error: " << se.what() << endl;
  return -1;
}catch(std::exception const& e){
  // Catch any other exceptions
  cerr<< "[fcgi] Exception: " << e.what() << endl;
  return -1;
}catch(...){
  cerr<< "[fcgi] Uncaught exception!" << endl;
  return -1;
}
}
//]
