#include <boost/cgi/response.hpp>

int main(int, char**)
{
  cgi::response resp;

  resp<< "blah";

  return 0;
}
