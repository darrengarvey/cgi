
#ifndef BOOST_CGI_COMMON_SOURCE_ENUMS_HPP_INCLUDED__
#define BOOST_CGI_COMMON_SOURCE_ENUMS_HPP_INCLUDED__

#include <boost/fusion/include/at.hpp>
///////////////////////////////////////////////////////////
#include "boost/cgi/common/map.hpp"

namespace cgi {
 namespace common {

   enum env_data_type     { env,     env_data    };
   enum get_data_type     { get,     get_data    };
   enum post_data_type    { post,    post_data   };
   enum cookie_data_type  { cookies, cookie_data };
   enum form_data_type    { form,    form_data   };

   template<typename Impl>
   env_map& env_vars(Impl& impl)       { return boost::fusion::at_c<0>(impl); }
   template<typename Impl>
   get_map& get_vars(Impl& impl)       { return boost::fusion::at_c<1>(impl); }
   template<typename Impl>
   post_map& post_vars(Impl& impl)     { return boost::fusion::at_c<2>(impl); }
   template<typename Impl>
   cookie_map& cookie_vars(Impl& impl) { return boost::fusion::at_c<3>(impl); }

 } // namespace common
} // namespace cgi

#endif // BOOST_CGI_COMMON_SOURCE_ENUMS_HPP_INCLUDED__

