#ifndef CGI_ROLE_TYPE_HPP_INCLUDED__
#define CGI_ROLE_TYPE_HPP_INCLUDED__
#include "boost/cgi/config.hpp"

BOOST_CGI_NAMESPACE_BEGIN
 namespace common {
  namespace role {

   struct responder {};
   struct authorizer {};
   struct filter {};

 } // namespace role
 
  enum role_type
  { none
  , responder
  , authorizer
  , filter
  };

 } // namespace common
BOOST_CGI_NAMESPACE_END

#endif // CGI_ROLE_TYPE_HPP_INCLUDED__
