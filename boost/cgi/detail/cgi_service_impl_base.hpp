#ifndef CGI_CGI_SERVICE_IMPL_BASE_HPP_INCLUDED__
#define CGI_CGI_SERVICE_IMPL_BASE_HPP_INCLUDED__

#include "boost/cgi/detail/push_options.hpp"

#include <string>
#include <cstdlib>
#include <iostream>
#include <boost/assert.hpp>
#include <boost/regex.hpp>
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/system/error_code.hpp>
#include <boost/algorithm/string/find.hpp>

#include "boost/cgi/map.hpp"
#include "boost/cgi/basic_client.hpp"
#include "boost/cgi/role_type.hpp"
#include "boost/cgi/status_type.hpp"
#include "boost/cgi/detail/extract_params.hpp"
#include "boost/cgi/detail/save_environment.hpp"
#include "boost/cgi/common/form_part.hpp"
#include "boost/cgi/detail/throw_error.hpp"

#include "boost/cgi/common/form_parser.hpp"

namespace cgi {

 namespace detail {


 } // namespace detail

  using std::cerr; // **FIXME**
  using std::endl; // **FIXME**


  template<typename RequestImplType>
  class cgi_service_impl_base
  {
  public:
    //typedef RequestImplType     implementation_type;
    typedef ::cgi::common::map          map_type;

    cgi_service_impl_base()
    {
    }

    template<typename T>
    cgi_service_impl_base(T&)
    {
    }

    struct implementation_type
      : RequestImplType
    {
      typedef boost::asio::const_buffers_1          const_buffers_type;
      typedef boost::asio::mutable_buffers_1        mutable_buffers_type;
      typedef typename RequestImplType::client_type client_type;
      typedef std::vector<char>                     buffer_type;
      //typedef std::list<std::string>::iterator      marker_list_type;

      implementation_type()
        : //buffer_()
        //, istream_(&buffer_)
          pos_()
        , offset_(0)
        , fp_(NULL)
      {
      }

      client_type client_;

      std::string boundary_marker;
      std::list<std::string> boundary_markers;
      //std::vector<char> data_buffer_;
      // The number of characters left to read (ie. "content_length - bytes_read")
      std::size_t characters_left_;
      
      //std::vector<char>::iterator read_pos_;
      //boost::asio::streambuf buffer_;
      //std::istream istream_;

      buffer_type buf_;
      typename buffer_type::iterator pos_;
      std::size_t offset_;
      //boost::sub_match<typename buffer_type::iterator> offset_;
      std::vector<common::form_part> form_parts_;
      
      detail::form_parser* fp_;

      /*
      mutable_buffers_type prepare(typename buffer_type::iterator& pos, std::size_t size)
      {
        std::size_t bufsz( buf_.size() );

        // Reserve more space if it's needed.
        // (this could be safer, referencing this against CONTENT_LENGTH)
        if ( (bufsz + size) >= buf_.capacity() )
        {
          buf_.resize(bufsz + size);
        }

        return boost::asio::buffer(&(buf_.begin()), size);
  //      return boost::asio::buffer(&buf_[bufsz], size);
      }
      */

      mutable_buffers_type prepare(std::size_t size)
      {
        
        std::size_t bufsz( buf_.size() );
        cerr<< "bufsz    = " << bufsz << endl;

        // Reserve more space if it's needed.
        // (this could be safer, referencing this against CONTENT_LENGTH)
        //if ( (bufsz + size) >= buf_.capacity() )
        //{
          buf_.resize(bufsz + size);
        //}

        cerr<< "Pre-read buffer (size: " << buf_.size() 
            << "|capacity: " << buf_.capacity() << ") == {" << endl
            << std::string(buf_.begin() + offset_, buf_.end()) << endl
   //         << "-----end buffer-----" << endl
   //         << "-------buffer-------" << endl
  //          << std::string(&buf_[0], &buf_[buf_.size()]) << endl
            << "}" << endl;
            ;
        //return boost::asio::buffer(&(*(buf_.end())), size);
  //      return boost::asio::buffer(&(*(buf_.begin())) + bufsz, size);
        return boost::asio::buffer(&buf_[bufsz], size);
      }

      std::string buffer_string()
      {
        return std::string(buf_.begin() + offset_, buf_.end());
        //return std::string(pos_, buf_.size());//buf_.end());
      }
      /*
        // Silently ignore a request to grow the buffer greater than the
        // content-length allows.
        // Note: this will break if the contents of the buffer are ever changed
        // (such as if data is url-decoded and inserted back into the buffer).
        if (size >= characters_left_)
        {
          size = characters_left_;
        }

        data_buffer_.reserve(bufsz + size);
        return boost::asio::buffer(data_buffer_.begin() + bufsz
                                  , data_buffer_.capacity() - bufsz);
      }
      */
      //boost::acgi::form_entry form_;
    };

