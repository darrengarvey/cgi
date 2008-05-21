//                     -- write.hpp --
//
//            Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_WRITE_HPP_INCLUDED
#define CGI_WRITE_HPP_INCLUDED

#include <boost/asio/write.hpp>

namespace cgi {
 namespace common {

  using boost::asio::write;
  using boost::asio::async_write;

 } // namespace common

  //using common::write;
  //using common::async_write;

} // namespace cgi

#endif // CGI_WRITE_HPP_INCLUDED
