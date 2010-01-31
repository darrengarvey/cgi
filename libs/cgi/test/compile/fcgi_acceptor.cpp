#include <boost/cgi/fcgi/acceptor.hpp>

int main()
{
  boost::fcgi::service s;
  boost::fcgi::acceptor a(s);

  return 0;
}