    /// Return if the request is still open
    /**
     * For CGI, this always returns true. However, in the case that a
     * "Location: xxx" header is sent and the header is terminated, the
     * request can be taken to be 'closed'.
     */
    bool is_open(implementation_type& impl)
    {
      return impl.status() >= aborted;
    }

    /// Return the connection associated with the request
    typename implementation_type::client_type&
      client(implementation_type& impl)
    {
      return impl.client_;
    }

    int close(implementation_type& impl, http::status_code& http_s, int status)
    {
      impl.status() = closed;
      impl.http_status() = http_s;
      return status;
    }

    /// Synchronously read/parse the request meta-data
    /**
     * @param parse_stdin if true then STDIN data is also read/parsed
     */
    boost::system::error_code&
      load(implementation_type& impl, bool parse_stdin
          , boost::system::error_code& ec)
    {
      detail::save_environment(impl.env_vars());
      const std::string& cl = var(impl.env_vars(), "CONTENT_LENGTH", ec);
      impl.characters_left_ = cl.empty() ? 0 : boost::lexical_cast<std::size_t>(cl);
      impl.client_.bytes_left() = impl.characters_left_;

      const std::string& request_method = var(impl.env_vars(), "REQUEST_METHOD", ec);
      if (request_method == "GET")
        parse_get_vars(impl, ec);
      else
      if (request_method == "POST" && parse_stdin)
        parse_post_vars(impl, ec);

      if (ec) return ec;

      parse_cookie_vars(impl, ec);
      impl.status() = loaded;

      //BOOST_ASSERT(impl.status() >= loaded);

      return ec;
    }

    std::string&
      var(map_type& meta_data, const std::string& name
         , boost::system::error_code& ec)
    {
      return meta_data[name];
    }

    std::string
      GET(implementation_type& impl, const std::string& name
         , boost::system::error_code& ec)
    {
      //BOOST_ASSERT(impl.status() >= loaded);
      return var(impl.get_vars(), name, ec);
    }

    map_type&
      GET(implementation_type& impl)
    {
      //BOOST_ASSERT(impl.status() >= loaded);
      return impl.get_vars();
    }

    /// Find the post meta-variable matching name
    /**
     * @param greedy This determines whether more data can be read to find
     * the variable. The default is true to cause least surprise in the common
     * case of having not parsed any of the POST data.

     -----------------------------------------------
     Should this return a pair of iterators instead?
     What about url_decoding?
     -----------------------------------------------

     */
    std::string
      POST(implementation_type& impl, const std::string& name
          , boost::system::error_code& ec, bool greedy = true)
    {
      //BOOST_ASSERT(impl.status() >= loaded);
      const std::string& val = var(impl.post_vars(), name, ec);
      if (val.empty() && greedy && !ec)
      {

      }

      return val;
    }

    map_type&
      POST(implementation_type& impl)
    {
      //BOOST_ASSERT(impl.status() >= loaded);
      return impl.post_vars();
    }

    // prefer this to the above
    map_type&
      POST(implementation_type& impl, boost::system::error_code& ec)
    {
      //BOOST_ASSERT(impl.status() >= loaded);
      return impl.post_vars();
    }

