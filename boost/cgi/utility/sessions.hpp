
#ifndef BOOST_CGI_SESSIONS_HPP_INCLUDED_
#define BOOST_CGI_SESSIONS_HPP_INCLUDED_

#include "boost/cgi/detail/push_options.hpp"

#include <string>
#include <fstream>
#include <iostream>
///////////////////////////////////////////////////////////
#include <boost/system/error_code.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/map.hpp>
#include <boost/noncopyable.hpp>
///////////////////////////////////////////////////////////
#include "boost/cgi/import/write.hpp"
#include "boost/cgi/common/header.hpp"
#include "boost/cgi/detail/throw_error.hpp"
///////////////////////////////////////////////////////////

/// The directory to store sessions in.
/**
 * Your web server will need read and write permissions to this directory.
 * You are responsible for ensuring that no other users are able to access
 * files in this directory!
 * 
 * Session support is currently experimental and relatively dumb.
 *
 * A file is saved for every user, containing the session data.
 * The session data itself defaults to a std::map<string,string>, which
 * is derived from the ProtocolTraits of the request.
 *
 * Look at the `sessions` example to see how to use your own types
 * for sessions. In general, the idea is that any type that is both
 * DefaultConstructable and also Serializable (as defined by
 * Boost.Serialization) can be used as the session type with no runtime
 * overhead.
 */
#ifndef BOOST_CGI_SESSIONS_DIRECTORY
#  define BOOST_CGI_SESSIONS_DIRECTORY "../sessions/"
#endif // BOOST_CGI_SESSIONS_DIRECTORY

#ifndef BOOST_CGI_SESSION_COOKIE_NAME
#  define BOOST_CGI_SESSION_COOKIE_NAME ".ssid"
#endif // BOOST_CGI_SESSION_COOKIE_NAME

BOOST_CGI_NAMESPACE_BEGIN
 namespace common {

using namespace std;
//using namespace boost::serialization;

///////////////////////////////////////////////////////////

template<typename T>
class basic_session : public T, private boost::noncopyable
{
public:
  typedef T value_type;

  basic_session(string const& id = "")
    : id_(id)
    , loaded_(false)
  {}
  
  basic_session(T& t, string const& id = "")
    : T(t)
    , id_(id)
    , loaded_(false)
  {}
  
  basic_session(T const& t, string const& id = "")
    : T(t)
    , id_(id)
    , loaded_(false)
  {}

  string const& id () const { return id_; }
  void id (string const& new_id) { id_ = new_id; }
  
  operator T() { return static_cast<T&>(*this); }
  operator T() const { return static_cast<T const&>(*this); }

  bool const& loaded() const { return loaded_; }
  void loaded(bool status) { loaded_ = status; }

private:
  string id_;
  bool loaded_;
};

class session_manager
{
public:
  session_manager()
  {}

  template<typename T>
  void save(basic_session<T>& sesh)
  {
    ofstream ofs(
        (std::string(BOOST_CGI_SESSIONS_DIRECTORY) + sesh.id() + ".arc").c_str());
    if (ofs) {
      boost::archive::text_oarchive archive(ofs);
      archive<< static_cast<typename basic_session<T>::value_type&>(sesh);
    }
  }

  template<typename T>
  void load(basic_session<T>& sesh)
  {
    ifstream ifs(
        (std::string(BOOST_CGI_SESSIONS_DIRECTORY) + sesh.id() + ".arc").c_str());
    if (ifs) {
      boost::archive::text_iarchive archive(ifs);
      archive>> static_cast<typename basic_session<T>::value_type&>(sesh);
      sesh.loaded(true);
    }
  }
};

 } // namespace common
BOOST_CGI_NAMESPACE_END

#endif // BOOST_CGI_SESSIONS_HPP_INCLUDED_

