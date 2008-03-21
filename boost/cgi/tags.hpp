//                     -- tags.hpp --
//
//            Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_TAGS_HPP_INCLUDED__
#define CGI_TAGS_HPP_INCLUDED__

#if _MSC_VER > 1020
#pragma once
#endif

namespace cgi {

  /// SCGI (note, this must be removed)
   struct scgi_{};
  /// FastCGI
   struct fcgi_{};
 
 namespace tags {

   // the null tag type
   struct null {};

   // protocol types
   struct cgi {};
   struct async_cgi {}; // call it acgi?
   struct acgi {};
   struct fcgi {};
   struct scgi {};

   // IoServiceProvider PoolingPolicy types
   struct single_service {};
   struct service_pool {};
   struct round_robin {};

   // Connection types
   struct stdio {};
   struct async_stdio {};
   struct tcp_socket {};

 } // namespace tags
} // namespace cgi

#endif // CGI_TAGS_HPP_INCLUDED__
