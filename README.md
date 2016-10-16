This is a slightly derived version of the CGI C++ Library.
It is tested with Ubuntu 14.04 (using Travis CI), 16.04 (the Development System), 64 bit using CMake.

The Build status with Ubuntu 14.04 and Boost 1.58 is
[![Build Status](https://travis-ci.org/leutloff/cgi.png)](https://travis-ci.org/leutloff/cgi)


This is the original README.txt:

------------------------------------------------------------

Comments/critique are welcome and can be directed to one of:

  cgi-devel @ cgi dot sourceforge dot net
  darren dt garvey @ gmail dt com

------------------------------------------------------------

== DISCLAIMER ==

Note: This library is *not* a Boost library. It uses Boost coding conventions and will be proposed for review in due course.

Please do not confuse the code style with any mark of approval coming from the members of Boost. Said approval is still to be obtained.

== / DISCLAIMER ==

Websites for more information on this library are:
 - http://cgi.sf.net
 - http://omnisplat.com
 
Documentation is being updated so the documentation up on these sites is quite out of date. The examples in this package are by far the best teaching aid for the moment.

Check the library works ok by first running the tests + compiling the examples.

These are all in the

	libs/cgi/test

directory (see Jamfile.v2 for build instructions).

Note that some of the examples (ie. the "stencil" examples) require Google's cTemplate, so they will fail to compile unless you have that installed.

The tests aren't exhaustive - this library is still under development.

Currently tested on

- Ubuntu 8.04 desktop 32bit
- Ubuntu 9.10 desktop 32bit
- Ubuntu 8.04 server 64bit
- MSVC 2008 Windows XP
- MSVC 2008 Windows 7

FastCGI stuff works on linux and Windows under Apache 2.2 and mod_fcgid. Currently external fastcgi servers (eg. on mod_fastcgi) are only supported on linux.

Bleeding edge code is at https://svn.boost.org/trac/boost/browser/sandbox/SOC/2007/cgi/trunk

Check out the latest code with: svn co https://svn.boost.org/svn/boost/sandbox/SOC/2007/cgi/trunk


