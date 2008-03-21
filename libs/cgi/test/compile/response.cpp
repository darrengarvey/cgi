#include <boost/cgi/response.hpp>

int main(int, char**)
{
  cgi::common::response resp;

  resp<< "blah";

  return 0;
}
