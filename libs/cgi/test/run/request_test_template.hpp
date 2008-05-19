
#define TEST_ENV_DATA(req)                                                       \
                                                                                 \
  /* Check environment parsing */                                                \
  BOOST_CHECK( req[env_data].size() );                                           \
  BOOST_CHECK_EQUAL( req[env_data]["HTTP_HOST"], "localhost" );                  \
  BOOST_CHECK_EQUAL( req[env_data]["EMPTY_VAR"], "" );                           \
  BOOST_CHECK_EQUAL( req[env_data]["UGLY_VAR"], "$££$^%%£&&^%@%26$ £_abcd" );    \
  BOOST_CHECK_EQUAL( req[env_data]["QUERY_STRING"]                               \
                   , "hello=world&foo=bar&encoded=%22!%C2%A3$%^$*^hh%%thd@:~" ); \
  /* Check case-insensitive name comparing */                                    \
  BOOST_CHECK_EQUAL( req[env_data]["http_host"], "localhost" );                  \
  /* Check helper function (need to test them all?) */                           \
  BOOST_CHECK_EQUAL( req.script_name(), "some/test/script" ); 


#define TEST_GET_DATA(req)                                                      \
                                                                                \
  /* Make sure the data is going to be parsed */                                \
  BOOST_CHECK_EQUAL( req.request_method(), "GET" );                             \
                                                                                \
  /* Check GET data/query string parsing */                                     \
  BOOST_CHECK( req[get_data].size() );                                          \
  BOOST_CHECK_EQUAL( req[get_data]["hello"], "world" );                         \
  BOOST_CHECK_EQUAL( req[get_data]["foo"], "bar" );                             \
  /* Case-insensitive check */                                                  \
  BOOST_CHECK_EQUAL( req[get_data]["FOO"], "bar" );                             \
  /* Value should be case-sensitive */                                          \
  BOOST_CHECK_NE( req[get_data]["foo"], "BAR" );                                \
  /* Check url-decoding */                                                      \
  BOOST_CHECK_EQUAL( req[get_data]["encoded"], "\"!£$%^$*^hh%%thd@:~" );


#define TEST_ONE_COOKIE(req)                                                    \
                                                                                \
    /* Check cookie parsing */                                                  \
    BOOST_CHECK( req[cookie_data].size() );                                     \
    BOOST_CHECK_EQUAL( req[cookie_data]["foo"], "bar" );                        \
    /* Check case-insensitive name comparing */                                 \
    BOOST_CHECK_EQUAL( req[cookie_data]["FOO"], "bar" );

#define TEST_TWO_COOKIES(req)                                                   \
                                                                                \
    BOOST_CHECK_EQUAL( req[cookie_data]["foo"], "bar" );                        \
    BOOST_CHECK_EQUAL( req[cookie_data]["another_one"], "stuff" );
 
#define TEST_ENCODED_COOKIE(req)                                                \
                                                                                \
    BOOST_CHECK_EQUAL( req[cookie_data]["foo"], "bar" );                        \
    BOOST_CHECK_EQUAL( req[cookie_data]["encoded"], "\"£$%^$*^hh%%thd@:" );


#include <string>
#include <cstdlib>
#include <boost/config.hpp>

#ifdef BOOST_MSVC
  // MSVC doesn't support setenv, but it does support putenv
  void setenv(std::string const& name, std::string const& val, int reset = 0)
  {
    if (putenv((name + val).c_str()) != 0)
    {
      std::cerr<< "Error adding environment variable." << std::endl;
    }
  }
#endif

