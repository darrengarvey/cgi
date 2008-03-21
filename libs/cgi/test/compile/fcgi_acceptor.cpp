#include <boost/cgi/fcgi/acceptor.hpp>

int main()
{
  cgi::fcgi::service s;
  cgi::fcgi::acceptor a(s);

  return 0;
}
