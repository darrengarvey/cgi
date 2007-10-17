#include <boost/thread.hpp>
#include <boost/enable_shared_from_this.hpp>

#include <boost/cgi/fcgi.hpp>

/************************************
NOTE: this doesn't work for now, because FastCGI support isn't
      built in yet.

      This is what a FastCGI program should look like though.

      The request will be handled in `sub_main`
*************************************/

namespace cgi {

  class fcgi_threadpool_server
    : public boost::enable_shared_from_this<fcgi_threadpool_server>
  {
  public:
    typedef fcgi_threadpool_server                    type;
    typedef cgi::fcgi_service                         service_type;
    typedef cgi::fcgi_acceptor                        acceptor_type;
    typedef boost::function<int (cgi::fcgi_request&)> handler_type;

    /// Constructor (could use Boost.Parameter)
    fcgi_threadpool_server(int num_threads, const handler_type& handler)
      : service_()
      , acceptor_(service_)
      , num_threads_(num_threads)
      , handler_(handler)
      , thread_group()
    {
      start_accept();
    }

    /// Run the io_service(s) that service_ uses
    /**
     * This call will block until all of the service_'s work has been completed.
     */
    void run()
    {
      for (int i = num_threads_; i != 0; i++)
      {
        thread_group_.create_thread
          (boost::bind(&io_service::run, boost::ref(service_.io_service())));
      }
      thread_group_.join_all();
    }

    /// Stop the server
    /**
     * All outstanding async operations will be cancelled when the acceptor
     * is closed. The server should not be used again until after a call to
     * `reset`.
     */
    void stop()
    {
      acceptor_.close();
    }

    /// Start an async accept on a new request object
    void start_accept()
    {
      request_type::pointer new_request = request_type::create(service_);
      acceptor_.async_accept(*new_request
                            , boost::bind(&type::handle_accept
                                         , shared_from_this(), new_request
                                         , boost::arg<1>));
    }

    /// Asynchronously load the request and call `start_accept` again
    void handle_accept(request_type::pointer req, boost::system::error_code& ec)
    {
      if (!ec)
      {
        req->async_load(boost::bind(&type::handle_load, req, boost::arg<1>));
        start_accept();
      }
    }

    /// If no errors, call handler_(req) in a background thread
    void handle_load(request_type::pointer req, boost::system::error_code& ec)
    {
      if (!ec)
      {
        service_.post(boost::bind(&handler_type, handler_
                                 , boost::ref(*new_request)));
      }
    }

  private:
    service_type service_;
    acceptor_type acceptor_;

    int num_threads_;
    handler_type handler_;
    boost::thread::thread_group thread_group_;
  };

} // namespace cgi


/// The function that handles a request
int sub_main(cgi::fcgi_request& req)
{
  // ...
  // Use the request here
  // ...

  // Close the request; return "200 OK" to the client and '0' to the server
  return req.close(cgi::http::ok, 0);
}


int main()
{
  // Create a server with 10 threads handling requests
  fcgi_threadpool_server server(10, &sub_main);
  
  // Run the server
  server.run();

  return 0;
}
