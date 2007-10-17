//              -- fcgi/specification.hpp --
//
//            Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_FCGI_SPECIFICATION_HPP_INCLUDED__
#define CGI_FCGI_SPECIFICATION_HPP_INCLUDED__

#include <cinttypes>

// NOTE: CamelCase style mimicks the FastCGI specification
// SEE: http://www.fastcgi.com/devkit/doc/fcgi-spec.html#S8

namespace cgi {
 namespace fcgi {
  namespace detail {

    // Listening socket file number
    const short LISTENSOCK_FILENO = 0;

    /**
     * Number of bytes in a Header. Future versions of the protocol
     * will not reduce this number
     */
    const short HEADER_LEN = 8;

    // Value for version component of Header
    const short VERSION_1 = 1;

    // current version
    const unsigned char VERSION_NUM = (unsigned char)VERSION_1;

    // Values for the type component of Header
    enum request_t { BEGIN_REQUEST     =  1
                   , ABORT_REQUEST     =  2
                   , END_REQUEST       =  3
                   , PARAMS            =  4
                   , STDIN             =  5
                   , STDOUT            =  6
                   , STDERR            =  7
                   , DATA              =  8
                   , GET_VALUES        =  9
                   , GET_VALUES_RESULT = 10
                   , UNKNOWN_TYPE      = 11
                   , MAXTYPE           = UNKNOWN_TYPE
    };

    // a null request id is a management record
    static const unsigned char NULL_REQUEST_ID = 0;

    // Mask for flags component of BeginRequestBody
    static const unsigned char KEEP_CONN = 1;

    // The longest message possible per record
    const std::u_int16_t MAX_MSG_LEN = 65535;

    // Values for role component of BeginRequestBody
    enum role_t { RESPONDER  = 1
                , AUTHORIZER = 2
                , FILTER     = 3
                , ANY
    };

    // Values for protocolStatus component of EndRequestBody
    enum status_t { REQUEST_COMPLETE = 0
                  , CANT_MPX_CONN    = 1
                  , OVERLOADED       = 2
                  , UNKNOWN_ROLE     = 3
    };

    struct Header
    {
    private:
      unsigned char version_;
      unsigned char type_;
      unsigned char requestIdB1_;
      unsigned char requestIdB0_;
      unsigned char contentLengthB1_;
      unsigned char contentLengthB0_;
      unsigned char paddingLength_;
      unsigned char reserved_;
    public:
      Header() { memset(this, 0, sizeof(*this)); }

      Header(request_t t, int id, int len)
            : version_        (VERSION_NUM)
            , type_           ((unsigned char)t)
            , requestIdB1_    ((unsigned char)(id  >> 8) & 0xff)
            , requestIdB0_    ((unsigned char)(id      ) & 0xff)
            , contentLengthB1_((unsigned char)(len >> 8) & 0xff)
            , contentLengthB0_((unsigned char)(len     ) & 0xff)
            , paddingLength_  ((unsigned char)0)
            , reserved_       (0)
      { }

      std::u_int16_t version() const
      {
        return version_;
      }

      std::u_int16_t type() const
      {
        return type_;
      }

      std::u_int16_t request_id() const
      {
        return requestIdB0_ + (requestIdB1_ << 8);
      }

      std::u_int16_t content_length() const
      {
        return contentLengthB0_ + (contentLengthB1_ << 8);
      }

      std::u_int16_t padding_length() const
      {
        return paddingLength_;
      }
      int body_length() const
      {
        return content_length() + padding_length();
      }
    };

    struct BeginRequestBody
    {
      unsigned char roleB1_;
      unsigned char roleB0_;
      unsigned char flags_;
      unsigned char reserved_[5];

      int role() const
      {
        return (roleB1_ << 8 ) + roleB0_;
      }

      unsigned char flags() const
      {
        return flags_;
      }
    };

    struct BeginRequestRecord
    {
      Header header_;
      BeginRequestBody body_;
    };

    struct EndRequestBody
    {
      unsigned char appStatusB3_;
      unsigned char appStatusB2_;
      unsigned char appStatusB1_;
      unsigned char appStatusB0_;
      unsigned char protocolStatus_;
      unsigned char reserved_[3];

      EndRequestBody( std::u_int64_t appStatus
                    , status_t  procStatus
                    )
        : appStatusB3_( (appStatus >> 24) & 0xff )
        , appStatusB2_( (appStatus >> 16) & 0xff )
        , appStatusB1_( (appStatus >>  8) & 0xff )
        , appStatusB0_( (appStatus >>  0) & 0xff )
        , protocolStatus_((unsigned char)procStatus)
      {
        memset(reserved_, 0, sizeof(reserved_));
      }
    };

    struct EndRequestRecord
    {
      Header header_;
      EndRequestBody body_;

      EndRequestRecord( std::u_int16_t id
                      , std::u_int64_t appStatus
                      , status_t  procStatus
                      )
        : header_( END_REQUEST, id, sizeof(EndRequestBody) )
        , body_( appStatus, procStatus )
      {
      }
    };

    struct UnknownTypeBody
    {
      unsigned char type_;
      unsigned char reserved_[7];

      UnknownTypeBody( unsigned char t )
          : type_(t)
      {
        memset(reserved_, 0, sizeof(reserved_));
      }
    };

    struct UnknownTypeRecord
    {
      Header header_;
      UnknownTypeBody body_;

      UnknownTypeRecord( int type )
        : header_( UNKNOWN_TYPE, 0, sizeof(UnknownTypeBody) )
        , body_( (unsigned char)type )
      {
      }
    };
    
  } // namespace detail
 } // namespace fcgi
}// namespace cgi

#endif // CGI_FCGI_SPECIFICATION_HPP_INCLUDED__
