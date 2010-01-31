#include "boost/cgi/connections/shareable_tcp_socket.hpp"

int main()
{
  boost::cgi::common::io_service ios;
  boost::cgi::connections::shareable_tcp conn(ios);

  return 0;
}
