#include <boost/cgi/scgi/service.hpp>
#include <boost/cgi/scgi/acceptor.hpp>

int main()
{
  cgi::scgi::service s;
  cgi::scgi::acceptor a(s);

  return 0;
}
