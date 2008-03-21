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

//#include <inttypes.h>
#include <boost/cstdint.hpp>

// NOTE: CamelCase style mimicks the FastCGI specification
// SEE: http://www.fastcgi.com/devkit/doc/fcgi-spec.html#S8

namespace cgi {
 namespace fcgi {
  namespace spec_detail {

    // Listening socket file number
    const short LISTENSOCK_FILENO = 0;

    /**
     * Number of bytes in a Header. Future versions of the protocol
     * will not reduce this number
     */
    const short HEADER_LEN = 8;

#if 1
    // Value for version component of Header
    const short BOOST_CGI_FASTCGI_VERSION_1 = 1;

    // current version
    const unsigned char VERSION_NUM
      = (unsigned char)BOOST_CGI_FASTCGI_VERSION_1;

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
    const boost::uint16_t MAX_MSG_LEN = 65535;

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
    //private:
      /// The underlying type of a FastCGI header.
      /**
       * To guarantee the header is laid out exactly as we want, the
       * structure must be a POD-type (see http://tinyurl.com/yo9eav).
       */
      struct implementation_type
      {
        unsigned char version_;
        unsigned char type_;
        unsigned char requestIdB1_;
        unsigned char requestIdB0_;
        unsigned char contentLengthB1_;
        unsigned char contentLengthB0_;
        unsigned char paddingLength_;
        unsigned char reserved_;
      } impl;

    public:
      Header() { memset(static_cast<void*>(&this->impl), 0, sizeof(this->impl)); }

      Header(request_t t, int id, int len)
      {
        reset(t, id, len);
      }

      void reset(request_t t, int id, int len)
      {
        impl.version_         = (VERSION_NUM);
        impl.type_            = ((unsigned char)t);
        impl.requestIdB1_     = ((unsigned char)(id  >> 8) & 0xff);
        impl.requestIdB0_     = ((unsigned char)(id      ) & 0xff);
        impl.contentLengthB1_ = ((unsigned char)(len >> 8) & 0xff);
        impl.contentLengthB0_ = ((unsigned char)(len     ) & 0xff);
        impl.paddingLength_   = ((unsigned char)0);
        impl.reserved_        = (0);
      }

      boost::uint16_t version() const
      {
        return impl.version_;
      }

      boost::uint16_t type() const
      {
        return impl.type_;
      }

      boost::uint16_t request_id() const
      {
        return impl.requestIdB0_ + (impl.requestIdB1_ << 8);
      }

      boost::uint16_t content_length() const
      {
        return impl.contentLengthB0_ + (impl.contentLengthB1_ << 8);
      }

      boost::uint16_t padding_length() const
      {
        return impl.paddingLength_;
      }

      int body_length() const
      {
        return content_length() + padding_length();
      }
    };

    class BeginRequestBody
    {
      /// The underlying type of a BeginRequestBody sub-header.
      /**
       * To guarantee the header is laid out exactly as we want, the
       * structure must be a POD-type (see http://tinyurl.com/yo9eav).
       */
      struct implementation_type
      {
        unsigned char roleB1_;
        unsigned char roleB0_;
        unsigned char flags_;
        unsigned char reserved_[5];
      } impl;

    public:

      int role() const
      {
        return (impl.roleB1_ << 8 ) + impl.roleB0_;
      }

      unsigned char flags() const
      {
        return impl.flags_;
      }
    };

    struct BeginRequestRecord
    {
      Header header_;
      BeginRequestBody body_;
    };

    class EndRequestBody
    {
      /// The underlying type of an EndRequestBody sub-header.
      /**
       * To guarantee the header is laid out exactly as we want, the
       * structure must be a POD-type (see http://tinyurl.com/yo9eav).
       */
      struct implementation_type
      {
        unsigned char appStatusB3_;
        unsigned char appStatusB2_;
        unsigned char appStatusB1_;
        unsigned char appStatusB0_;
        unsigned char protocolStatus_;
        unsigned char reserved_[3];
      } impl;

    public:
      EndRequestBody() {}

      EndRequestBody( boost::uint64_t appStatus
                    , status_t  procStatus
                    )
      {
        reset(appStatus, procStatus);
      }

      void reset( boost::uint64_t appStatus, status_t procStatus)
      {
        impl.appStatusB3_    = ( (appStatus >> 24) & 0xff );
        impl.appStatusB2_    = ( (appStatus >> 16) & 0xff );
        impl.appStatusB1_    = ( (appStatus >>  8) & 0xff );
        impl.appStatusB0_    = ( (appStatus >>  0) & 0xff );
        impl.protocolStatus_ = ((unsigned char)procStatus);

        memset(impl.reserved_, 0, sizeof(impl.reserved_));
      }
    };

