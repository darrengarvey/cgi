
#define TEST_ENV_DATA(req)                                                       \
                                                                                 \
  /* Check environment parsing */                                                \
  BOOST_CHECK( req.env.size() );                                           \
  BOOST_CHECK_EQUAL( req.env["HTTP_HOST"], "localhost" );                       \
  BOOST_CHECK_EQUAL( req.env["EMPTY_VAR"], "" );                                \
  BOOST_CHECK_EQUAL( req.env["UGLY_VAR"], "$££$^%%£&&^%@%26$ £_abcd" );     \
  BOOST_CHECK_EQUAL( req.env["QUERY_STRING"]                                    \
                   , "hello=world&foo=bar&encoded=%22!%C2%A3$%^$*^hh%%thd@:~" ); \
  /* Check case-insensitive name comparing */                                    \
  BOOST_CHECK_EQUAL( req.env["http_host"], "localhost" );                       \
  /* Check helper function (need to test them all?) */                           \
  BOOST_CHECK_EQUAL( req.script_name(), "some/test/script" );


#define TEST_GET_DATA(req)                                                      \
                                                                                \
  /* Make sure the data is going to be parsed */                                \
  BOOST_CHECK_EQUAL( req.request_method(), "GET" );                             \
                                                                                \
  /* Check GET data/query string parsing */                                     \
  BOOST_CHECK( req.get.size() );                                               \
  BOOST_CHECK_EQUAL( req.get["hello"], "world" );                              \
  BOOST_CHECK_EQUAL( req.get["foo"], "bar" );                                  \
  /* Case-insensitive check */                                                  \
  BOOST_CHECK_EQUAL( req.get["FOO"], "bar" );                                  \
  /* Value should be case-sensitive */                                          \
  BOOST_CHECK_NE( req.get["foo"], "BAR" );                                     \
  /* Check url-decoding */                                                      \
  BOOST_CHECK_EQUAL( req.get["encoded"], "\"!£$%^$*^hh%%thd@:~" );


#define TEST_ONE_COOKIE(req)                                                    \
                                                                                \
    /* Check cookie parsing */                                                  \
    BOOST_CHECK( req.cookies.size() );                                         \
    BOOST_CHECK_EQUAL( req.cookies["foo"], "bar" );                            \
    /* Check case-insensitive name comparison */                                \
    BOOST_CHECK_EQUAL( req.cookies["FOO"], "bar" );

#define TEST_TWO_COOKIES(req)                                                   \
                                                                                \
    BOOST_CHECK_EQUAL( req.cookies["foo"], "bar" );                            \
    BOOST_CHECK_EQUAL( req.cookies["another_one"], "stuff" );

#define TEST_ENCODED_COOKIE(req)                                                \
                                                                                \
    BOOST_CHECK_EQUAL( req.cookies["foo"], "bar" );                            \
    BOOST_CHECK_EQUAL( req.cookies["encoded"], "\"£$%^$*^hh%%thd@:" );


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

