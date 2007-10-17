//                -- connection_base.hpp --
//
//            Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_CONNECTION_BASE_HPP_INCLUDED__
#define CGI_CONNECTION_BASE_HPP_INCLUDED__

namespace cgi {

  /// Abstract Base Class for all connection types
  class connection_base
  {
  public:
    virtual ~connection_base() {}

    // read functions
    //template<typename MutableBufferSequence>
    //virtual std::size_t read(MutableBufferSequence) = 0;

    //template<typename MutableBufferSequence, typename Handler>
    //virtual void async_read(MutableBufferSequence, Handler) = 0;

    // write functions
    //template<typename ConstBufferSequence>
    //virtual std::size_t write(ConstBufferSequence) = 0;

    //template<typename ConstBufferSequence, typename Handler>
    //virtual void async_write(ConstBufferSequence, Handler) = 0;

  protected:
    //~connection_base() { }
  };

} // namespace cgi

#endif // CGI_CONNECTION_BASE_HPP_INCLUDED__
