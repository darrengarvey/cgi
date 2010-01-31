#include "boost/cgi/connections/async_stdio.hpp"

int main()
{
  boost::cgi::common::io_service ios;
  boost::cgi::connections::async_stdio conn(ios);

  return 0;
}
