//          -- fcgi/acceptor_service_impl.hpp --
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_FCGI_TRANSPORT_SELECTION_HPP_INCLUDED__
#define CGI_FCGI_TRANSPORT_SELECTION_HPP_INCLUDED__

#include "boost/cgi/detail/push_options.hpp"

#include <boost/asio.hpp>

BOOST_CGI_NAMESPACE_BEGIN

  namespace detail {
    struct transport {
      enum type {
        pipe = 0,
        socket
      };
    };


#if    BOOST_WINDOWS
    inline transport::type transport_type() {
      HANDLE hStdInput = ::GetStdHandle(STD_INPUT_HANDLE);
      if (hStdInput != INVALID_HANDLE_VALUE) 
      {
        SOCKET socket = reinterpret_cast<SOCKET&>(hStdInput);
        SOCKADDR_STORAGE sockStor;
        int addrInfoSize = sizeof(sockStor);
        ::ZeroMemory(&sockStor, sizeof(sockStor));
        if (::getsockopt(socket, SOL_SOCKET, SO_BSP_STATE, reinterpret_cast<char*>(&sockStor), &addrInfoSize) == 0)
          return transport::socket;
        else
          return transport::pipe;
      }

      return transport::pipe;
    }

    inline boost::asio::ip::tcp::endpoint detect_endpoint() {
      HANDLE hStdInput = ::GetStdHandle(STD_INPUT_HANDLE);
      if (hStdInput != INVALID_HANDLE_VALUE)
      {
        SOCKET socket = reinterpret_cast<SOCKET&>(hStdInput);
        SOCKADDR_STORAGE sockStor;
        int addrInfoSize = sizeof(sockStor);
        ::ZeroMemory(&sockStor, sizeof(sockStor));
        if (::getsockopt(socket, SOL_SOCKET, SO_BSP_STATE, reinterpret_cast<char*>(&sockStor), &addrInfoSize) == 0)
        {
          SOCKADDR *pSockAddr = nullptr;
          CSADDR_INFO *pAddrInfo = reinterpret_cast<CSADDR_INFO*>(&sockStor);
          if (pAddrInfo->LocalAddr.lpSockaddr != nullptr)
            pSockAddr = pAddrInfo->LocalAddr.lpSockaddr;
          else if (pAddrInfo->RemoteAddr.lpSockaddr != nullptr)
            pSockAddr = pAddrInfo->RemoteAddr.lpSockaddr;

          if (pSockAddr != nullptr)
          {
            switch (pSockAddr->sa_family)
            {
              case AF_INET:
              {
                sockaddr_in* sa = reinterpret_cast<sockaddr_in*>(pSockAddr);
                boost::asio::ip::address_v4 v4(ntohl(sa->sin_addr.S_un.S_addr));
                boost::asio::ip::address addr(v4);
                return boost::asio::ip::tcp::endpoint(addr, ntohs(sa->sin_port));
              }

              case AF_INET6:
              {
                sockaddr_in6* sa = reinterpret_cast<sockaddr_in6*>(pSockAddr);
                boost::asio::ip::address_v6 v6(reinterpret_cast<boost::asio::ip::address_v6::bytes_type const&>(sa->sin6_addr.u.Byte), ntohl(sa->sin6_scope_id));
                boost::asio::ip::address addr(v6);
                return boost::asio::ip::tcp::endpoint(addr, ntohs(sa->sin6_port));
              }
            }
          }
        }
      }
      return boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), 0);
    }
#else
       inline transport::type transport_type() {
           return transport::socket;
       }

       inline boost::asio::ip::tcp::endpoint detect_endpoint() {
         return boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), 0);
       }

#endif // defined (BOOST_WINDOWS)

   } // namespace detail
BOOST_CGI_NAMESPACE_END

#include "boost/cgi/detail/pop_options.hpp"

#endif CGI_FCGI_TRANSPORT_SELECTION_HPP_INCLUDED__