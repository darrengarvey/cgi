
#include <cstdlib>

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#define BOOST_TEST_MODULE hello_world_test
//#include <boost/test/unit_test.hpp>
#ifndef BOOST_TEST_DYN_LINK
#define BOOST_TEST_DYN_LINK
#endif
// the following definition must be defined once per test project
#define BOOST_TEST_MAIN
// include Boost.Test
#include <boost/test/unit_test.hpp>
using boost::unit_test::test_suite;

using namespace std;
using namespace boost;
namespace fs = boost::filesystem;

void check_output( string const& example, string const& output )
{
  string wget_cmd ("wget localhost/");
  if (example.substr(0,4) == "fcgi")
  {
    wget_cmd += "fcgi/";
  }
  else
  {
    wget_cmd += "cgi-bin/";
  }
  wget_cmd += example;

  std::system(wget_cmd.c_str());
  BOOST_CHECK( fs::exists(example) );
  BOOST_CHECK( fs::is_regular(example) );
  fs::ifstream ifs(example);
  string s;
  getline(ifs, s);
  BOOST_CHECK_EQUAL( s, output);
  fs::remove(example);
}

BOOST_AUTO_TEST_CASE( hello_world_test )
{
  string output ("Hello there, universe.");
  //check_output("acgi_hello_world", output);
  check_output("cgi_hello_world", output);
  check_output("fcgi_hello_world", output);
}

