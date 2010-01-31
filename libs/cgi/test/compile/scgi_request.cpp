#include <boost/cgi/scgi/service.hpp>
#include <boost/cgi/scgi/request.hpp>

int main()
{
  boost::scgi::service s;
  boost::scgi::request r(s);

  return 0;
}
