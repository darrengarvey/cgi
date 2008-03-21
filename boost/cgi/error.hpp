//                     -- error.hpp --
//
//            Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
// Abstract:
// ---------
//
// The errors for everything are defined in here. ie. FastCGI,
// CGI and SCGI errors.
//
////////////////////////////////////////////////////////////////
#ifndef CGI_ERROR_HPP_INCLUDED__
#define CGI_ERROR_HPP_INCLUDED__

#include <string>
#include <boost/system/error_code.hpp>

namespace cgi {
  namespace error {

enum fcgi_errors
{
  bad_header_type = 1,

  /// A packet arrived for a request id that doesn't exist and the packet
  // wasn't a BEGIN_REQUEST record.
  bad_request_id,

  /// When trying to write a packet, the client::write_some() call didn't
  // transmit enough data before returning.
  couldnt_write_complete_packet,

  // Tried to read/write from/to the client associated to a request when it
  // was closed.
  client_closed,

  // Self-explanatory (not much a user can do about this though).
  abort_request_record_recieved_for_invalid_request,

  // For now a user has to recognise this error and construct a request
  // themselves. This is an ugly hack.
  multiplexed_request_incoming,

  duplicate_request,

  // Calling async_accept on a request that hasn't been closed isn't
  // allowed.
  accepting_on_an_open_request,

  invalid_socket
};

  namespace detail {

class fcgi_category
  : public boost::system::error_category
{
public:
  const char* name() const { return "fcgi_error"; }
  std::string message(int e) const
  {
    switch(e)
    {
    case client_closed:
      return "You are trying to read from or write to a closed client.";
    case multiplexed_request_incoming:
      return "A new request is pending on this connection (ie. it is "
             "multiplexed). This isn't handled for now. **FIXME**";
    case accepting_on_an_open_request:
      return "You called async_accept before closing a request.";
    default:
      return "(FastCGI) BOOM!!!";
    }
  }
};

  } // namespace detail

inline const boost::system::error_category& get_fcgi_category()
{
  static detail::fcgi_category instance;
  return instance;
}

static const boost::system::error_category& fcgi_category
  = ::cgi::error::get_fcgi_category();

  } // namespace error
} // namespace cgi

namespace boost {
  namespace system {

    template<> struct is_error_code_enum< ::cgi::error::fcgi_errors>
    {
      BOOST_STATIC_CONSTANT(bool, value = true);
    };

  } // namespace system
} // namespace boost

namespace cgi {
  namespace error {
  
    inline boost::system::error_code make_error_code(fcgi_errors e)
    {
      return boost::system::error_code(
          static_cast<int>(e), get_fcgi_category());
    }

  } // namespace error
} // namespace cgi

#endif // CGI_ERROR_HPP_INCLUDED__

