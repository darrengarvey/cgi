//                -- basic_io_object.hpp --
//
//            Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_BASIC_IO_OBJECT_HPP_INCLUDED__
#define CGI_BASIC_IO_OBJECT_HPP_INCLUDED__

#include <boost/noncopyable.hpp>
#include <boost/asio/io_service.hpp>

namespace cgi {

  template<typename Service>
  class basic_io_object
    : private boost::noncopyable
  {
  public:
    typedef Service                        service_type;

    ::cgi::io_service&
      io_service()
    {
      return service.io_service();
    }

  private:
    typedef typename Service::implementation_type    impl_type;
    typedef typename Service::implementation_type    implementation_type;

  protected:
    explicit basic_io_object(::cgi::io_service& ios)
      : service(boost::asio::use_service<Service>(ios))
    {
      service.construct(implementation);
    }

    ~basic_io_object()
    {
      service.destroy(implementation);
    }

    implementation_type implementation;
    service_type& service;
  };

  /*
  template<>
  class basic_io_object< ::cgi::request> 
    : private boost::noncopyable
  {
  public:
    typedef Service                        service_type;
    typedef typename Service::impl_type    impl_type;

  protected:
    explicit basic_io_object()
    {
      service.construct(impl);
    }

    ~basic_io_object()
    {
      service.destroy(impl);
    }

    impl_type impl;
    service_type service;
    boost::asio::io_service io_service;
  };
  */

} // namespace cgi

#endif // CGI_BASIC_IO_OBJECT_HPP_INCLUDED__
