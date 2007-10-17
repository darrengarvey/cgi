#include <boost/cgi/acgi.hpp>

int main()
{
  cgi::acgi::service service;
  cgi::acgi::request req(service);

  return 0;
}
