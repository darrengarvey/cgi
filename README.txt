Comments/critique are welcome and can be directed to one of:

  cgi-devel @ cgi dot sourceforge dot net
  lists dot drrngrvy @ gmail dot com

------------------------------------------------------------

Check the library works ok by first running the tests + compiling the examples.

These are all in the

	libs/cgi/test

directory (see Jamfile.v2 for build instructions).

Note that some of the examples require cTemplate, so they will fail to compile unless you have that installed.

The tests aren't exhaustive - this library is still under development.

Currently tested on

Ubuntu 8.04 desktop 32bit
Ubuntu 8.04 server 64bit
MSVC 2008

FastCGI stuff only works on linux at the moment. If you're a Windows network programmer, can you help?

