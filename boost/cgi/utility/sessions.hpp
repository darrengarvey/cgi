
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
  {}
  
  basic_session(T& t, string const& id = "")
    : T(t)
    , id_(id)
  {}
  
  basic_session(T const& t, string const& id = "")
    : T(t)
    , id_(id)
  {}
  
  string const& id () const { return id_; }
  void id (string const& new_id) { id_ = new_id; }
  
  operator T() { return static_cast<T&>(*this); }
  operator T() const { return static_cast<T const&>(*this); }

private:
  string id_;
};

class session_manager
{
public:
  session_manager()
  {}

  template<typename T>
  void save(basic_session<T>& sesh)
  {
    ofstream ofs((sesh.id() + ".arc").c_str());
    if (ofs) {
      boost::archive::text_oarchive archive(ofs);
      archive<< static_cast<basic_session<T>::value_type&>(sesh);
    }
  }

  template<typename T>
  void load(basic_session<T>& sesh)
  {
    ifstream ifs((sesh.id() + ".arc").c_str());
    if (ifs) {
      boost::archive::text_iarchive archive(ifs);
      archive>> static_cast<basic_session<T>::value_type&>(sesh);
    }
  }
};

 } // namespace common
BOOST_CGI_NAMESPACE_END

#endif // BOOST_CGI_SESSIONS_HPP_INCLUDED_

