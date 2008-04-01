#ifndef CGI_DETAIL_FORM_PARSER_HPP_INCLUDED__
#define CGI_DETAIL_FORM_PARSER_HPP_INCLUDED__

#include "boost/cgi/detail/push_options.hpp"

#include <set>
#include <vector>
#include <string>
#include <boost/regex.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/error.hpp>
#include <boost/function.hpp>
#include <boost/system/error_code.hpp>
#include "boost/cgi/common/form_part.hpp"
#include "boost/cgi/basic_client.hpp"

namespace cgi {
 namespace detail {

  /// Destined for better things than an implementation detail (hopefully).
  template<typename RequestImplType>
  class form_parser
  {
  public:
    //typedef
    //  boost::function<
    //    std::size_t (
    //        const mutable_buffers_type&
    //      , boost::system::error_code& )
    //  >
    //callback_type;

    typedef boost::asio::mutable_buffers_1 mutable_buffers_type;
    typedef std::vector<char>   buffer_type;
    typedef ::cgi::common::map  map_type;

    typedef RequestImplType     implementation_type;

    implementation_type& impl_;
    std::size_t& bytes_left_;
    buffer_type::iterator pos_;
    bool& stdin_data_read_;
    std::size_t offset_;

    std::string boundary_marker;
    std::list<std::string> boundary_markers;
    std::vector<common::form_part> form_parts_;

    form_parser(implementation_type& impl)
      : impl_(impl)
      , bytes_left_(impl.characters_left_)
      , stdin_data_read_(impl.stdin_data_read_)
      , offset_(0)
    {
    }
    /*
    form_parser(std::size_t& cl, buffer_type& buf, map_type& m
               , unsigned int& client_bytes, bool& sp, bool& sdr
               , std::string ct, callback_type const & cb)
      : bytes_left_(cl)
      , buf_(buf)
      , map_(m)
      , client_bytes_left_(client_bytes)
      , stdin_parsed_flag_(sp)
      , stdin_data_read_(sdr)
      , offset_(0)
      , content_type_(ct)
      //, client_(c)
      , callback_(cb)
    {

    }
    */

    mutable_buffers_type prepare(std::size_t size)
    {
      std::size_t bufsz(impl_.buf_.size());
      impl_.buf_.resize(bufsz + size);
      return boost::asio::buffer(&impl_.buf_[bufsz], size);
    }
      
    std::string buffer_string()
    {
      return std::string(impl_.buf_.begin() + offset_, impl_.buf_.end());
    }
    
    boost::system::error_code 
      parse(boost::system::error_code& ec)
    {
      parse_boundary_marker(ec);
      //parse_one_form_part(impl, ec);
      
      move_to_start_of_first_part(ec);
      
      if (ec == boost::asio::error::eof) {
        return boost::system::error_code();
      }else
      if (ec)
        return ec;

      do {
        parse_form_part(ec);
      }while( !impl_.stdin_parsed_
           && impl_.client_.bytes_left() != 0
           );//&& ec != boost::asio::error::eof );

      return ec;
    }

    boost::system::error_code
      parse_form_part(boost::system::error_code& ec)
    {
      if (!parse_form_part_meta_data(ec)
      &&  !parse_form_part_data(ec))
        return ec;

      return ec;
    }

