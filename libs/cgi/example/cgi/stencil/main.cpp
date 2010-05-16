//                      -- main.hpp --
//
//          Copyright (c) Darren Garvey 2007-2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
//
//[cgi_stencil_headers
#include <iostream>
#include <boost/cgi/cgi.hpp>
#include <boost/cgi/utility/stencil.hpp>
//]

//[cgi_stencil_main
namespace cgi = boost::cgi;

int handle_request(cgi::request& req)
{
  req.load(cgi::parse_all);
  
  // Construct a response that uses Google cTemplate. Also sets the root
  // directory where the stencils are found.
  cgi::stencil resp("../stencils/");

  //// Test 1.

  // This is a minimal response. The content_type(...) may go before or after
  // the response text.
  // The content of the response - which is everything streamed to it - is
  // added to a {{content}} field in the stencil.
  resp<< cgi::content_type("text/html")
      << "Hello there, universe!";
      
  //// Test 2.

  // Set some fields.
  resp.set("script_name", req.script_name()); // Populates {{script_name}}.
  resp.set("some_string", req.get["string"]); // Populates {{some_string}}.
  // set() supports any type that Boost.Lexical_cast does.
  resp.set("short_bits", 8 * sizeof(short)); // Populates {{short_bits}}.
  if (req.get.count("short"))
  {
    // Almost any type is supported by pick<>
    // (ie. any type supported by Boost.Lexical_cast.).
    short some_short = req.get.pick<short>("short", -1);
    resp.set("some_short", some_short);
  }
  
  //// Test 3.
  
  // Show a section, conditionally.
  
  // Use the "show" GET variable, or default to the string "show" if not set.
  cgi::request::string_type show = req.get.pick("show", "show");
  resp.set("show", show == "show" ? "hide" : "show");
  if (show == "show")
    resp.show("some_section"); // Shows {{#some_section}}...{{/some_section}}.
  
  //// Test 4.
  
  int num = req.get.pick("count", 0);
  if (num < 0) num = 0;
  resp.set("show_less", num ? num - 1 : 0);
  resp.set("show_more", num + 1);
  cgi::section sec("section_with_variable");
  for (int i(0); i < num; ++i)
  {
    // We can show a section and set one field in it in one go.
    resp.set("some_section_variable", i + 1, sec);
  }
  
  //// Test 5.

  cgi::dictionary test5 = resp.add("test5");
  test5.add("input")
    .set("type", "text")
    .set("name", "text")
    .set("value", req.form["text"])
    .show("label", "Text");

  test5.add("input")
    .set("type", "checkbox")
    .set("name", "check")
    .set("value", "one")
    .show("label", "One")
    .show(req.post.matches("check", "one") ? "checked" : "");
  test5.add("input")
    .set("type", "checkbox")
    .set("name", "check")
    .set("value", "two")
    .show("label", "Two")
    .show(req.post.matches("check", "two") ? "checked" : "");
  test5.add("input")
    .set("type", "checkbox")
    .set("name", "check")
    .set("value", "six")
    .show("label", "Six")
    .show(req.post.matches("check", "six") ? "checked" : "");
  test5.add("input")
    .set("type", "checkbox")
    .set("name", "check")
    .set("value", "ten")
    .show("label", "Ten")
    .show(req.post.matches("check", "ten") ? "checked" : "");

  test5.add("input")
    .set("type", "radio")
    .set("name", "radio")
    .set("value", "yes")
    .show("label", "Yes")
    .show(req.post.matches("radio", "yes") ? "checked" : "");
  test5.add("input")
    .set("type", "radio")
    .set("name", "radio")
    .set("value", "no")
    .show("label", "No")
    .show(req.post.matches("radio", "no") ? "checked  " : "");

  test5.add("input")
    .set("type", "submit")
    .set("value", "Submit");

  //// Test 6.

  resp.add(cgi::section("embedded")).set("test", "passed");
  
  cgi::dictionary dict = resp.add("embedded");
  dict.add("subsection") // returns a new sub-dictionary.
      .set("test", "passed again")
      .set("other", "(another field)");
  dict.set("test", "passed yet again", cgi::section("subsection"));

  //// Test 7.

  // Include another stencil into this one at marker {{>include}}.
  resp.include(
      cgi::section(
          "include",
          "cgi_stencil.include.html"
        )
    );
  
  // Short-cut for stencil includes.
  resp.include("include", "cgi_stencil.include.html");
  
  // Set a session cookie, which expires when the user closes their browser.
  resp<< cgi::cookie("name", "value");

  /// End Tests.
  
  // Expand the response using the specified template.
  // cTemplate has a cache internally, which we can choose to
  // ignore.
  resp.expand("cgi_stencil.html");

  // Send the response and close the request.
  return cgi::commit(req, resp);
}
  
int main()
{
try {
  
  cgi::request req;
  if (handle_request(req))
    std::cerr<< "Error returned from handling request." << std::endl;
  else
    std::cerr<< "Successfully handled request." << std::endl;
  
  return 0;
  
} catch(std::exception& e) {
  using namespace std;
  cout<< "Content-type: text/plain\r\n\r\n"
      << "Error: " << e.what() << endl;
} catch(...) {
  using namespace std;
  cout<< "Content-type: text/plain\r\n\r\n"
      << "Unexpected exception." << endl;
}
  return 1;
}
//]
