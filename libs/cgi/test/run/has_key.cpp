
#include <map>
#include "boost/cgi/utility/has_key.hpp"

#define BOOST_TEST_MODULE has_key_test_suite
#include <boost/test/unit_test.hpp>

using boost::cgi::has_key;

BOOST_AUTO_TEST_CASE( std_map )
{
  typedef std::map<std::string, std::string> map_type;
  
  map_type m;
  m["1"] = "one";
  m["two"] = "2";
  
  BOOST_CHECK_EQUAL(has_key(m, "1"), true);
  BOOST_CHECK_EQUAL(m["1"], "one");
  BOOST_CHECK_EQUAL(has_key(m, "two"), true);
  BOOST_CHECK_EQUAL(m["two"], "2");

  BOOST_CHECK_EQUAL(has_key(m, "3"), false);
}

#include "boost/cgi/common/map.hpp"

BOOST_AUTO_TEST_CASE( map_test )
{
  typedef boost::cgi::common::map map_type;
  map_type m;
  
  /// First check that upper case compares with lower case.
  m["foo"] = "bar";
  BOOST_CHECK_EQUAL( has_key(m,"foo"), true );
  BOOST_CHECK_EQUAL( has_key(m,"FOO"), true );

  /// Then check that lower case compares with upper case.
  m["FATFOO"] = "minibar";
  BOOST_CHECK_EQUAL( has_key(m,"FATFOO"), true );
  BOOST_CHECK_EQUAL( has_key(m,"fatfoo"), true );

  // Sanity check.
  BOOST_CHECK_EQUAL( has_key(m,"nonfoo"), false );
}

