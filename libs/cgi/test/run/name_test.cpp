
#include "boost/cgi/common/name.hpp"
#include <iostream>

//#define BOOST_TEST_MODULE name_test_suite
#include <boost/test/unit_test.hpp>

using boost::cgi::common::name;

BOOST_AUTO_TEST_CASE( case_insensitive_cmp )
{
  name n1("foo");
  name n2("FOO");
  name n3("bar");

  BOOST_CHECK_EQUAL( n1, n2 );
  BOOST_CHECK_NE( n1, n3 );
  BOOST_CHECK_NE( n2, n3 );
}

BOOST_AUTO_TEST_CASE( name_from_string )
{
  std::string s("foo");
  name n1 = s.c_str();
  name n2("FOO");
  s = n2.c_str();
  //s = n2;  // won't compile

  BOOST_CHECK_EQUAL( n1, n2 );
  BOOST_CHECK_EQUAL( s, "FOO" );
  BOOST_CHECK_EQUAL( s.c_str(), n1 );
  BOOST_CHECK_EQUAL( n1, s.c_str() );
  BOOST_CHECK_NE( s, n1.c_str() );
}

BOOST_AUTO_TEST_CASE( lt_compare )
{
  name n1("lexicographically_diddy");
  name n2("lexicographically_large");

  BOOST_CHECK( n1 < n2 );
}

BOOST_AUTO_TEST_CASE( adding_names )
{
  name FOO("FOO");
  name foo("foo");

  BOOST_CHECK_EQUAL( FOO + foo, foo + FOO );
}
