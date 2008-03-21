#include <boost/cgi/fcgi/request.hpp>

int main()
{
  cgi::fcgi::service s;
  cgi::fcgi::request req(s);

  return 0;
}
