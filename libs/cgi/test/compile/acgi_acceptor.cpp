#include "boost/cgi/acgi/acceptor.hpp"

int main()
{
  cgi::acgi::service s;
  cgi::acgi::acceptor a(s);

  return 0;
}