    boost::system::error_code
      parse_form_part_data(boost::system::error_code& ec)
    {
      std::string regex("^(.*?)" // the data
                        "\\x0D\\x0A" // CR LF
                        "--" "(");
      if (boundary_markers.size() > 1)
      {
        std::list<std::string>::iterator i(boundary_markers.begin());
        regex = regex + "(?:" + *i + ")";
        ++i;
        for(; i != boundary_markers.end(); ++i)
        {
          regex = regex + "|(?:" + *i + ")";
        }
      }
      else
      {
        regex += *boundary_markers.begin();
      }
      
      regex += ")(--)?[ ]*\\x0D\\x0A";
      boost::regex re(regex);
      
      typedef buffer_type::iterator buffer_iter;

      boost::match_results<buffer_iter> matches;

      std::size_t offset = offset_;

      //int runs = 0;
      buffer_iter begin(impl_.buf_.begin() + offset);
      buffer_iter end(impl_.buf_.end());

      for(;;)
      {
        if (!boost::regex_search(begin, end, matches, re
                                , boost::match_default
                                | boost::match_partial))
        {
          return boost::system::error_code(345, boost::system::system_category);
        }
        else
        {
          if (matches[1].matched)
          {
            form_parts_.back().buffer_
             // = boost::range_iterator<;
             = std::make_pair(matches[1].first, matches[1].second);
            impl_.post_vars_[form_parts_.back().name] = matches[1];
            offset_ = offset + matches[0].length();
            pos_ = matches[0].second;

            if (matches[3].matched)
              impl_.stdin_parsed_ = true;
            return ec;
          }
          else
          {
            std::size_t bytes_read = impl_.client_.read_some(prepare(64), ec);
            
            if (bytes_read == 0)
            {
              stdin_data_read_ = true;
              return ec;
            }

            begin = impl_.buf_.begin() + offset;
            end = impl_.buf_.end();

            if (ec)
              return ec;
          }
        }
      }

      return ec;
    }

    boost::system::error_code
      parse_form_part_meta_data(boost::system::error_code& ec)
    {
      // Oh dear this is ugly. The move to Boost.Spirit will have to be sooner than planned.
      // (it's a nested, recursive pattern, which regexes don't suit, apparently)
      boost::regex re(  "(?:"         // [IGNORE] the line may be empty, as meta-data is optional
                          "^"
                          "([-\\w]+)" // name
                          ":[ ^]*"       // separator
                          "([-/\\w]+)" // optional(?) value
                          ""
                          "(?:"
                            ";"
                            "[ ]*"    // additional name/value pairs (don't capture)
                            "([-\\w]+)" // name
                            "[ \\x0D\\x0A]*=[ \\x0D\\x0A]*"       // separator
                            "(?:\"?([-.\\w]*)\"?)" // value may be empty
                          ")?"
                          "(?:"
                            ";"
                            "[ ]*"    // additional name/value pairs (don't capture)
                            "([-\\w]+)" // name
                            "[ \\x0D\\x0A]*=[ \\x0D\\x0A]*"       // separator
                            "(?:\"?([-.\\w]*)\"?)" // value may be empty
                          ")?"        // mark the extra n/v pairs optional
                          "\\x0D\\x0A"
                        ")"
                        "(?:"
                          "([-\\w]+)" // name
                          ":[ ^]*"       // separator
                          "([-/\\w]+)" // optional(?) value
                          ""
                          "(?:"
                            ";"
                            "[ ]*"    // additional name/value pairs (don't capture)
                            "([-\\w]+)" // name
                            "[ \\x0D\\x0A]*=[ \\x0D\\x0A]*"       // separator
                            "(?:\"?([-.\\w]*)\"?)" // value may be empty
                          ")?"
                          "(?:"
                            ";"
                            "[ ]*"    // additional name/value pairs (don't capture)
                            "([-\\w]+)" // name
                            "[ \\x0D\\x0A]*=[ \\x0D\\x0A]*"       // separator
                            "(?:\"?([-.\\w]*)\"?)" // value may be empty
                          ")?"        // mark the extra n/v pairs optional
                          "\\x0D\\x0A"    // followed by the end of the line
                        ")?" 
                      "(\\x0D\\x0A)");     // followed by the 'header termination' line

      typedef buffer_type::iterator buffer_iter;

      boost::match_results<buffer_iter> matches;

      std::size_t offset = offset_;
      pos_ = impl_.buf_.begin();
      int runs = 0;

      std::size_t bytes_read = 0;
      for(;;)
      {
        buffer_iter begin(impl_.buf_.begin() + offset);
        buffer_iter end(impl_.buf_.end());
        
        if (!boost::regex_search(begin, end, matches, re
                                , boost::match_default | boost::match_partial))
        {
          impl_.stdin_parsed_ = true;
          return ec;
        }
        if (matches[0].matched)
        {
          common::form_part part;
          for ( unsigned int i = 1
              ; i < matches.size() 
               && matches[i].matched
               && !matches[i].str().empty()
              ; i+=2)
          {
            if (matches[i].str() == "name")
            {
              part.name = matches[i+1];
            }
            else
            {
              part.meta_data_[matches[i]]
                = std::make_pair(matches[i+1].first, matches[i+1].second);
            }
            form_parts_.push_back(part);
         }
          
         if (matches[13].str() == "\r\n")
         {
           offset_ = offset + matches[0].length();
           offset += matches[0].length();
           pos_ = matches[0].second;
          
           return ec;
         }
         else
         {
           throw std::runtime_error("Invalid POST data (header wasn't terminated as expected)");
         }
         
        }else{
          bytes_read = impl_.client_.read_some(prepare(64), ec);
          if (ec)
            return ec;
          if (++runs > 40)
          {
            std::cerr<< "Done 40 runs; bailing out" << std::endl;
            break;
          }
       }
      }

      return ec;
    }

