// Test the cgi::cookie class works as advertised

/* TODO
 * ----
 * - Verify it works with other string types (std::wstring is probably enough)
 */

#define BOOST_TEST_MODULE cookie_test
#include <boost/test/unit_test.hpp>

#include <sstream>
#include "boost/cgi/common/cookie.hpp"


BOOST_AUTO_TEST_CASE( cookie_constructor_delete )
{
  // Constructing a cookie with only a name should be a valid way to 
  // delete the cookie. ie. set its value to NULL and give it a date
  // in the past
  using namespace cgi;
  using cgi::common::cookie;
  using namespace std;

  string ex("Fri, 05-Jun-1989 15:30:00 GMT");
  cookie ck("unwanted_cookie_name");
  
  BOOST_CHECK(ck.name      == "unwanted_cookie_name");
  BOOST_CHECK(ck.value     == "");
  BOOST_CHECK(ck.path      == "/");
  // this one could be more robust (by using a generic RFC2616 date parser)
  // see: http://www.apps.ietf.org/rfc/rfc2616.html#sec-3.3
  BOOST_CHECK_EQUAL(ck.expires, ex);
  BOOST_CHECK(ck.domain    == "");
  BOOST_CHECK(ck.secure    == false);
  BOOST_CHECK(ck.http_only == false);
}

BOOST_AUTO_TEST_CASE( cookie_constructor_full )
{
  // Check the full version of the constructor works (simple test)
  using namespace cgi;
  using cgi::common::cookie;
  using namespace std;

  string ex("Wed, 03-Oct-2007 16:26:06 GMT");

  cookie ck("name", "value", ex
           , "/cookie", "example.com", false, true);

  BOOST_CHECK(ck.name      == "name");
  BOOST_CHECK(ck.value     == "value");
  BOOST_CHECK(ck.path      == "/cookie");
  // this one could be more robust (by using a generic RFC2616 date parser)
  // see: http://www.apps.ietf.org/rfc/rfc2616.html#sec-3.3
  BOOST_CHECK_EQUAL(ck.expires, ex);
  BOOST_CHECK(ck.domain    == "example.com");
  BOOST_CHECK(ck.secure    == false);
  BOOST_CHECK(ck.http_only == true);
}

BOOST_AUTO_TEST_CASE( cookie_stream_operator )
{
  // Constructing a cookie with only a name should be a valid way to 
  // delete the cookie. ie. set its value to NULL and give it a date
  // in the past
  using namespace cgi;
  using cgi::common::cookie;
  using namespace std;

  string cookie_content(
    "unwanted_cookie_name=; expires=Fri, 05-Jun-1989 15:30:00 GMT; path=/");

  // First check the cookie below outputs the above string when streamed
  // (don't continue if it doesn't: the rest of the test relies on this).
  cookie ck("unwanted_cookie_name");
  ostringstream oss;
  oss<< ck;
  BOOST_ASSERT(oss.str() == cookie_content);

  // Now check if the string cookie_content can be turned into a cookie type,
  // then streamed without being altered.
  //cookie ck2(cookie::from_string(cookie_content));
  //oss.clear();
  //oss<< ck2;
  //BOOST_CHECK(oss.str() == cookie_content);

  // Now try a more complete cookie, using the detailed constructor
  cookie_content = "name=value; expires=Wed, 03-Oct-2007 16:26:06 GMT;"
                   " path=/cookie; domain=example.com; secure; HttpOnly";

  cookie ck2("name", "value", "Wed, 03-Oct-2007 16:26:06 GMT"
            , "/cookie", "example.com", true, true);
  oss.str("");
  BOOST_CHECK(oss.str() == "");
  oss<< ck2;
  string ostr(oss.str());
  BOOST_CHECK_EQUAL(ostr, cookie_content);  
}
