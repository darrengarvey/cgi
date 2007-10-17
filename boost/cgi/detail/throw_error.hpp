//                  -- throw_error.hpp --
//
//            Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_THROW_ERROR_HPP_INCLUDED__
#define CGI_THROW_ERROR_HPP_INCLUDED__

#include <boost/system/error_code.hpp>
//#include <boost/system/system_error.hpp>

namespace cgi {
 namespace detail {

   void throw_error(boost::system::error_code& ec)
   {
     if(ec)
       throw ec;
//     throw boost::system::system_error(ec);
   }

 } // namespace detail
} // namespace cgi

#endif // CGI_THROW_ERROR_HPP_INCLUDED__
