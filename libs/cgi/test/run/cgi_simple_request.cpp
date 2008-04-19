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
#define BOOST_TEST_MODULE cgi_request_test
#include <boost/test/unit_test.hpp>

void init_env()
{
  using namespace std;
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

  // Check environment parsing
  BOOST_CHECK( req[env_data].size() );
  BOOST_CHECK_EQUAL( req[env_data]["HTTP_HOST"], "localhost" );
  BOOST_CHECK_EQUAL( req[env_data]["EMPTY_VAR"], "" );
  BOOST_CHECK_EQUAL( req[env_data]["UGLY_VAR"], "$££$^%%£&&^%@%26$ £_abcd" );
  BOOST_CHECK_EQUAL( req[env_data]["QUERY_STRING"]
                   , "hello=world&foo=bar&encoded=%22!%C2%A3$%^$*^hh%%thd@:~" );
  // Check case-insensitive name comparing
  BOOST_CHECK_EQUAL( req[env_data]["http_host"], "localhost" );
  // Check helper function (need to test them all?)
  BOOST_CHECK_EQUAL( req.script_name(), "some/test/script" );
}

BOOST_AUTO_TEST_CASE( cookie_test )
{
  using namespace boost::cgi;

  {
    request req;

    // Check cookie parsing
    BOOST_CHECK( req[cookie_data].size() );
    BOOST_CHECK_EQUAL( req[cookie_data]["foo"], "bar" );
    // Check case-insensitive name comparing
    BOOST_CHECK_EQUAL( req[cookie_data]["FOO"], "bar" );
  }
  
  {
    // Set two cookies
    setenv("HTTP_COOKIE", "foo=bar;another_one=stuff", 1);

    request req;
    BOOST_CHECK_EQUAL( req[cookie_data]["foo"], "bar" );
    BOOST_CHECK_EQUAL( req[cookie_data]["another_one"], "stuff" );
  }

  {
    // Set a url-encoded cookie (with a '; ' separator).
    setenv("HTTP_COOKIE", "foo=bar; encoded=%22%C2%A3$%^$*^hh%%thd@:", 1);

    request req;
    BOOST_CHECK_EQUAL( req[cookie_data]["foo"], "bar" );
    BOOST_CHECK_EQUAL( req[cookie_data]["encoded"], "\"£$%^$*^hh%%thd@:" );
  }
}

BOOST_AUTO_TEST_CASE( get_data_test )
{
  using namespace boost::cgi;
  request req;

  // Make sure the data is going to be parsed
  BOOST_CHECK_EQUAL( req.request_method(), "GET" );

  // Check GET data/query string parsing
  BOOST_CHECK( req[get_data].size() );
  BOOST_CHECK_EQUAL( req[get_data]["hello"], "world" );
  BOOST_CHECK_EQUAL( req[get_data]["foo"], "bar" );
  BOOST_CHECK_EQUAL( req[get_data]["encoded"], "\"!£$%^$*^hh%%thd@:~" );
}

