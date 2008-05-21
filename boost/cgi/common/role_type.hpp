#ifndef CGI_ROLE_TYPE_HPP_INCLUDED__
#define CGI_ROLE_TYPE_HPP_INCLUDED__

namespace cgi {
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

} // namespace cgi

#endif // CGI_ROLE_TYPE_HPP_INCLUDED__
