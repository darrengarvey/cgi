
#ifndef BOOST_CGI_HAS_KEY_HPP_INCLUDED_
#define BOOST_CGI_HAS_KEY_HPP_INCLUDED_

#include "boost/cgi/detail/push_options.hpp"

#include <map>
///////////////////////////////////////////////////////////
#include "boost/cgi/common/name.hpp"

BOOST_CGI_NAMESPACE_BEGIN
 namespace common {

   /// Check if the given map has an entry `name`.
   /**
    * Examples:
    *
    * Take a map who's keys are case insensitive:
    *
    *   std::map<common::name,std::string> icase_map;
    *   icase_map["key1"] = "value1";
    *   icase_map["key2"] = "value2";
    *   has_key(icase_map, "key1"); // true
    *   has_key(icase_map, "KEY1"); // true
    *   has_key(icase_map, "key2"); // true
    *   has_key(icase_map, "key3"); // false
    *
    * CGI data is also held in maps with case-insensitive keys.
    * If you sent a request that looked something like, which made its way to
    * your CGI app (probably after being received by your HTTP server):
    *
    *   GET some_cgi_script?foo=bar HTTP/1.1
    *
    * If you wrote `some_cgi_script` and initialised a request, `req`, then:
    *
    *   has_key(req[env], "script_name") // true
    *   has_key(req[env], "SCRIPT_NAME") // true
    *   has_key(req[get], "blah") // false
    *   has_key(req[form], "foo") // true
    *
    * Note that the last line above would also be matched by a POST request,
    * with the same data `foo=bar`.
    */
    template<typename MapT>
    bool has_key(MapT& data, typename MapT::key_type const& key)
    {
      if (!data.empty())
        for(typename MapT::const_iterator iter = data.begin(), end = data.end();
            iter != end; ++iter)
        {
          if (iter->first == key)
            return true;
        }
      return false;
    }

 } // namespace common
BOOST_CGI_NAMESPACE_END

#endif // BOOST_CGI_HAS_KEY_HPP_INCLUDED_

