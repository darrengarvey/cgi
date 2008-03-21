#include <boost/cgi/scgi/service.hpp>
#include <boost/cgi/scgi/request.hpp>

int main()
{
  cgi::scgi::service s;
  cgi::scgi::request r(s);

  return 0;
}
