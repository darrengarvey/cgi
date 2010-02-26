Comments/critique are welcome and can be directed to one of:

  cgi-devel @ cgi dot sourceforge dot net
  lists dot drrngrvy @ gmail dot com

------------------------------------------------------------

Websites for more information on this library are:
 http://cgi.sf.net
 http://omnisplat.com

Check the library works ok by first running the tests + compiling the examples.

These are all in the

	libs/cgi/test

directory (see Jamfile.v2 for build instructions).

Note that some of the examples (ie. the "stencil" examples) require cTemplate, so they will fail to compile unless you have that installed.

The tests aren't exhaustive - this library is still under development.

Currently tested on

Ubuntu 8.04 desktop 32bit
Ubuntu 9.10 desktop 32bit
Ubuntu 8.04 server 64bit
MSVC 2008 Windows XP

FastCGI stuff only works on linux at the moment. FastCGI is supported on Windows as an external server, tested against Apache 2.2 with mod_fastcgi (obsolete) and mod_fcgid. mod_fcgid has been merged with upstream Apache and should become part of a future release.

