
#ifndef BOOST_CGI_REQUEST_DATA_HPP_INCLUDED_20100429_
#define BOOST_CGI_REQUEST_DATA_HPP_INCLUDED_20100429_

#include <exception>
#include <boost/assert.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/utility/enable_if.hpp>
///////////////////////////////////////////////////////////
#include "boost/cgi/config.hpp"
#include "boost/cgi/common/map.hpp"
#include "boost/cgi/common/data_map_proxy.hpp"
#include "boost/cgi/detail/is_multimap.hpp"

BOOST_CGI_NAMESPACE_BEGIN

 namespace common {
 
  /// The default request_data<T> type is essentially data_map_proxy<T>.
  template<typename T, class Enable = void>
  class request_data
    : public data_map_proxy<T>
  {
  public:
    typedef T                                         map_type;
    typedef data_map_proxy<map_type>                  self_type;
    typedef typename map_type::key_type               key_type;
    typedef typename map_type::value_type             value_type;
    typedef typename map_type::mapped_type            mapped_type;
    typedef typename map_type::size_type              size_type;
    typedef typename map_type::iterator               iterator;
    typedef typename map_type::const_iterator         const_iterator;
    typedef typename map_type::reverse_iterator       reverse_iterator;
    typedef typename map_type::const_reverse_iterator const_reverse_iterator;
    typedef typename map_type::allocator_type         allocator_type;

    using data_map_proxy<T>::operator[];
  };

  /// Specialisation for request_data<T> when T is a multimap.
  template<typename T>
  class request_data<T, 
        typename boost::enable_if<detail::is_multimap<T> >::type
    >
    : public data_map_proxy<T>
  {
  public:
    typedef T                                         map_type;
    typedef data_map_proxy<map_type>                  self_type;
    typedef typename map_type::key_type               key_type;
    typedef typename map_type::value_type             value_type;
    typedef typename map_type::mapped_type            mapped_type;
    typedef typename map_type::size_type              size_type;
    typedef typename map_type::iterator               iterator;
    typedef typename map_type::const_iterator         const_iterator;
    typedef typename map_type::reverse_iterator       reverse_iterator;
    typedef typename map_type::const_reverse_iterator const_reverse_iterator;
    typedef typename map_type::allocator_type         allocator_type;

    std::pair<iterator, iterator> equal_range( const key_type& key ) {
      return impl()equal_range(key);
    }

    bool matches(key_type const& key, mapped_type const& val)
    {
      std::pair<iterator, iterator> ret = impl().equal_range(key);
      for (iterator iter = ret.first, end = ret.second; iter != end; ++iter)
        if (iter->second == val)
          return true;
      return false;
    }

    mapped_type& operator[](const char* varname) {
      BOOST_CGI_MAP_ASSERT(impl_);
      if (impl_->find(varname) == impl_->end()) {
        using std::make_pair;
        impl_->insert(make_pair(varname, ""));
      }
      return impl_->find(varname)->second;
    }

    mapped_type& operator[](key_type const& varname) {
      BOOST_CGI_MAP_ASSERT(impl_); 
      if (impl_->find(varname) == impl_->end()) {
        using std::make_pair;
        impl_->insert(make_pair(varname, ""));
      }
      return impl_->find(varname)->second;
    }
  };

  typedef request_data<env_map> env_data;
  typedef request_data<post_map> post_data;
  typedef request_data<get_map> get_data;
  typedef request_data<form_map> form_data;
  typedef request_data<cookie_map> cookie_data;
  typedef request_data<upload_map> upload_data;

 } // namespace common
BOOST_CGI_NAMESPACE_END

#endif // BOOST_CGI_REQUEST_DATA_HPP_INCLUDED_20100429_
