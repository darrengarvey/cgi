#include "boost/cgi/connections/shareable_tcp_socket.hpp"

int main()
{
  cgi::common::io_service ios;
  cgi::common::connection::shareable_tcp conn(ios);

  return 0;
}