    // TODO: use `greedy`
    std::string
      form(implementation_type& impl, const std::string& name
		      , boost::system::error_code& ec, bool greedy)
		{
		  std::string rm(env(impl, "REQUEST_METHOD", ec));
			//BOOST_ASSERT(!ec && !rm.empty());
			if (rm == "GET")
			  return GET(impl, name, ec);
		  else
			if (rm == "POST")
			  return POST(impl, name, ec);
			else
			  return "***BOOST_CGI_ERROR_INVALID_REQUEST_METHOD***";
		}
		
    // TODO: use `greedy`
		map_type&
      form(implementation_type& impl, boost::system::error_code& ec
		      , bool greedy)
		{
		  std::string rm(env(impl, "REQUEST_METHOD", ec));
			//BOOST_ASSERT(!ec && !rm.empty());
			if (rm == "GET")
			  return GET(impl);
		  else
			if (rm == "POST")
			  return POST(impl, ec);
		}

    /// Find the cookie meta-variable matching name
    std::string
      cookie(implementation_type& impl, const std::string& name
            , boost::system::error_code& ec)
    {
      //BOOST_ASSERT(impl.status() >= loaded);
      return var(impl.cookie_vars(), name, ec);
    }

    map_type&
      cookie(implementation_type& impl)
    {
      //BOOST_ASSERT(impl.status() >= loaded);
      return impl.cookie_vars();
    }


    /// Find the environment meta-variable matching name
    std::string
      env(implementation_type& impl, const std::string& name
         , boost::system::error_code& ec)
    {
      //BOOST_ASSERT(impl.status() >= loaded);
      const char* c = ::getenv(name.c_str());
      return c ? c : std::string();
    }

    map_type&
      env(implementation_type& impl)
    {
      //BOOST_ASSERT(impl.status() >= loaded);
      return impl.env_vars();
    }


    role_type
      get_role(implementation_type& impl)
    {
      return responder;
    }

    /// Set the http status (this does nothing for aCGI)
    void set_status(implementation_type& impl, http::status_code&)
    {
    }

  protected:
    /// Read and parse the cgi GET meta variables
    template<typename RequestImpl>
    boost::system::error_code
    parse_get_vars(RequestImpl& impl, boost::system::error_code& ec)
    {
      // Make sure the request is in a pre-loaded state
      //BOOST_ASSERT (impl.status() <= unloaded);

      std::string& vars = impl.env_vars()["QUERY_STRING"];
      if (vars.empty())
        return ec;

      detail::extract_params(vars, impl.get_vars()
                            , boost::char_separator<char>
                                ("", "=&", boost::keep_empty_tokens)
                            , ec);

      return ec;
    }

    /// Read and parse the HTTP_COOKIE meta variable
    template<typename RequestImpl>
    boost::system::error_code
    parse_cookie_vars(RequestImpl& impl, boost::system::error_code& ec)
    {
      // Make sure the request is in a pre-loaded state
      //BOOST_ASSERT (impl.status() <= unloaded);

      std::string& vars(impl.env_vars()["HTTP_COOKIE"]);
      if (vars.empty())
        return ec;

      detail::extract_params(vars, impl.cookie_vars()
                            , boost::char_separator<char>
                                ("", "=&", boost::keep_empty_tokens)
                            , ec);

      return ec;
    }

    /// Read and parse the cgi POST meta variables (greedily)
    template<typename RequestImpl>
    boost::system::error_code
    parse_post_vars(RequestImpl& impl, boost::system::error_code& ec)
    {
      // Make sure this function hasn't already been called
      //BOOST_ASSERT (!impl.stdin_parsed());

      std::string content_type (var(impl.env_vars(), "CONTENT_TYPE", ec));

      BOOST_ASSERT(!content_type.empty());

      if (boost::algorithm::ifind_first(content_type,
            "application/x-www-form-urlencoded"))
      {
        detail::throw_error(
          parse_url_encoded_form(impl, ec)
        );
      }
      else
      {
        detail::throw_error(
          parse_multipart_form(impl, ec)
        );
      }

      return ec;
    }

