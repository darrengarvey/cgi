#include <boost/cgi/scgi.hpp>
#include <boost/system/system_error.hpp>
#include <fstream>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/program_options/environment_iterator.hpp>

using namespace cgi::scgi;
using namespace std;

#define LOG_FILE "/var/www/log/scgi_hello_world.txt"
//#define LOG_FILE "scgi_hello_world.txt"

template<typename OStream>
void accept_handler(OStream& os, const boost::system::error_code& ec)
{
  os<< "Accepted connection ok!" << endl;
}


int main()
{
  std::cerr<< "[hw] Started SCGI daemon (see " LOG_FILE " for more)..." << endl;
try {
  ofstream of(LOG_FILE);
  if (!of) std::cerr<< "Couldn't open file: \"" LOG_FILE "\"." << endl;
  of<< boost::posix_time::second_clock::local_time() << endl;
  of<< "Going to start acceptor" << endl;
  service s;
  acceptor a(s, 9499);

  of<< "Writing environment variables..." << endl;
  for(boost::environment_iterator i(environ), e; i != e; ++i)
  {
    of<< i->first << ", " << i->second << endl;
  }
  of<< "--------------------------------" << endl;
  request req(s);
  of<< "Made request" << endl;
  boost::system::error_code ec;
  boost::asio::ip::tcp::endpoint ep(boost::asio::ip::tcp::v4(), 9499);
  of<< "ep.address := " << ep.address() << endl;
  //a.open(boost::asio::ip::tcp::v4());
  //of<< (ec ? "Couldn't open: " + ec.message() : "Opened ok") << endl; 
  //a.listen(boost::asio::socket_base::max_connections, ec);
  //of<< "[a] Listening: " << ec.message() << endl;
  //of<< "[a] address: " << a.local_endpoint().address().to_string()
  //  << ":" << a.local_endpoint().port() << endl;
  a.assign(boost::asio::ip::tcp::v4(), 0, ec);
  of<< "[a] Assigned: " << ec.message() << endl;
  //a.bind(, ec);
  //of<< "[a] Bind outcome: " << ec.message() << endl;
  while(!a.accept(req, ec))
  {
  of<< "Called accept" << endl;
  //boost::asio::io_service ios;
  //using boost::asio::ip::tcp;
  //tcp::socket sock(ios);
  if (ec) { of<< "[hw] accept error: " << ec.message() << endl; return 13; }
  //cout<< "Hello there." << endl;
  //of<< "acceptor.is_open() := " << a.is_open() << endl;
  of<< "request.is_open() := " << req.is_open() << endl;
  of<< "request.client().is_open() := " << req.client().is_open() << endl;
  of<< "next_layer stuff := " << req.client().connection()->next_layer().is_open() << endl;

  //boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address::from_string("127.0.0.1"), 9499);
  //boost::asio::ip::tcp::acceptor a2(s.io_service(), ep);
  //a2.open(ep.protocol());
  //of<< "Opened a2" << endl;
  //a2.listen(boost::asio::socket_base::max_connections, ec);
  //of<< "a2 listening: " << ec.message() << endl;
  //boost::asio::ip::tcp::socket s(s.io_service());
  //a2.accept(req.client().connection()->next_layer(), ec);
  //a.async_accept(s, boost::bind(&accept_handler<std::ofstream>, boost::ref(of), boost::asio::placeholders::error));
  of<< "Accept had result: " << ec.message() << endl;
  boost::asio::streambuf buf;
  of<< "Start log:" << endl;

  req.load(ec, true);//cgi::parse_all);
  if (ec) { of<< "Couldn't load request: " << ec.message() << endl; return 14; }
  //for (;;)
  //{
    size_t br = req.client().read_some(buf.prepare(64), ec);
    of<< "Client wrote " << br << " bytes." << endl
      ;//<< string(boost::asio::buffer_cast<const char *>(buf.data()),
       //         boost::asio::buffer_size(buf.data())) << endl;
    if (ec) { of<< "read error: " << ec.message() << endl; return 5; }
  //}
  sleep(4);
  }
  of<< "Accept error (shutting down): " << ec.message() << endl;
  return 0;
}catch(boost::system::system_error& se){
  cerr<< "[hw] System error: " << se.what() << endl;
  return 1313;
}catch(exception& e){
  cerr<< "[hw] Exception: " << e.what() << endl;
  return 666;
}catch(...){
  cerr<< "[hw] Uncaught exception!" << endl;
  return 667;
}
}
