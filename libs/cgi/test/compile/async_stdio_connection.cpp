#include "boost/cgi/connections/async_stdio.hpp"

int main()
{
  cgi::common::io_service ios;
  cgi::common::async_stdio_connection conn(ios);

  return 0;
}
