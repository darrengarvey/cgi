#ifndef CGI_DETAIL_FORM_PARSER_HPP_INCLUDED__
#define CGI_DETAIL_FORM_PARSER_HPP_INCLUDED__

#include "boost/cgi/detail/push_options.hpp"

#include <set>
#include <vector>
#include <string>
#include <boost/asio/buffer.hpp>
#include <boost/function.hpp>
#include "boost/cgi/common/form_part.hpp"

namespace cgi {
 namespace detail {

  /// Destined for greater things than an implementation detail.
  class form_parser
  {
  public:
    typedef
      boost::function<
        std::size_t (
            const boost::asio::mutable_buffer&
          , boost::system::error_code& )
      >
    callback_type;

    typedef std::vector<char> buffer_type;

    form_parser()
    {

    }
    
  private:
    std::set<std::string> boundary_markers_;
    std::vector<common::form_part> form_parts_;

    //buffer_type& buffer_;

    callback_type callback_;
  };

 } // namespace detail
} // namespace cgi

#endif // CGI_DETAIL_FORM_PARSER_HPP_INCLUDED__

