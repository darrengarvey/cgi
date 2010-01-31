#include <boost/cgi/scgi/service.hpp>
#include <boost/cgi/scgi/acceptor.hpp>

int main()
{
  boost::scgi::service s;
  boost::scgi::acceptor a(s);

  return 0;
}
