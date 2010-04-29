
#define BOOST_CGI_ENABLE_SESSIONS
#include <boost/cgi/utility/sessions.hpp>

#define BOOST_TEST_MODULE sessions_test_suite
#include <boost/test/unit_test.hpp>
#include <iostream>
#include <string>
#include <map>

BOOST_AUTO_TEST_CASE(session_test)
{
  using namespace std;
  using namespace boost::cgi::common;

  typedef basic_session_manager<tags::cgi>                  session_manager;
  typedef basic_session<std::map<std::string,std::string> > session_type;
  
  session_manager session_mgr;
  
  {
    session_type session;

    session_mgr.start(session);
    session["foo"] = "bar";
    session["two"] = "2";

    cout<< "session[\"two\"] = " << session["two"] << endl;

    BOOST_CHECK_EQUAL(session["foo"], "bar");
    BOOST_CHECK_EQUAL(session["two"], "2");
    BOOST_CHECK_NE(session["two"], "4");

    session.id("aaaaa");
    session.loaded(true);
    session_mgr.save(session);
  }

  {
    session_type session;
    session.id("aaaaa");
    session_mgr.load(session);

    BOOST_CHECK_EQUAL(session["foo"], "bar");
    BOOST_CHECK_EQUAL(session["two"], "2");
    BOOST_CHECK_NE(session["two"], "4");
  }

  {
    session_type session;
    session.id("not_an_existing_session");
    session_mgr.load(session);

    BOOST_CHECK_NE(session["foo"], "bar");
    BOOST_CHECK_NE(session["two"], "2");
    BOOST_CHECK_NE(session["two"], "4");
  }

  {
    session_type session;
    session.id("aaaaa");
    session_mgr.close(session);
    BOOST_CHECK_EQUAL(session.loaded(), false);
  }
}