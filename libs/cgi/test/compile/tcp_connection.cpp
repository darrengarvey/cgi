#include "boost/cgi/connections/tcp_socket.hpp"

int main()
{
  boost::cgi::common::io_service ios;
  boost::cgi::connections::tcp conn(ios);

  return 0;
}
