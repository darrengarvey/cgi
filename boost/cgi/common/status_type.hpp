//              -- common/status_type.hpp --
//
//            Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_COMMON_STATUS_TYPE_HPP_INCLUDED__
#define CGI_COMMON_STATUS_TYPE_HPP_INCLUDED__

namespace cgi {
 namespace common {

   enum status_type
     { null
     , unloaded
     , activated
     , loaded
     , ok = loaded
     , aborted
     , closed
     };

 } // namespace common
} // namespace cgi

#endif // CGI_COMMON_STATUS_TYPE_HPP_INCLUDED__

