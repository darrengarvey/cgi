//               -- acgi_simple_request.hpp --
//
//           Copyright (c) Darren Garvey 2007-2008.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
//
// Check that acgi::request works as expected.
//

#include "boost/cgi/acgi.hpp"
#define BOOST_TEST_MODULE acgi_request_test
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

  using namespace boost::acgi;
  service s;
  request req(s, parse_all);

  TEST_ENV_DATA(req);
}

BOOST_AUTO_TEST_CASE( get_data_test )
{
  using namespace boost::acgi;
  service s;
  request req(s, parse_all);

  TEST_GET_DATA(req);
}

BOOST_AUTO_TEST_CASE( cookie_test )
{
  using namespace boost::acgi;

  {
    service s;
    request req(s, parse_all);
    TEST_ONE_COOKIE(req);
  }
  
  {
    // Set two cookies
    setenv("HTTP_COOKIE", "foo=bar;another_one=stuff", 1);

    service s;
    request req(s, parse_all);
    TEST_TWO_COOKIES(req);
  }

  {
    // Set a url-encoded cookie (with a '; ' separator).
    setenv("HTTP_COOKIE", "foo=bar; encoded=%22%C2%A3$%^$*^hh%%thd@:", 1);

    service s;
    request req(s, parse_all);
    TEST_ENCODED_COOKIE(req);
  }
}

