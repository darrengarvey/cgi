#include "boost/cgi/connections/async_stdio.hpp"

int main()
{
  cgi::io_service ios;
  cgi::async_stdio_connection conn(ios);

  return 0;
}
