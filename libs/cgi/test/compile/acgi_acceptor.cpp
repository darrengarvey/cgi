#include "boost/cgi/acgi/acceptor.hpp"

int main()
{
  boost::acgi::service s;
  boost::acgi::acceptor a(s);

  return 0;
}