    boost::system::error_code
      parse_url_encoded_form(implementation_type& impl
                            , boost::system::error_code& ec)
    {      
      std::istream& is(std::cin);
      char ch;
      std::string name;
      std::string str;
      map_type& post_map(impl.post_vars());

      while( is.get(ch) && impl.characters_left_-- )
      {
          //std::cerr<< "; ch=" << ch << "; ";
          switch(ch)
          {
          case '%': // unencode a hex character sequence
              // note: function params are resolved in an undefined order!
              str += detail::url_decode(is);
              impl.characters_left_ -= 2; // this is dodgy **FIXME**
              break;
          case '+':
              str.append(1, ' ');
              break;
          case ' ':
              continue;
          case '=': // the name is complete, now get the corresponding value
              name = str;
              str.clear();
              break;
          case '&': // we now have the name/value pair, so save it
              post_map[name] = str;
              str.clear();
              name.clear();
             break;
          default:
              str.append(1, ch);
          }
      }
      // save the last param (it won't have a trailing &)
      if( !name.empty() )
          post_map[name] = str;//.empty() ? "" : str;
      

      return ec;
    }

    /// Parse a multipart form.
    boost::system::error_code
      parse_multipart_form(implementation_type& impl, boost::system::error_code& ec)
    {
      parse_boundary_marker(impl, ec);
      //parse_one_form_part(impl, ec);
      move_to_start_of_first_part(impl, ec);
      if (ec == boost::asio::error::eof) {
        cerr<< " -- Parsing done -- " << endl;
        //return ec.clear();
        return boost::system::error_code();
      }

      do {
        parse_form_part(impl, ec);
      }while( !impl.stdin_parsed_  && impl.client_.bytes_left() != 0 );

      // Do this just for now, for debugging
      parse_url_encoded_form(impl, ec);
      return ec;
    }

    boost::system::error_code
      parse_form_part(implementation_type& impl, boost::system::error_code& ec)
    {
      if (!parse_form_part_meta_data(impl, ec)
      &&  !parse_form_part_data(impl, ec))
        return ec;

      return ec;
    }

