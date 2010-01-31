//                 -- server1/server.hpp --
//
//           Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
//

// standard includes
#include <set>
// external includes
#include <boost/function.hpp>
// internal includes
#include <boost/cgi/fcgi.hpp>

/// The server is used to abstract away protocol-specific setup of requests.
/**
 * This server only works with FastCGI. Later examples will show you how to
 * make a protocol-independent server.
 *
 * Later examples will demonstrate making protocol-independent servers.
 * (**FIXME**)
 */
class server4
{
public:
  typedef boost::fcgi::request                  request_type;
  typedef boost::function<
            int ( request_type&
                , boost::system::error_code&)
          >                                     function_type;

  server4(const function_type& handler)
    : handler_(handler)
    , service_()
    , acceptor_(service_)
  {}

  int run()
  {
    // Create a new request (on the heap - uses boost::shared_ptr<>).
    request_type::pointer new_request = request_type::create(service_);
    // Add the request to the set of existing requests.
    requests_.insert(new_request);

    int ret(0);
    for (;;)
    {
      boost::system::error_code ec;

      acceptor_.accept(*new_request, ec);

      if (ec)
      {
        std::cerr<< "Error accepting: " << ec.message() << std::endl;
        return 5;
      }

      // Load in the request data so we can access it easily.
      // Read and parse POST data.
      new_request->load(boost::fcgi::parse_post, ec);

      // Call the request handler and capture the result of handling
      // the request.
      ret = handler_(*new_request, ec);

      // A non-zero return value indicates an error.
      if (ret)
        break;
    }
    return ret;
  }

private:
  function_type handler_;
  boost::fcgi::service service_;
  boost::fcgi::acceptor acceptor_;
  std::set<request_type::pointer> requests_;
};
