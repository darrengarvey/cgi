#include <string>
#include <boost/cgi/cgi.hpp>
using namespace cgi;

//[main
/*`
It is assumed you have included <boost/cgi/cgi.hpp> and are `using namespace cgi`.
*/

int main(int,char**)
{
  request req;

/*`
At this point, the environment variables are accessible. This includes cookie and form variables too, which by default are all parsed (this is optional).

The `response` class provides a streaming interface for writing replies. You ['can] write to the request object directly, but for now we're going to just use the `response`, which works well for most situations. Writing to a `response` is buffered - whereas writing to the request directly isn't - so if an error occurs, you can simply `clear()` the response and send an error message, which is much cleaner than sending half a response to the client, followed by "... Sorry, I just broke!".
*/

  response resp;

/*`
Let's assume you now want to check if the user has a cookie, "user_name", set. We get at it like this:
*/

  std::string user_name( req.cookies["user_name"] );

/*`
If it's set, we'll be polite and say hello. If you are used to CGI programming, you'll notice the lack of any HTTP headers. If you don't want to bother with headers, a default header `'Content-type: text/plain'` is sent, followed by the usual HTTP end-of-line `'\r\n'` and a blank line which indicates the end of the headers and the start of content.
*/

  if (!user_name.empty())
  {
    resp<< "Hello there, " << req.cookies["user_name"] << ". How are you?";

/*`
That's all we want to say for now, so just send this back and quit.
*/

    resp.send(req);

    return 0;
  }

/*`
If the cookie isn't set, we will check if the user has posted a __GET__/__POST__ form with their name.
*/
  user_name = req.form["user_name"];

  if (!user_name.empty())
  {
/*`
If they have told us their name, we should set a cookie so we remember it next time. Then we can say hello and exit. There are two ways to set a cookie: either directly using `req.set_cookie("user_name", user_name)` or the method below. You can also send an expiry date for the cookie [rfc 822] and a path. Note that if you don't set a value for the cookie, the cookie will be deleted.

Again, the request object isn't buffered, so we are going to keep using the `response` in case something breaks and we end up not wanting to set the cookie. The cookie we set below will expire when the client closes their browser.

This time, we shall send a Date header. If we do this (ie. send a header ourselves), we must also set the Content-type header and terminate the headers, like below.
*/

    resp<< cookie("user_name", user_name)
        << header("Date", "Tue, 15 Nov 1994 08:12:31 GMT")
        << header("Content-type", "text/plain")
        << "Hello there, " << user_name << ". You're new around here.";

    resp.send(req);
  }

/*`
Now, if we have no idea who they are, we'll send a form asking them for their name. As the default `"Content-type"` header is `"text/plain"`, we'll change this to `"text/html"` so the user's browser will display the HTML form. You can do this using `set_header(req, "Content-type", "text/html")` or `resp<< header("Content-type", "text/html")`. Since writing with raw strings is error-prone, the shortcut below is available.
*/

  resp<< content_type("text/html")
      << "Hello there. What's your name?" "<p />"
         "<form method='POST'>"
         "<input type='text' name='user_name' />"
         "<input type='submit' />";

  resp.send(req);

/*`
And that's all we want to do for now, so we can close the program.
*/

  return 0;
}
//]