    boost::system::error_code
      move_to_start_of_first_part(boost::system::error_code& ec)
    {
      boost::regex re("((?:.*)?"   // optional leading characters
                      //"(?:\\x0D\\x0A)|^" // start of line
                      "[\\x0D\\x0A^]*?"
                      "("
                        "--" + boundary_markers.front() + // two dashes and our marker
                      ")"
                      "(--)?" // optional two dashes (not sure if this is allowed)
                      " *\\x0D\\x0A)");
                                        // on the first marker.

      typedef buffer_type::iterator buffer_iter;
      //std::cerr<< "Regex := " << re << std::endl;

      boost::match_results<buffer_iter> matches;

      // get data into our buffer until we reach the first boundary marker.
      int runs = 0;
      std::size_t offset = 0;
      std::size_t bytes_read = 0;
      for(;;)
      {
        bytes_read = impl_.client_.read_some(prepare(32), ec);
        if (ec || (bytes_read == 0))
          return ec;
        buffer_iter begin(impl_.buf_.begin());// + offset);
        buffer_iter end(impl_.buf_.end());
        if (!boost::regex_search(begin, end //impl.buf_.begin(), impl.buf_.end()
                                , matches, re, boost::match_default | boost::match_partial))
        {
          offset = impl_.buf_.size();
          continue;
        }
        else
        {
          if (matches[2].matched)
          {
            impl_.buf_.erase(impl_.buf_.begin(), matches[0].second);
            offset_ = 0;
            pos_ = impl_.buf_.begin();
            return ec;
          }
          else
          {
            if (++runs > 10)
              return ec;
            continue;
          }
        }
      }
      // skip that line and then erase the buffer
      return ec;
    }

    boost::system::error_code
      parse_one_form_part(boost::system::error_code& ec)
    {
      // continuously read data while parsing it until one complete form part has
      // been read.
      // Note, this may mean recursing into embedded sub-parts if necessary, but
      // still only the first *complete* part/sub-part would be read.
      //boost::regex re("\r\n--" + boundary_marker + " +

      for(;;)
      {
        //if (impl.client_->read_some(impl.prepare(1024), ec))
        //  return ec;
        //boost::asio::read_until(impl.client_-> impl.buffer_, boundary_marker, ec);
        break;


      }

      return ec;
    }

    /// Get the boundary marker from the CONTENT_TYPE header.
    boost::system::error_code
      parse_boundary_marker(boost::system::error_code& ec)
    {
      // get the meta-data appended to the content_type
      std::string content_type_(impl_.env_vars_["CONTENT_TYPE"]);
      //BOOST_ASSERT(!content_type.empty());

      boost::regex re("; ?boundary=\"?([^\"\n\r]+)\"?");
      boost::smatch match_results;
      if (!boost::regex_search(content_type_, match_results, re))
        return boost::system::error_code(666, boost::system::system_category);

      boundary_marker = match_results[1].str();
      // New boundary markers are added to the front of the list.
      boundary_markers.push_front(match_results[1].str());

      return ec;
    }

  private:
    //std::set<std::string> boundary_markers_;
    //std::vector<common::form_part> form_parts_;

    //buffer_type& buffer_;

    //callback_type callback_;
  };

 } // namespace detail
} // namespace cgi

#endif // CGI_DETAIL_FORM_PARSER_HPP_INCLUDED__

