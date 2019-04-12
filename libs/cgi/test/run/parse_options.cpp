
#include "boost/cgi/common/parse_options.hpp"
#define BOOST_TEST_MODULE parse_options_test
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE( test_parse_options )
{
  using namespace boost::cgi::common;

  {
    parse_options opt = parse_env;
    BOOST_CHECK(opt < parse_get);
    BOOST_CHECK(opt < parse_post);
    BOOST_CHECK(opt < parse_cookie);
    BOOST_CHECK(opt < parse_all);
  }

  {
    parse_options opt = parse_form;
    BOOST_CHECK(opt & parse_get);
    BOOST_CHECK(opt & parse_post);
  }

  {
    parse_options opt = parse_all;
    BOOST_CHECK(opt & parse_env);
    BOOST_CHECK(opt & parse_get);
    BOOST_CHECK(opt & parse_post);
    BOOST_CHECK(opt & parse_cookie);
  }
}

