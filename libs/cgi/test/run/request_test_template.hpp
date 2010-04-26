
using std::string;

template<typename Request>
void test_env_data(Request& req)
{
  /* Check environment parsing */
  BOOST_CHECK( req.env.size() );
  BOOST_CHECK_EQUAL( string(req.env["HTTP_HOST"]), string("localhost") );
  BOOST_CHECK_EQUAL( string(req.env["EMPTY_VAR"]), string("") );
  BOOST_CHECK_EQUAL( string(req.env["UGLY_VAR"]), string("$££$^%%£&&^%@%26$ £_abcd") );
  BOOST_CHECK_EQUAL( string(req.env["QUERY_STRING"])
                   , string("hello=world&foo=bar&encoded=%22!%C2%A3$%^$*^hh%%thd@:~") );
  /* Check case-insensitive name comparing */
  BOOST_CHECK_EQUAL( string(req.env["http_host"]), string("localhost") );
  /* Check helper function (need to test them all?) */
  BOOST_CHECK_EQUAL( req.script_name(), string("some/test/script") );
}

template<typename Request>
void test_get_data(Request& req)
{
  /* Make sure the data is going to be parsed */
  BOOST_CHECK_EQUAL( req.request_method(), "GET" );

  /* Check GET data/query string parsing */
  BOOST_CHECK( req.get.size() );
  BOOST_CHECK_EQUAL( string(req.get["hello"]), string("world") );
  BOOST_CHECK_EQUAL( string(req.get["foo"]), string("bar") );
  /* Case-insensitive check */
  BOOST_CHECK_EQUAL( string(req.get["FOO"]), string("bar") );
  /* Value should be case-sensitive */
  BOOST_CHECK_NE( string(req.get["foo"]), string("BAR") );
  /* Check url-decoding */
  BOOST_CHECK_EQUAL( string(req.get["encoded"]), string("\"!£$%^$*^hh%%thd@:~") );
}

template<typename Request>
void test_one_cookie(Request& req)
{
    /* Check cookie parsing */
    BOOST_CHECK( req.cookies.size() );
    BOOST_CHECK_EQUAL( string(req.cookies["foo"]), string("bar") );
    /* Check case-insensitive name comparison */
    BOOST_CHECK_EQUAL( string(req.cookies["FOO"]), string("bar") );
}

template<typename Request>
void test_two_cookies(Request& req)
{
    BOOST_CHECK_EQUAL( string(req.cookies["foo"]), string("bar") );
    BOOST_CHECK_EQUAL( string(req.cookies["another_one"]), string("stuff") );
}

template<typename Request>
void test_encoded_cookie(Request& req)
{
    BOOST_CHECK_EQUAL( string(req.cookies["foo"]), string("bar") );
    BOOST_CHECK_EQUAL( string(req.cookies["encoded"]), string("\"£$%^$*^hh%%thd@:") );
}

#include <string>
#include <cstdlib>
#include <boost/config.hpp>

#ifdef BOOST_WINDOWS
  // MSVC doesn't support setenv, but it does support putenv
  void setenv(std::string const& name, std::string const& val, int reset = 0)
  {

    //char *envvar = new char["
    if (0 != _putenv((name + "=" + val).c_str()))
    {
      std::cerr<< "Error adding environment variable." << std::endl;
    }
  }
#endif

