//                      -- main.hpp --
//
//          Copyright (c) Darren Garvey 2007-2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
//
// [cgi_stencil
//

#include <iostream>
#include <boost/cgi/fcgi.hpp>
#include <boost/cgi/utility/stencil.hpp>

using namespace boost::fcgi;

int handle_request(request& req)
{
  req.load(parse_all);
  
  // Construct a response that uses Google cTemplate. Also sets the root
  // directory where the stencils are found.
  stencil resp("stencils/");

  //// Test 1.

  // This is a minimal response. The content_type(...) may go before or after
  // the response text.
  // The content of the response - which is everything streamed to it - is
  // added to a {{content}} field in the stencil.
  resp<< content_type("text/html")
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
    // This is equivalent to request::as<>, but returns the second parameter
    // iff there is no item in the map and / or the value could not be
    // cast to the specified type. In this case, we shall explicitly cast
    // to a `short`. In general, however, the default type that the value is
    // cast to is inferred from the type of the second parameter.
    short some_short = req.get.pick<short>("short", -1);
    resp.set("some_short", some_short);
  }
  
  //// Test 3.
  
  // Show a section, conditionally.
  
  // Use the "show" GET variable, or default to the string "show" if not set.
  request::string_type show = req.get.pick("show", "show");
  resp.set("show", show == "show" ? "hide" : "show");
  if (show == "show")
    resp.show("some_section"); // Shows {{#some_section}}...{{/some_section}}.
  
  //// Test 4.
  
  int num = req.get.pick("count", 0);
  if (num < 0) num = 0;
  resp.set("show_less", num ? num - 1 : 0);
  resp.set("show_more", num + 1);
  stencil::section sec("section_with_variable");
  for (int i(0); i < num; ++i)
  {
    // We can show a section and set one field in it in one go.
    resp.set("some_section_variable", i + 1, sec);
  }
  
  //// Test 5.

  resp.add(stencil::section("embedded")).set("test", "passed");
  
  stencil::dictionary dict = resp.add("embedded");
  dict.add("subsection") // returns a new sub-dictionary.
      .set("test", "passed again")
      .set("other", "(another field)");
  dict.set("test", "passed yet again", stencil::section("subsection"));

  //// Test 6.

  // Include another stencil into this one at marker {{>include}}.
  resp.include(
      stencil::section(
          "include",
          "stencil.include.html"
        )
    );
  
  // Short-cut for stencil includes.
  resp.include("include", "stencil.include.html");
  
  resp<< cookie("name", "value");

  /// End Tests.
  
  // Expand the response using the specified template.
  // cTemplate has a cache internally, which we can choose to
  // ignore.
  resp.expand("stencil.html", stencil::lazy_reload);

  // Send the response and close the request.
  return commit(req, resp);
}
  
int main()
{
try {
  service s;
  acceptor a(s);
  
  // Handle requests (one at a time) until the counter overflows.
  for(unsigned count(1);count;++count)
  {
    if (a.accept(&handle_request))
      std::cerr<< "Error returned from handling request #" << count << std::endl;
    else
      std::cerr<< "Successfully handled request #" << count << std::endl;
  }
  
  return 0;
  
} catch(std::exception& e) {
  using namespace std;
  cerr<< "Error: " << e.what() << endl;
} catch(...) {
  using namespace std;
  cerr<< "Unexpected exception." << endl;
}
  // Control only reaches here if an exception has been caught.
  using namespace std;
  cerr<< "Press enter to close console window..." << endl;
  cin.get();
  return 1;
}
//]
