//                     -- read.hpp --
//
//           Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_READ_HPP_INCLUDED
#define CGI_READ_HPP_INCLUDED

#include <boost/asio/read.hpp>

namespace cgi {

  using boost::asio::read;
  using boost::asio::async_read;

} // namespace cgi

#endif // CGI_READ_HPP_INCLUDED

