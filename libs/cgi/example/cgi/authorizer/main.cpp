//                       -- main.hpp --
//
//           Copyright (c) Darren Garvey 2010.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
//
//[fcgi_authoriser

#include <boost/cgi/fcgi.hpp>

#include <fstream>
using namespace std;
namespace fcgi = boost::fcgi;

template<typename Response, typename RequestData, typename String>
void dump_map(Response& resp, RequestData& data, String title)
{
  if (!data.empty())
  {
    typedef typename RequestData::iterator iterator;
    resp<< title << '\n';
    for (iterator iter = data.begin(), end = data.end();
        iter != end;
        ++iter)
    {
      resp<< iter->first << " = " << iter->second << '\n';
    }
  }
}

int handle_request(fcgi::request& req)
{
  req.load(fcgi::parse_all);

  fcgi::response resp;

  if (req.role() == fcgi::authorizer)
  {
    ofstream ofs("../logs/authoriser.log", ios::out | ios::app);
    ofs<< time(NULL) << "::Checked:: " << req.role() << endl;
    dump_map(ofs, req.env, "Environment variables.");
    dump_map(ofs, req.get, "Get variables.");
    dump_map(ofs, req.post, "Post variables.");
    dump_map(ofs, req.cookies, "Cookies.");
    dump_map(ofs, req.uploads, "Uploads.");

    // Note that this (and any other) HTTP header can go either before or after
    // the response contents.
    if (req.env.count("remote_user") && req.env.count("remote_passwd"))
    {
      if (req.env["remote_user"] == "test"
          && req.env["remote_passwd"] == "test")
      {
        resp<< fcgi::header("Status", "200");
        // Send the response to the client that made the request.
        return fcgi::commit(req, resp);
      }
    }
  }
  else
  {
      ofstream ofs("../logs/authenticator.log", ios::out | ios::app);
      ofs<< time(NULL) << "::Checked::" << req.role() << endl;
      dump_map(ofs, req.env, "Environment variables.");
      dump_map(ofs, req.get, "Get variables.");
      dump_map(ofs, req.post, "Post variables.");
      dump_map(ofs, req.cookies, "Cookies.");
      dump_map(ofs, req.uploads, "Uploads.");

      // Note that this (and any other) HTTP header can go either before or after
      // the response contents.
      if (req.env.count("remote_user"))
      {
        if (req.env["remote_user"] == "test")
        {
          resp<< fcgi::header("Status", "200");
          // Send the response to the client that made the request.
          return fcgi::commit(req, resp);
        } else
          resp<< "Wrong parameters.";
      } else
        resp<< "The FastCGI authenticator did not receive the necessary parameters.";
  }
  resp<< "Hello, world"
      << fcgi::header("Status", "401")
      << fcgi::http::unauthorized;

  // Send the response to the client that made the request.
  return fcgi::commit(req, resp);
}

int main()
{
  fcgi::service service;
  fcgi::acceptor acceptor(service);

  int status(0);
  while (!status)
  {
    status = acceptor.accept(&handle_request);
  }
  return status;
}
//]
