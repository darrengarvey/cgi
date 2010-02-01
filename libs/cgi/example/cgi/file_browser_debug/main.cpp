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
#include <fstream>
#include <boost/filesystem.hpp>
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


std::size_t process_id()
{
#if defined(BOOST_WINDOWS)
  return _getpid();
#else
  return getpid();
#endif
}

/// Get the MIME type of the file. 
/**
 * @returns The MIME type as a string. Returns an empty string if the
 *          file type isn't recognised / supported.
 */
std::string get_mime_type(fs::path const& file)
{
  std::string filetype (file.filename());
  // Note: we want the string after the '.'
  std::size_t pos (filetype.rfind(".")+1);
  if (pos == std::string::npos)
    return "";
  
  filetype = filetype.substr(pos);
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
  if (filetype == "flac")
    return "audio/flac";
  else
  if (filetype == "mp3")
    return "audio/mpeg";
  else
  /// Video files.
  if (filetype == "avi")
    return "video/x-msvideo";
  else
  if (filetype == "wmv")
    return "video/x-ms-wmv";
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

template<typename Response, typename Client>
boost::system::error_code& show_file(
    Response& resp, Client& client, fs::path const& file
  , boost::system::error_code& ec)
{
  if (!fs::exists(file))
    resp<< content_type("text/plain")
        << "File not found.";
  else
  {
    boost::uintmax_t size (fs::file_size(file));
    cerr<< "size: " << size << endl;
    if (size > 750000000L) // Files must be < 750MB (this is arbitrary).
      resp<< "File too large.";
    else
    {
      /// Check the file type is allowed.
      std::string mime_type (get_mime_type(file));
      if (!mime_type.empty())
      {
        // Write unbuffered (ie. not using a response). This makes sense
        // for large files, which would take up too much memory to
        // buffer.
        // First, some debugging to the console.
        cerr<< "MIME-type: " << mime_type << '\n';
        cerr<< "File size: " << size << '\n';
        /// Open the file and read it as binary data.
        ifstream ifs (file.string().c_str(), std::ios::binary);
        if (ifs.is_open())
        {
          // Write out the response headers.
          std::string ctype (content_type(mime_type));
          std::string clen (content_length<char>(size));
          clen += "\r\n\r\n";
          
          // Create an output buffer. This will only hold references
          // to the data so we need to ensure the data stays in memory
          // until the write completes.
          //
          // The purpose of this buffer is so efficient I/O operations 
          // (eg. scatter-gather) can be used under the hood. This vector
          // is light-weight so it can be copied cheaply.
          std::vector<boost::asio::const_buffer> output(2);
          output.push_back(boost::asio::buffer(ctype));
          output.push_back(boost::asio::buffer(clen));

          // Read then write up to 1MB at a time.
          boost::uintmax_t bufsize = 1000000;
          boost::uintmax_t read_bytes;
          char buf[10000];
          ifs.seekg(0, std::ios::beg);
          cerr<< "Writing file contents now." << endl;
          while (!ifs.eof() && size > 0 && !ec)
          {
            std::streamsize num = (std::streamsize)(size < bufsize ? size : bufsize);
            cerr<< "num = " << num << endl;
            ifs.read(buf, num);
            read_bytes = ifs.gcount();
            cerr<< "Read " << read_bytes << " bytes (buf size: " << strlen(buf) << "." << endl;
            size -= read_bytes;

            output.push_back(boost::asio::buffer(buf, read_bytes));
            // Write unbuffered (ie. not using a response).
            write(client, output
                 , boost::asio::transfer_all(), ec);

            cerr<< "Written (got: " << ec.message() << ")." << endl;
            // This needs to go at the end, so the Content-type, etc. 
            // headers are sent the first time around.
            output.clear();
          }
        }
        else
        {
          resp<< content_type("text/plain")
              << "File cannot be opened. Please try again later.";
        }
      }
      else
      {
        resp<< content_type("text/plain")
            << "File type not allowed.";
      }
    }
  }
  return ec;
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
    resp<< parent << "\n";
    resp<< "<li class=\"directory\"><a href=\"?dir="
        << parent.string() << "\">.</a></li>\n";
    if (fs::is_directory(parent.parent_path()))
      resp<< "<li class=\"directory\"><a href=\"?dir="
          << parent.parent_path().string() << "\">..</a></li>\n";
    for (fs::directory_iterator iter(parent), end; iter != end; ++iter)
    {
      if (fs::is_directory(*iter))
      {
        resp<< "<li class=\"directory\"><a href=\"?dir="
            << iter->string() << "\">" << iter->path() << "</a></li>\n";
        if (recursive)
          show_paths(resp, iter->path(), recursive);
      }
      else
      {
        // Display only the filename.
        resp<< "<li class=\"file\"><a href=\"?file="
            << iter->string() << "\">" << iter->path().filename()
            << "</a>";
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
  req.load(parse_get); // Read and parse STDIN (ie. POST) data.

  //
  // Construct a `response` object (makes writing/sending responses easier).
  //
  response resp;

  if (req.get.count("file"))
  {
    boost::system::error_code ec;
    show_file(resp, req.client(), req.get["file"], ec);
    if (ec)
    {
      cerr<< "Writing file finished unexpectedly!\n"
             "  Error " << ec << ": " << ec.message() << '\n';
      // Carry on processing other requests.
      return req.close(http::request_timeout, -1, ec);
    }
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
    resp<< content_type("text/plain")
        << "No path specified.\n";

  resp<< header("FastCGI-client", "fcgi_file_browser");
  return commit(req, resp);
}

int main()
{
try {

  request req;
  return handle_request(req);

}catch(boost::system::system_error const& se){
  // This is the type of error thrown by the library.
  cerr<< "[cgi] System error: " << se.what() << endl;
  return -1;
}catch(std::exception const& e){
  // Catch any other exceptions
  cerr<< "[cgi] Exception: " << e.what() << endl;
  return -1;
}catch(...){
  cerr<< "[cgi] Uncaught exception!" << endl;
  return -1;
}
}
//]
