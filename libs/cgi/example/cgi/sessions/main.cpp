
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

struct session_enabled_cgi {};

namespace boost { namespace cgi {

template<>
struct protocol_traits<session_enabled_cgi>
  : protocol_traits<boost::cgi::tags::cgi>
{
  typedef basic_session<context> session_type;
};

} } // namespace boost::cgi

using namespace boost::cgi;

typedef basic_request<session_enabled_cgi> my_request;

int main(int, char**)
{
  try
  {
    my_request req;
    response resp;

    resp<< "one = " << req.session.data["one"]
        << ", two = " << req.session.data["two"]
        << ", ten = " << req.session.data["ten"];
    
    // Modify the request session.
    req.session.data["one"] = 1;
    req.session.data["two"] = 2;
    req.session.data["ten"] = 10;

    // Set the session id, so the data is saved.    
    req.session_id_ = "1";

    resp<< content_type("text/plain") << "\nBlah\n";
    
    commit(req, resp);

    cout<< "Press enter to continue...";
    //cin.get();
    return 0;
  
  } catch (std::exception& e) {
    cerr<< "Error: " << e.what() << endl;
  }
  
  cout<< "Press enter to continue...";
  cin.get();
}