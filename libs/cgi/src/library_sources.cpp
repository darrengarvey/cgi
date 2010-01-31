
/**
 * Include this file into your project to compile the library "in-place".
 *
 * If you're using Boost.Jam, the library can be built by specifying
 * "--build-cgi" on the command line.
 *
 * eg.
 *    libs/cgi/build$ bjam --build-cgi               (builds the library)
 *
 *    libs/cgi/example$ bjam --build-cgi install
 *
 *        (builds and installs all the examples using the compiled binary
 *         - which will be installed if necessary - see the docs for more)
 *
**/

#ifndef BOOST_CGI_LIBRARY_SOURCES_INCLUDE_ONCE
#    define BOOST_CGI_LIBRARY_SOURCES_INCLUDE_ONCE
#else
#    error "CGI library sources included twice (don't do that)."
#endif

#ifndef BOOST_CGI_BUILD_LIB
#    warning "BOOST_CGI_BUILD_LIB not defined (are you sure you want to "
#    warning "build the library?)"
#    warning "Building anyway"
#    define BOOST_CGI_BUILD_LIB
#endif // BOOST_CGI_BUILD_LIB

////////////  the sources
//#include <boost/cgi/impl/form_parser.ipp>
//#include <boost/cgi/impl/response.ipp>
#include <boost/cgi/impl/fcgi_request_service.ipp>
#include <boost/cgi/detail/url_decode.ipp>
