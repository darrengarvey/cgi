#include <boost/static_assert.hpp>
#include "boost/cgi/common/is_async.hpp"


int main()
{
  using namespace boost::cgi::common;

  BOOST_STATIC_ASSERT(is_async<tags::cgi>::value == false);
  BOOST_STATIC_ASSERT(is_async<tags::async_cgi>::value == true);
  BOOST_STATIC_ASSERT(is_async<tags::fcgi>::value == true);
  BOOST_STATIC_ASSERT(is_async<tags::scgi>::value == true);

  return 0;
}
