#include "boost/cgi/connections/tcp_socket.hpp"

int main()
{
  cgi::io_service ios;
  cgi::tcp_connection conn(ios);

  return 0;
}
