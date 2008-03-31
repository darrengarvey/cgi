//                  -- data_source.hpp --
//
//            Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_DATA_SOURCE_HPP_INCLUDED__
#define CGI_DATA_SOURCE_HPP_INCLUDED__

namespace cgi {

  enum source
  { stdin_ };

 namespace common {

  enum data_source
  { get_data
  , post_data
  , cookie_data
  , env_data
  , form_data
  };
 
 } // namespace common
} // namespace cgi

#endif // CGI_DATA_SOURCE_HPP_INCLUDED__
