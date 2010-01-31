
#include "boost/cgi/common/map.hpp"
#include <iostream>

#define BOOST_TEST_MODULE value_test_suite
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE( map_test )
{
  boost::cgi::common::map m;

  /// First check that upper case compares with lower case.
  m["foo"] = "bar";
  BOOST_CHECK_EQUAL( m["foo"], "bar" );
  BOOST_CHECK_EQUAL( m["FOO"], "bar" );

  /// Then check that lower case compares with upper case.
  m["FATFOO"] = "minibar";
  BOOST_CHECK_EQUAL( m["FATFOO"], "minibar" );
  BOOST_CHECK_EQUAL( m["fatfoo"], "minibar" );

  // Sanity check.
  BOOST_CHECK_NOT_EQUAL( m["nonfoo"], "dfpij" );
}
