//                     -- return.hpp --
//
//            Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
//
// A macro to make finishing up a request easier.
// eg.
//    return_(resp, req, status);
// is equivalent to
//    resp.send(req.client());
//    req.close(resp.status(), status);
//    return status;
//
#ifndef CGI_RETURN_HPP_INCLUDED__
#define CGI_RETURN_HPP_INCLUDED__

#include "boost/cgi/common/response.hpp"
#include "boost/cgi/basic_request.hpp"
#include "boost/cgi/config.hpp"

BOOST_CGI_NAMESPACE_BEGIN
 namespace common {

   template<typename Response, typename Request>
   boost::system::error_code
     return_helper(Response& response, Request& request, int program_status)
   {
     boost::system::error_code ec;
     response.send(request.client(), ec);
     if (ec) return ec;

     request.close(response.status(), program_status);

     return ec;
   }

 } // namespace common
BOOST_CGI_NAMESPACE_END

/// If an error occurs during the sending or closing then `status` will be
// incremented by the value of this macro.
#ifndef BOOST_CGI_RETURN_ERROR_INCREMENT
#  define BOOST_CGI_RETURN_ERROR_INCREMENT 100
#endif

#define BOOST_CGI_RETURN(response, request, status)                     \
          if ( ::BOOST_CGI_NAMESPACE::common::return_helper(response, request, status)) \
            /** error **/                                               \
            return status + BOOST_CGI_RETURN_ERROR_INCREMENT;           \
          return status;

BOOST_CGI_NAMESPACE_BEGIN
 namespace common {

#define return_(response, request, status) BOOST_CGI_RETURN(response, request, status)

 } // namespace common
BOOST_CGI_NAMESPACE_END

#endif // CGI_RETURN_HPP_INCLUDED__
