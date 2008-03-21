#include "boost/cgi/connections/tcp_socket.hpp"

int main()
{
  cgi::common::io_service ios;
  cgi::common::connection::tcp conn(ios);

  return 0;
}
