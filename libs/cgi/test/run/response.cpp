//                    -- response.hpp --
//
//           Copyright (c) Darren Garvey 2007-2008.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
//
// Test the cgi::response class works as advertised.
//

#define BOOST_TEST_MODULE response_test
#include <boost/test/unit_test.hpp>

#include "boost/cgi/common/response.hpp"

using namespace std;
using namespace boost;

class dummy_request
{
public:

  template<typename ConstBufferSequence>
  size_t write_some(ConstBufferSequence buf, boost::system::error_code ec)
  {
    return write_some(buf);
  }

  template<typename ConstBufferSequence>
  size_t write_some(ConstBufferSequence buf)
  {
    typedef typename ConstBufferSequence::const_iterator iter;
    size_t bytes_written = 0;

    for( iter i = buf.begin(), e = buf.end()
       ; i != e
       ; ++i )
    {
      size_t len( asio::buffer_size(*i) );
      buffer.append( asio::buffer_cast<const char*>(*i), len );
      //buffer += b;
      bytes_written += len;
    }

    return bytes_written;
  }

  std::string buffer;
};

BOOST_AUTO_TEST_CASE( response_test )
{
  using namespace cgi::common;

  dummy_request req;
  string s ("Hello, world.");
  string crlf ("\r\n");
  string ct ("Content-type: text/non-plain");

  {
    response resp;
    resp<< s
        << content_type("text/non-plain");
    resp.flush(req);

    BOOST_CHECK( !req.buffer.empty() );
    BOOST_CHECK_EQUAL( req.buffer, ct + crlf + crlf + s );

    resp<< "...." << double(7.99);
    resp.send(req);
    BOOST_CHECK_EQUAL( req.buffer, ct + crlf + crlf + s + "....7.99" );
  
    req.buffer.clear();
    BOOST_CHECK( req.buffer.empty() );

    resp.flush(req);
    BOOST_CHECK_EQUAL( req.buffer, "....7.99" );

    req.buffer.clear();
    BOOST_CHECK( req.buffer.empty() );
  } // destroy the response (no good way of clearing it now)
  
  response resp;
  resp<< content_type("text/non-plain")
      << s
      << header("Some", "other");

  string expected( ct + crlf + "Some: other" + crlf + crlf + s );

  resp.send(req);
  BOOST_CHECK_EQUAL( req.buffer, expected );

  resp.resend(req); // send it again
  BOOST_CHECK_EQUAL( req.buffer, expected + expected );
}

