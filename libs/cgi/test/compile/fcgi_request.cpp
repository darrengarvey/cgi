#include <boost/cgi/fcgi/request.hpp>

int main()
{
  boost::fcgi::service s;
  boost::fcgi::request req(s);

  return 0;
}
