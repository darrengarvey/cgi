#include <boost/cgi/common/response.hpp>

int main(int, char**)
{
  boost::cgi::common::response resp;

  resp<< "blah";

  return 0;
}