    boost::system::error_code
      parse_form_part_data(implementation_type& impl, boost::system::error_code& ec)
    {
      std::string regex("^(.*?)" // the data
                        "\\x0D\\x0A" // CR LF
                        "--" "(");
      if (impl.boundary_markers.size() > 1)
      {
        std::list<std::string>::iterator i(impl.boundary_markers.begin());
        regex = regex + "(?:" + *i + ")";
        ++i;
        for(; i != impl.boundary_markers.end(); ++i)
        {
          regex = regex + "|(?:" + *i + ")";
        }
      }
      else
      {
        regex += *impl.boundary_markers.begin();
      }
      
      regex += ")(--)?[ ]*\\x0D\\x0A";
      cerr<< "Regex: " << regex << endl;
      boost::regex re(regex);
      
      typedef typename
        implementation_type::buffer_type::iterator
      buffer_iter;

      boost::match_results<buffer_iter> matches;

      std::size_t offset = impl.offset_;
      cerr<< "offset = " << offset << endl;

      //int runs = 0;
      buffer_iter begin(impl.buf_.begin() + offset);
      buffer_iter end(impl.buf_.end());

      for(;;)
      {
        cerr<< "Starting regex_search" << endl;
        if (!boost::regex_search(begin, end, matches, re
                                , boost::match_default
                                | boost::match_partial))
        {
          cerr<< "Can't match any of this. {" << endl
              << std::string(begin, end) << endl
              << "}" << endl;
          return boost::system::error_code(345, boost::system::system_category);
        }
        else
        {
          for (unsigned int i = 0; i < matches.size(); ++i)
          {
            if (matches[i].length())
            {
              cerr<< "[" << i << "] == {" << endl
                  << matches[i] << endl
                  << "}" << endl;
            }
          }
          //  cerr<< "matches[0] = {" << endl
          //      << matches[0].str() << endl
          //      << "}" << endl;
          if (matches[1].matched)
          {
            impl.form_parts_.back().buffer_
             // = boost::range_iterator<;
             = std::make_pair(matches[1].first, matches[1].second);
            cerr<< "Saved buffer (size: "
                << std::distance(matches[1].first, matches[1].second)
                << ") := { " << impl.form_parts_.back().name << ", " << matches[1] << " }" << endl;
            impl.post_vars()[impl.form_parts_.back().name] = matches[1];
            impl.offset_ = offset + matches[0].length();
            //offset += matches[0].length();
            impl.pos_ = matches[0].second;
            cerr<< "offset := " << offset << endl
                << "impl.offset_ := " << impl.offset_ << endl;

            if (matches[3].matched)
              impl.stdin_parsed_ = true;
            //if (impl.client_.bytes_left() == 0)
            //{
            //  cerr<< "Read all the bytes up." << endl;
              //impl.stdin_parsed_ = true;
              return ec;
            //}
          }
          else
          {
            cerr<< "Reading more data." << endl;
            std::size_t bytes_read = impl.client_.read_some(impl.prepare(64), ec);
            //impl.stdin_bytes_read_ += bytes_read;
            
            if (bytes_read == 0)
            {
              impl.stdin_data_read_ = true;
              return ec;
            }

            begin = impl.buf_.begin() + offset;
            end = impl.buf_.end();
            cerr<< "Buffer (+" << bytes_read << ") == {" << endl
                << std::string(begin, end) << endl
                << "}" << endl;

            if (ec)
            {
              cerr<< "Error in parse_form_part_data()." << endl;
              return ec;
            }

          }
        }
          
      }
          

      return ec;
    }

    boost::system::error_code
      parse_form_part_meta_data(implementation_type& impl, boost::system::error_code& ec)
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

      typedef typename
        implementation_type::buffer_type::iterator
      buffer_iter;

      boost::match_results<
        typename
          implementation_type::buffer_type::iterator
      > matches;
      
      std::size_t offset = impl.offset_;
      cerr.flush();
      impl.pos_ = impl.buf_.begin();
      int runs = 0;
      cerr<< "Entering for() loop." << endl;
      std::size_t bytes_read = 0;
      for(;;)
      {
        buffer_iter begin(impl.buf_.begin() + offset);
        buffer_iter end(impl.buf_.end());
        
        if (!boost::regex_search(begin, end, matches, re
                                , boost::match_default | boost::match_partial))
        {
          cerr<< "No chance of a match, quitting." << endl;
          impl.stdin_parsed_ = true;
          return ec;
        }
        cerr<< "matches.str() == {" << endl
            << matches.str() << endl
            << "}" << endl
            << matches.size() << " submatches" << endl;
        for (unsigned i = matches.size(); i != 0; --i)
        {
          cerr<< "match[" << i << "] := { " << matches[i] << " }" << endl;
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
              cerr<< "Saved name" << endl;
            }
            else
            {
              part.meta_data_[matches[i]]
                = std::make_pair(matches[i+1].first, matches[i+1].second);
              cerr<< "Part := { " << matches[i] << ", " << matches[i+1] << " }" << endl;
              //= boost::iterator_range<buffer_iter>(matches[3].first, matches[3].second);
            }
            impl.form_parts_.push_back(part);
          
          
         }
          
