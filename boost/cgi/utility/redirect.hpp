
#ifndef BOOST_CGI_REDIRECT_HPP_INCLUDED_
#define BOOST_CGI_REDIRECT_HPP_INCLUDED_

#include <string>
///////////////////////////////////////////////////////////
#include <boost/system/error_code.hpp>
///////////////////////////////////////////////////////////
#include "boost/cgi/write.hpp"
#include "boost/cgi/header.hpp"
#include "boost/cgi/detail/throw_error.hpp"

namespace cgi {
 namespace common {

   /// Redirect a request to another place.
   template <typename RequestT>
   boost::system::error_code
   redirect(RequestT& req, typename RequestT::string_type const& dest
           , boost::system::error_code& ec)
   {
     basic_header<typename RequestT::char_type> hdr("Location", dest);
     write(req.client(), buffer(hdr.content), ec);
     return ec;
   }

   template <typename RequestT>
   void redirect(RequestT& req, std::string const& dest)
   {
     boost::system::error_code ec;
     redirect(req, dest, ec);
     detail::throw_error(ec);
   }

 } // namespace common
} // namespace cgi

#endif // BOOST_CGI_REDIRECT_HPP_INCLUDED_

