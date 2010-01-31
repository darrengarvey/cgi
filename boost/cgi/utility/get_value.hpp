
#ifndef BOOST_CGI_REDIRECT_HPP_INCLUDED_
#define BOOST_CGI_REDIRECT_HPP_INCLUDED_

#include "boost/cgi/detail/push_options.hpp"

#include <string>
///////////////////////////////////////////////////////////
#include <boost/system/error_code.hpp>
///////////////////////////////////////////////////////////
#include "boost/cgi/import/write.hpp"
#include "boost/cgi/common/header.hpp"
#include "boost/cgi/detail/throw_error.hpp"

BOOST_CGI_NAMESPACE_BEGIN
 namespace common {

   /// Redirect a request to another place.
   /**
    * Note that without also outputting a content-type header, this will be a
    * 'soft' redirect and the location won't be updated. If you want to do a
    * 'hard' redirect, pass a 'Content-type: text/plain' header along with
    * two CRLFs to complete the headers.
    */
   template <typename RequestT>
   basic_header<typename RequestT::char_type>
   redirect(RequestT& req, typename RequestT::string_type const& dest
           , bool secure, boost::system::error_code& ec)
   {
     std::string url(secure ? "https" : "http");
     url += "://" + req.server_name() + "/" + dest;
     basic_header<typename RequestT::char_type> hdr("Location", url);
     return hdr;
   }

   template <typename RequestT>
   basic_header<typename RequestT::char_type>
   redirect(RequestT& req, typename RequestT::string_type const& dest, bool secure = false)
   {
     boost::system::error_code ec;
     basic_header<typename RequestT::char_type>
         hdr(redirect(req, dest, secure, ec));
     detail::throw_error(ec);
     return hdr;
   }

 } // namespace common
BOOST_CGI_NAMESPACE_END

#endif // BOOST_CGI_REDIRECT_HPP_INCLUDED_