         if (matches[13].str() == "\r\n")
         {
           impl.offset_ = offset + matches[0].length();
           offset += matches[0].length();
           impl.pos_ = matches[0].second;
          
           //cerr<< "Current buffer == {" << endl
           //    << impl.buffer_string() << endl
           //    << "}" << endl;
           cerr<< "Leaving parse_form_part_meta_data()" << endl;
           return ec;
         }
         else
         {
           throw std::runtime_error("Invalid POST data (header wasn't terminated as expected)");
         }
         
        }else{
          cerr<< "Not read enough data yet, reading more." << endl;
          bytes_read = impl.client_.read_some(impl.prepare(64), ec);
          if (ec)
          {
            cerr<< "Error reading data: " << ec.message() << endl;
            cerr<< "Leaving parse_form_part_meta_data()" << endl;
            return ec;
          }
          cerr<< "Read " << bytes_read << " bytes." << endl;
          cerr<< "buffer = {" << endl
              << impl.buffer_string() << endl
              << "} or {" << endl;
              //<< std::string(impl.pos_, ;
          /*
          for (unsigned int i = 0; i < matches.size(); ++i)
          {
            if (matches[i].length())
            {
              cerr<< "[" << i << "] == {" << endl
                  << matches[i] << endl
                  << "}" << endl;
            }
          }
          cerr<< "-------buf------" << endl
              << std::string(begin, end) << endl
              << "----------------" << endl;
          */
          //offset = impl.buf_.end();
            if (++runs > 40)
            {
              cerr<< "Run 40 times; bailing out." << endl;
              break;
            }
          cerr<< "Waiting buffer (unparsed) == {" << endl << std::flush
              << impl.buffer_string() << endl
              << "}" << endl
              << "offset     = " << offset     << endl;
        //if (bytes_read == 0)
        //  break;
       }
      }

      //cerr<< "impl.part

      cerr<< "Leaving parse_form_part_meta_data()" << endl;
      return ec;
    }

    boost::system::error_code
      move_to_start_of_first_part(implementation_type& impl, boost::system::error_code& ec)
    {
      //cerr<< "Marker = " << impl.boundary_markers.front() << endl
      //    << "blah" << "--" + impl.boundary_markers.front() + ")" << endl;
      boost::regex re("((?:.*)?"   // optional leading characters
                      //"(?:\\x0D\\x0A)|^" // start of line
                      "[\\x0D\\x0A^]*?"
                      "("
                        "--" + impl.boundary_markers.front() + // two dashes and our marker
                      ")"
                      "(--)?" // optional two dashes (not sure if this is allowed)
                      " *\\x0D\\x0A)");
                                        // on the first marker.

      typedef typename
          implementation_type::buffer_type::iterator
      buffer_iter;

      boost::match_results<
        typename
          implementation_type::buffer_type::iterator
      > matches;
      //cerr<< "Boundary marker == {" << endl
      //    << impl.boundary_marker << endl
      //    << "}" << endl;

      // get data into our buffer until we reach the first boundary marker.
      int runs = 0;
      std::size_t offset = 0;
//      std::size_t bytes_read = impl.client_.read_some(impl.prepare(32), ec);
//      if (ec || (bytes_read == 0))
//        return ec;
      std::size_t bytes_read = 0;
      for(;;)
      {
        bytes_read = impl.client_.read_some(impl.prepare(32), ec);
        if (ec || (bytes_read == 0))
          return ec;
        buffer_iter begin(impl.buf_.begin());// + offset);
        buffer_iter end(impl.buf_.end());
        if (!boost::regex_search(begin, end //impl.buf_.begin(), impl.buf_.end()
                                , matches, re, boost::match_default | boost::match_partial))
        {
          // No chance of matching input with expected, so erase it and start again.
          //std::cerr<< " == buffer = " << std::string(impl.buf_.begin(), impl.buf_.end())
          //         << " == capacity = " << impl.buf_.capacity() << " ======= ";
          //impl.buf_.clear();
          cerr<< "No chance of matching." << endl;
          offset = impl.buf_.size();
          //std::cerr<< "Buffer cleared." << endl;
          continue;
        }
        else
        {
          //std::cerr<< "regex_search == true" << std::endl
          //         << "matches[0] =={{ " << matches[0] << " }}=== " << std::endl
          //         << "matches[1] =={{ " << matches[1] << " }}=== " << std::endl
          //         << "matches[2] =={{ " << matches[2] << " }}=== " << std::endl;
          if (matches[2].matched){
            cerr<< "Found boundary marker... OK!!" << endl;
            //cerr<< "[0] = " << matches[0].str() << endl;
            //cerr<< "[1] = " << matches[1].str() << endl;
            //impl.offset_ = matches[1].length();
            //impl.pos_ = matches[0].second;
            //cerr<< "setting pos_ to < " << std::string(impl.pos_, impl.pos_ + 5) << "... >" << endl
            //    << "setting offset_ = " << impl.offset_ << endl;
           //     << std::string(matches[1].first, matches[1].second) << endl;
            //impl.pos_ = matches[1].second;
            //cerr<< "deleting {" << endl
            //    << std::string(impl.buf_.begin(), matches[0].second) << endl
            //    << "} leaving {" << endl
            //    << std::string(matches[0].second, impl.buf_.end()) << endl
            //    << "}" << endl;
            //cerr<< "bufsize = " << impl.buf_.size() << endl;
            //cerr<< "bufsize = " << impl.buf_.size() << endl;
            cerr<< "buffer now (before erase) == {" << endl
                << std::string(impl.buf_.begin(), impl.buf_.end()) << endl
                << "}" << endl;
            impl.buf_.erase(impl.buf_.begin(), matches[0].second);
            impl.offset_ = 0;
            impl.pos_ = impl.buf_.begin();
            return ec;
          } else {
            cerr<< "not read enough data" << std::endl;
      //std::cerr<< "; bytes_read = " << bytes_read
        //      << "; bufsize = " << impl.buf_.size() 
          //     << "; capacity = " << impl.buf_.capacity() << std::flush
            //   << "; data = {{" << std::string(impl.buf_.begin(), impl.buf_.end()) << "}}";
            
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
      parse_one_form_part(implementation_type& impl, boost::system::error_code& ec)
    {
      // continuously read data while parsing it until one complete form part has
      // been read.
      // Note, this may mean recursing into embedded sub-parts if necessary, but
      // still only the first *complete* part/sub-part would be read.
      //boost::regex re("\r\n--" + impl.boundary_marker + " +

      for(;;)
      {
        //if (impl.client_.read_some(impl.prepare(1024), ec))
        //  return ec;
        //boost::asio::read_until(impl.client_, impl.buffer_, impl.boundary_marker, ec);
        break;


      }

      return ec;
    }

    /// Get the boundary marker from the CONTENT_TYPE header.
    boost::system::error_code
      parse_boundary_marker(implementation_type& impl, boost::system::error_code& ec)
    {
      // get the meta-data appended to the content_type
      std::string content_type(var(impl.env_vars(), "CONTENT_TYPE", ec));
      //BOOST_ASSERT(!content_type.empty());

      //boost::algorithm::ifind(var(impl, "CONTENT_TYPE"), "bounday="
      
      boost::regex re("; ?boundary=\"?([^\"\n\r]+)\"?");
      boost::smatch match_results;
      if (!boost::regex_search(content_type, match_results, re))
        return boost::system::error_code(666, boost::system::system_category);

      impl.boundary_marker = match_results[1].str();
      // New boundary markers are added to the front of the list.
      impl.boundary_markers.push_front(match_results[1].str());

      /*
      cerr<< "full = " << content_type << endl
          << "full search string = " << match_results[0] << endl
          << "marker length = " << match_results[1].length() << endl
          << "marker = " << impl.boundary_marker << endl
          << "_[2] = " << match_results[2] << endl;
          */
     
      return ec;
    }

    /// Read and parse a single cgi POST meta variable (greedily)
    template<typename RequestImpl>
    boost::system::error_code
    parse_one_post_var(RequestImpl& impl, boost::system::error_code& ec)
    {
      //#     error "Not implemented"
      return ec;
    }
  };

} // namespace cgi

#include "boost/cgi/detail/pop_options.hpp"

#endif // CGI_CGI_SERVICE_IMPL_HPP_INCLUDED__

