
/*<
This example demonstrates using custom session types to carry around
data suited to your app. In this case, we'll make a dummy authenticated
CGI app that users can log into.

Note that the example uses cTemplate to construct the page, for the sake
of clarity. You will need to install cTemplate to compile the example
yourself.
>*/

//[sessions_quickstart

/*<
Session support is an optional component of the library. To use sessions,
just define `BOOST_CGI_ENABLE_SESSIONS` before including the library
headers.

Sessions are supported by using Boost.Serialization, so you'll need to
link to the library. On Windows, the linking should be done automatically.
>*/
#define BOOST_CGI_ENABLE_SESSIONS
#include <boost/cgi/cgi.hpp>
#include <boost/cgi/utility/stencil.hpp>
#include <boost/date_time/posix_time/time_serialize.hpp>

using namespace std;
namespace cgi = boost::cgi;

/// A basic class that hold site user's information.
class user
{
public:
  string id;
  string name;
  string email;
  string location;

  // This class is Serializable.
  template<typename Archive>
  void serialize(Archive& ar, const unsigned /* version */)
  {
    ar & id & name & email & location;
  }
};

/// The user_manager class checks credentials and logs users in.
/*<
For the purposes of this example, this class is just a stub that
knows about one user: guest.
  
In reality this would do more thorough validation on the authentication
information and look to a database to check the user's credentials and
retrieve their user information.
>*/
class user_manager
{
public:
  /// A dummy implementation of a login function.
  /*<
     For this example, we just have one user, "guest" with a password
     of "password".
  >*/
  template<typename RequestData>
  bool login(RequestData& data, user& usr)
  {
    if (data.count("name") && data.count("pass")) {
      if (data["name"] == "guest" && data["pass"] == "password") {
        usr.id = "1";
        usr.name = "guest";
        usr.email = "\"Guest\" <guest@omnisplat.com>";
        usr.location = "Liverpool, England, UK";
        return true;
      }
    }
    return false;
  }
};

/// Our custom session class.
/*< This class must be both DefaultConstructible and Serializable. >*/
struct my_session
{
  /// Your session class must be DefaultConstrutible.
  my_session()
    : last_accessed(boost::posix_time::microsec_clock::universal_time())
  {
  }
  
  /// The last-accessed time for the user. Defaults to now.
  boost::posix_time::ptime last_accessed;
  
  /// User information.
  user info;
  
  /// Your session class must be Serializable.
  template<typename Archive>
  void serialize(Archive& ar, const unsigned /* version */)
  {
    ar & last_accessed & info;
  }
};

/*<
The library uses tag-dispatching to determine which types to use
internally to make things work. You can specialise the `protocol_traits`
class template to customise parts of the library at compile-time.
The `protocol_traits` template is specialised for the protocols
supported by the library (ie. CGI and FastCGI).

To define a custom type to use for sessions, you can define a
"tag" (just an empty struct) and then specialise `protocol_traits`
for this tag. You should put this specialisation into the
`boost::cgi::common` namespace so it can be found via ADL.

In general, you will inherit most of the characteristics of either
CGI or FastCGI and override one or two types. You can do this by
inheriting from either `protocol_traits<tags::cgi>` or
`protocol_traits<tags::fcgi>`.

eg. This is overriding a protocol the long way.

struct mycgi; // The protocol tag

namespace boost { namespace cgi { namespace common {

  template<>
  struct protocol_traits<mycgi>
    : protocol_traits<tags::cgi>
  {
    // some custom traits.
  };

}}} // namespace boost::cgi::common

// Then add typedefs to use the custom tag struct.
struct mycgi {
  typedef boost::cgi::common::basic_protocol_service<mycgi> service;
  typedef boost::cgi::common::basic_request_acceptor<mycgi> acceptor;
  typedef boost::cgi::common::basic_request<mycgi> my_request;
};

   The `BOOST_CGI_OVERRIDE_PROTOCOL` macro is available as a shortcut
   for the above. You pass the protocol as the first argument, the
   name of your "tag" as the second and the third is a brace-enclosed
   list of traits. The code between the braces is actually the body of
   the specialised `protocol_traits<>` class template.
  
   When you use `BOOST_CGI_OVERRIDE_PROTOCOL`, you will end up with a
   struct with nested typedefs for `request`, `service` and `acceptor`.
>*/

/// Declare a custom protocol `mycgi`.
BOOST_CGI_OVERRIDE_PROTOCOL(cgi, mycgi, {
    typedef basic_session<my_session> session_type;
    static const bool auto_start_session = false;
})

int main(int, char**)
{
  try
  {
    mycgi::request req; // Our custom request type.
    cgi::stencil resp; // A basic wrapper over cTemplate.
    user_manager user_mgr;
    
    resp.set("last_accessed",
        boost::posix_time::to_simple_string(req.session.last_accessed));

    if (req.form.count("login"))
    {
      if (user_mgr.login(req.form, req.session.info)) {
        // Mark the request so when you call `commit`, the session is saved.
        req.start_session();
        // Redirect them back to where they started.
        resp<< cgi::redirect(req, req.script_name());
      } else {
        resp.set("user_name", req.form["name"]);
        // Show the HAS_ERROR section and set the {{error}} token.
        resp.set("error", "Login failed.", "HAS_ERROR");
        // Show all LOGIN_FORM sections.
        resp.show("LOGIN_FORM");
      }
    }
    else
    if (req.form.count("logout"))
    {
      /*<
         We call `stop_session` to close the session. This call deletes the
         session data stored on the server, but leaves the session object
         stored in memory as is. The session cookie itself is only removed
         when you call `commit`, later.
      req.stop_session();
         Redirect the user. This causes a "Location" header to be added to
         the response.
      >*/
      resp<< cgi::redirect(req, req.script_name());
    }
    else
    if (!req.session.info.name.empty())
    {
      resp.set("user_name", req.session.info.name);
      resp.set("user_email", req.session.info.email);
      resp.set("user_location", req.session.info.location);
      resp.show("AUTHENTICATED_PAGE");
    }
    else
    {
      resp.show("DEFAULT_PAGE");
      resp.show("LOGIN_FORM");
    }

    /*<
       This is where the response is actually built up, using the specified
       stencil: In this case we send back an HTML page. This could easily
       be turned into an AJAX-style page by constructing a stencil that looks
       like JSON when expanded and setting the content-type to
       `"application/json"`.
      
       Note that if the template looks like JSON, cTemplate can be very
       clever and auto-escape the content so you get valid JSON. For more
       info, take a look at the ctemplate documentation:
       @ http://code.google.com/p/google-ctemplate/
    >*/
    resp.expand("../stencils/login.html");
    resp<< cgi::content_type("text/html");
    
    req.session.last_accessed = boost::posix_time::microsec_clock::universal_time();
    
    // Send the response and save session data.
    return cgi::commit(req, resp);
  
  } catch (std::exception& e) {
    cerr<< "Error: " << e.what() << endl;
  } catch (...) {
    cerr<< "Unknown error" << endl;
  }

  cout<< "Content-type: text/html\r\n\r\nAn error occurred.";
}
//]