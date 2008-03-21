//                     -- return.hpp --
//
//            Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_RETURN_HPP_INCLUDED__
#define CGI_RETURN_HPP_INCLUDED__

#include "boost/cgi/response.hpp"
#include "boost/cgi/basic_request.hpp"

namespace cgi {
 namespace common {

   template<typename Response, typename Request>
   boost::system::error_code
     return_helper(Response& resp, Request& req, int program_status)
   {
     boost::system::error_code ec;
     resp.send(req.client(), ec);
     if (ec) return ec;

     req.close(resp.status(), program_status);

     return ec;
   }

 } // namespace common
} // namespace cgi

/// If an error occurs during the sending or closing then `status` will be
// incremented by the value of this macro.
#ifndef BOOST_CGI_RETURN_ERROR_INCREMENT
#  define BOOST_CGI_RETURN_ERROR_INCREMENT 100
#endif

#define BOOST_CGI_RETURN(resp, req, status)                     \
          if ( ::cgi::common::return_helper(resp, req, status)) \
            /** error **/                                       \
            return status + BOOST_CGI_RETURN_ERROR_INCREMENT;   \
          return status;

namespace cgi {
 namespace common {

#define return_(resp, req, status) BOOST_CGI_RETURN(resp, req, status)

 } // namespace common
} // namespace cgi

#endif // CGI_RETURN_HPP_INCLUDED__
