#ifndef BOOST_CGI_COMMON_MAP_HPP_INCLUDED__
#define BOOST_CGI_COMMON_MAP_HPP_INCLUDED__

#include <map>
#include "boost/cgi/common/name.hpp"

namespace cgi {
 namespace common {

  typedef std::map< ::cgi::common::name, std::string> map;

  typedef map        env_map;
  typedef map        get_map;
  typedef map        post_map;
  typedef map        form_map;
  typedef map        cookie_map;

 } // namespace common
} // namespace cgi

#endif // BOOST_CGI_COMMON_MAP_HPP_INCLUDED__

