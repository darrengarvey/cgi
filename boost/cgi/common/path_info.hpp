
#ifndef BOOST_CGI_COMMON_PATH_INFO_HPP_INCLUDED_
#define BOOST_CGI_COMMON_PATH_INFO_HPP_INCLUDED_

#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
///////////////////////////////////////////////////////////
#include "boost/cgi/fwd/basic_request_fwd.hpp"

namespace cgi {
 namespace common {
   
   struct path_info
   {
     typedef std::string             value_type;
     typedef std::vector<value_type> vector_type;
     typedef vector_type::iterator   iterator;
     
     template<typename S, typename P, typename A>
     path_info(basic_request<S,P,A> & request)
       : value(boost::algorithm::trim_copy_if(request["path_info"],
                  boost::algorithm::is_any_of("/")))
     {
       boost::algorithm::split(parts, value, boost::algorithm::is_any_of("/"));
     }

     path_info(value_type const& str)
       : value(boost::algorithm::trim_copy_if(str, boost::algorithm::is_any_of("/")))
     {
       boost::algorithm::split(parts, value, boost::algorithm::is_any_of("/"));
     }
     
     value_type  value;
     vector_type parts;
   };

 } // namespace common
} // namespace cgi

#endif // BOOST_CGI_COMMON_PATH_INFO_HPP_INCLUDED_