    class EndRequestRecord
    {
      /// The underlying type of an EndRequestRecord sub-header.
      /**
       * To guarantee the header is laid out exactly as we want, the
       * structure must be a POD-type (see http://tinyurl.com/yo9eav).
       */
      struct implementation_type
      {
        Header header_;
        EndRequestBody body_;
      } impl;

    public:
      EndRequestRecord( boost::uint16_t id
                      , boost::uint64_t appStatus
                      , status_t  procStatus
                      )
      {
        impl.header_.reset( END_REQUEST, id, sizeof(EndRequestBody) );
        impl.body_.reset( appStatus, procStatus );
      }
    };

    class UnknownTypeBody
    {
      /// The underlying type of an UnknownTypeBody sub-header.
      /**
       * To guarantee the header is laid out exactly as we want, the
       * structure must be a POD-type (see http://tinyurl.com/yo9eav).
       */
      struct implementation_type
      {
        unsigned char type_;
        unsigned char reserved_[7];
      } impl;

    public:
      UnknownTypeBody()
      {
      }

      UnknownTypeBody( unsigned char t )
      {
        reset(t);
      }

      void reset(unsigned char t)
      {
        impl.type_ = t;
        memset(impl.reserved_, 0, sizeof(impl.reserved_));
      }

      unsigned char type() const { return impl.type_; }
    };

    class UnknownTypeRecord
    {
      /// The underlying type of a UnknownTypeRecord sub-header
      /**
       * To guarantee the header is laid out exactly as we want, the
       * structure must be a POD-type (see http://tinyurl.com/yo9eav).
       */
      struct implementation_type
      {
        Header header_;
        UnknownTypeBody body_;
      } impl;

    public:
      UnknownTypeRecord( int type )
      {
        impl.header_.reset( UNKNOWN_TYPE, 0, sizeof(UnknownTypeBody) );
        impl.body_.reset( (unsigned char)type ); // not sure why this is C-style
      }
    };
    
#endif
  } // namespace detail  
  
  namespace specification {

#include <boost/mpl/int.hpp>

    struct max_packet_size
      : boost::mpl::int_<65535>
    {};

    struct header_length
      : boost::mpl::int_<8>
    {};
    
    struct listensock_fileno
      : boost::mpl::int_<0>
    {};

    static const unsigned char keep_connection = 1;
    
    struct null_request_id
      : boost::mpl::int_<0>
    {};

    template<typename Array>
    int get_version(Array& a) { return static_cast<int>(a[0]); }
    
    template<typename Array>
    spec_detail::request_t get_type(Array& a)
    {
      return static_cast<spec_detail::request_t>(a[1]);
    }
    
    template<typename Array>
    boost::uint16_t get_request_id(Array& a)
    {
      return (a[2] << 8) + a[3];
    }

    template<typename Array>
    boost::uint16_t get_content_length(Array& a)
    {
      return (a[4] << 8) + a[5];
    }

    template<typename Array>
    boost::uint16_t get_padding_length(Array& a)
    {
      return a[6];
    }

    template<typename Array>
    boost::uint16_t get_length(Array& a)
    {
      return get_content_length(a) + get_padding_length(a);
    }

    struct request_type
    {
      template<typename Array>
      static std::string to_string(Array& a)
      {
        switch(a[1])
        {
        case 0:
          return "ADMIN_REQUEST";
        case 1:
          return "BEGIN_REQUEST";
        case 2:
          return "ABORT_REQUEST";
        case 3:
          return "END_REQUEST";
        case 4:
          return "PARAMS";
        case 5:
          return "STDIN";
        case 6:
          return "STDOUT";
        case 7:
          return "STDERR";
        case 8:
          return "DATA";
        case 9:
          return "GET_VALUES";
        case 10:
          return "GET_VALUES_RESULT";
        case 11:
          return "UNKNOWN_TYPE";
        default:
          return "***ERROR-INVALID-TYPE***";
        }
      }
    };

    struct begin_request
      : boost::mpl::int_<1>
    {
      struct body
      {
        typedef boost::mpl::int_<8> size;
      };

      template<typename Array>
      static spec_detail::role_t
        get_role(Array& a)
      {
        return static_cast<spec_detail::role_t>( (a[0] << 8) + a[1] );
      }

      template<typename Array>
      static unsigned char
        get_flags(Array& a)
      {
        return a[2];
      }
      
      //typedef spec_detail::BEGIN_REQUEST type;

    };

    struct role_type
    {
      template<typename Array>
      static std::string to_string(Array& a)
      {
        using namespace spec_detail;
        switch(begin_request::get_role(a))
        {
        case RESPONDER:
          return "RESPONDER";
        case AUTHORIZER:
          return "AUTHORISER";
        case FILTER:
          return "FILTER";
        default:
          return "ANY";
        }
      }
    };
    //using namespace ::cgi::fcgi::detail;
  }

  namespace spec = specification;

 } // namespace fcgi
}// namespace cgi

#endif // CGI_FCGI_SPECIFICATION_HPP_INCLUDED__
