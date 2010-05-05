
#include "boost/cgi/common/request_data.hpp"
#include "boost/cgi/common/source_enums.hpp"
#include <iostream>

#define BOOST_TEST_MODULE request_data_test_suite
#include <boost/test/unit_test.hpp>

using namespace boost::cgi::common;
using std::make_pair;

template<typename Map>
void test_map(Map& data)
{
  /// First check that upper case compares with lower case.
  data.insert(make_pair("foo", "bar"));
  data.insert(make_pair("pi", "3.1415962"));

  BOOST_CHECK_EQUAL( data["foo"], "bar" );
  BOOST_CHECK_EQUAL( data["FOO"], "bar" );
  BOOST_CHECK_EQUAL( data.count("foo"), 1 );

  // Now check 

  request_data<Map> proxy;
  proxy.set(data);

  BOOST_CHECK_EQUAL( proxy["foo"], "bar" );
  BOOST_CHECK_EQUAL( proxy["FOO"], "bar" );
  BOOST_CHECK_EQUAL( proxy.count("foo"), 1 );

  /// Then check that lower case compares with upper case.
  BOOST_CHECK_EQUAL( proxy.as<double>("pi"), 3.1415962 );
  BOOST_CHECK_EQUAL( proxy.as<float>("PI"), 3.1415962 );

  // Sanity check.
  BOOST_CHECK_NE( proxy["nonfoo"], "dfpij" );

  // Get either the value of a variable, or a default value.
  BOOST_CHECK_EQUAL( proxy.pick("not_in_map", "some_value"), "some_value" );

  // Get either the value of a variable, converted to the specified type, or a default value.
  BOOST_CHECK_EQUAL( proxy.as<int>("foo", 1234), 1234 );
  // Get a default-constructed int when "foo" can't be found in the map,
  // or it's value can't be lexically casted to an int
  // (works on any type supported by Boost.Lexical_cast).
  BOOST_CHECK_EQUAL( proxy.as<int>("foo"), int() );
  // The type of the return value can be automatically deduced.
  BOOST_CHECK_EQUAL( proxy.as("foo", 1234.56), 1234.56 );
}

BOOST_AUTO_TEST_CASE( env_map_test )
{
  env_data m;
  test_map(m);
}

BOOST_AUTO_TEST_CASE( get_map_test )
{
  get_data m;
  test_map(m);
}

BOOST_AUTO_TEST_CASE( post_map_test )
{
  post_data m;
  test_map(m);
}

BOOST_AUTO_TEST_CASE( cookie_map_test )
{
  cookie_data m;
  test_map(m);
}

BOOST_AUTO_TEST_CASE( upload_map_test )
{
  upload_data m;
  test_map(m);
}
