#ifndef BOOST_CGI_EXAMPLES_XCGI_SERVER2_SERVER_HPP_INCLUDED_
#define BOOST_CGI_EXAMPLES_XCGI_SERVER2_SERVER_HPP_INCLUDED_

//[xcgi_server1_server
#include <boost/cgi/cgi.hpp>
#include <boost/cgi/fcgi.hpp>

class Server
{
public:
  Server()
    : service_()
    , acceptor_(service_)
  {
  }

  template<typename Handler>
  int run(Handler handler = Handler())
  {
    return acceptor_.is_cgi() ?
               handle_cgi_request(handler)
             : handle_fcgi_requests(handler);
  }

  template<typename Handler>
  int handle_cgi_request(Handler handler)
  {
    boost::cgi::request req;
    boost::cgi::response resp;
    return handler(req, resp);
  }

  template<typename Handler>
  int handle_fcgi_requests(Handler handler)
  {
    boost::fcgi::request req(service_);

    int ret = 0;
    for (;;) // Handle requests until something goes wrong
             // (an exception will be thrown).
    {
      acceptor_.accept(req);
      boost::fcgi::response resp;
      ret = handler(req, resp);
    }
    return ret;
  }
private:  
  boost::fcgi::service service_;
  boost::fcgi::acceptor acceptor_;
};
//]

#endif // BOOST_CGI_EXAMPLES_XCGI_SERVER2_SERVER_HPP_INCLUDED_

