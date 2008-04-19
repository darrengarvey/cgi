#ifndef BOOST_CGI_FORM_PART_HPP_INCLUDED__
#define BOOST_CGI_FORM_PART_HPP_INCLUDED__

#include <map>
#include <vector>
#include <boost/range.hpp>

namespace cgi {
 namespace common {

   struct form_part
   {
     typedef std::vector<char> buffer_type;
     typedef buffer_type::iterator iter_t;
     
     typedef boost::iterator_range<
       buffer_type::const_iterator
     > range_type;

     typedef std::pair<iter_t, iter_t> pair_t;
     
     typedef std::map<
       std::string, pair_t
     > meta_data_map_type;
   
     form_part()
     {
     }
   
     meta_data_map_type meta_data_;

     /// The boundary marker that's needed.
     // If this is empty, it means the corresponding data has been read.
     std::string boundary_marker_;

     //range_type buffer_;
     pair_t buffer_;

     std::string content_type; // must exist
     std::string name; // must exist (?) **FIXME**

     // Using a simple map while everything is changing. This will not copy the
     // values when it is properly implemented (it'll hold a pair of iterators 
     // to the data).
     //std::map<std::string, std::string> meta_data_;
     
     // Boolean to show if the form part has been completely read/parsed
     //bool finished_;
   };

 } // namespace common
} // namespace cgi

#endif // BOOST_CGI_FORM_PART_HPP_INCLUDED__

