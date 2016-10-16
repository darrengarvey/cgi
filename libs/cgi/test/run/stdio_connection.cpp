#include <boost/system/error_code.hpp>
#include "boost/cgi/buffer.hpp"
#include "boost/cgi/connections/stdio.hpp"

int main()
{
  boost::cgi::common::connections::stdio conn;

  boost::system::error_code ec;

  conn.write_some(cgi::buffer("Hello, world"), ec);

  return 0;
}
