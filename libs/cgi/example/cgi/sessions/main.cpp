
#include <boost/cgi/cgi.hpp>
#include <iostream>

using namespace std;

class context
{
public:

  context() {}
  
  map<string,int> data;

  template<typename Archive>
  void serialize(Archive& ar, const unsigned int version)
  {
    ar & data;
  }
  
};

/*<
 * Now we can override the default session type to use the `context` class.
 *
 * To do this we need to create a `Tag`. A `Tag` is a plain struct used as
 * an identifier. If we specialise the `protocol_traits<>` template for this
 * tag we can change the behaviour of the library at compile-time.
 *
 * In this example we are just going to override the actual `session_type`
 * and otherwise inherit the properties for CGI.
>*/

/// The `Tag`
struct session_enabled_cgi {};

/// Specialise the `protocol_traits` template in the `boost::cgi::common`
/// namespace.
namespace boost { namespace cgi { namespace common {

template<>
struct protocol_traits<session_enabled_cgi>
  : protocol_traits<boost::cgi::tags::cgi>
{
  typedef basic_session<context> session_type;
  static const bool auto_start_session = false;
};

} } } // namespace boost::cgi::common

using namespace boost::cgi;

/// Define a request type which uses the traits for our
/// `session_enabled_cgi` Tag.
typedef basic_request<session_enabled_cgi> my_request;

int main(int, char**)
{
  cerr<< "Started: " << time(NULL) << endl;
  try
  {
    boost::uuids::basic_random_generator<boost::rand48> generator;
    cerr<< "Here" << endl;
    boost::uuids::uuid u = generator();
    cerr<< "generated" << endl;
 

    my_request req;
    response resp;

    //req.load(parse_session);

    resp<< "one = " << req.session.data["one"]
        << ", two = " << req.session.data["two"]
        << ", ten = " << req.session.data["ten"];
    
    // Modify the request session.
    req.session.data["one"] = 1;
    req.session.data["two"] = 2;
    req.session.data["ten"] = 10;

    // Set the session id, so the data is saved.    
    //req.session_id_ = "1";

    resp<< content_type("text/plain") << "\nBlah\n";
    
    commit(req, resp);

    return 0;
  
  } catch (std::exception& e) {
    cerr<< "Error: " << e.what() << endl;
  }

  cout<< "Content-type: text/html\r\n\r\nBoom";
}
