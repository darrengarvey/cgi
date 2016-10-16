//               -- cgi_simple_request.hpp --
//
//           Copyright (c) Darren Garvey 2007-2008.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
//
// Check that cgi::request works as expected.
//

#include "boost/cgi/cgi.hpp"
//#define BOOST_TEST_MODULE cgi_request_test
#include <boost/test/unit_test.hpp>

#include "request_test_template.hpp"

void init_env()
{
  setenv("HTTP_HOST", "localhost", 1);
  setenv("SCRIPT_NAME", "some/test/script", 1);
  setenv("EMPTY_VAR", "", 1);
  setenv("UGLY_VAR", "$££$^%%£&&^%@%26$ £_abcd", 1);
  setenv("HTTP_COOKIE", "foo=bar", 1);
  setenv("QUERY_STRING", "hello=world&foo=bar&encoded=%22!%C2%A3$%^$*^hh%%thd@:~", 1);
  setenv("REQUEST_METHOD", "GET", 1);
}

BOOST_AUTO_TEST_CASE( env_test )
{
  init_env();

  using namespace boost::cgi;
  request req;

  test_env_data(req);
}

BOOST_AUTO_TEST_CASE( get_data_test )
{
  using namespace boost::cgi;
  request req;

  test_get_data(req);
}

BOOST_AUTO_TEST_CASE( cookie_test )
{
  using namespace boost::cgi;

  {
    request req;
    test_one_cookie(req);
  }
  
  {
    // Set two cookies
    setenv("HTTP_COOKIE", "foo=bar;another_one=stuff", 1);

    request req;
    test_two_cookies(req);
  }

  {
    // Set a url-encoded cookie (with a '; ' separator).
    setenv("HTTP_COOKIE", "foo=bar; encoded=%22%C2%A3$%^$*^hh%%thd@:", 1);

    request req;
    test_encoded_cookie(req);
  }
}

