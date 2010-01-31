
#ifndef BOOST_CGI_GET_HPP_INCLUDED_
#define BOOST_CGI_GET_HPP_INCLUDED_

#include "boost/cgi/detail/push_options.hpp"

#include <map>
///////////////////////////////////////////////////////////
#include "boost/cgi/common/name.hpp"
#include "boost/cgi/utility/has_key.hpp"

BOOST_CGI_NAMESPACE_BEGIN
 namespace common {

   /// Get a value from the map, or return the passed value as a default.
   /**
    * Examples:
    */
    template<typename MapT>
    std::string
      get_value(MapT& data, typename MapT::key_type const& key
               , typename MapT::mapped_type const& default_value)
    {
      return has_key(data, key) ? data[key] : default_value;
    }

 } // namespace common
BOOST_CGI_NAMESPACE_END

#endif // BOOST_CGI_GET_HPP_INCLUDED_

