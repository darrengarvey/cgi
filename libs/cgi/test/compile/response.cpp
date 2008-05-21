#include <boost/cgi/common/response.hpp>

int main(int, char**)
{
  cgi::common::response resp;

  resp<< "blah";

  return 0;
}
