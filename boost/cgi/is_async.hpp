//                  -- is_async.hpp --
//
//            Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_IS_ASYNC_HPP_INCLUDED__
#define CGI_IS_ASYNC_HPP_INCLUDED__

#include <boost/mpl/bool.hpp>

#include "tags.hpp"

namespace cgi {

  template<typename Protocol>
  struct is_async
    : boost::mpl::bool_<true>::type
  {
  };

  template<>
  struct is_async<tags::cgi>
   : boost::mpl::bool_<false>::type
  {
  };

} // namespace cgi

#endif // CGI_IS_ASYNC_HPP_INCLUDED__
