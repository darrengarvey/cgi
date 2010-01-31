#include "TracingServer.hpp"
#include <boost/chrono/chrono.hpp>
#include <typeinfo>

namespace detail {

// The styling information for the page.
static const char* gCSS_text =
//"body { padding: 0; margin: 3%; border-color: #efe; }"
".info"
"{"
    "border-color: #ca3766;"
    "border-width: 1px 0 1px 0;"
    "border-style: solid;"
    "padding: 2px 8px 2px 8px;"
    "margin: 1em"
"}"
"#response"
"{"
    "text-align: center;"
    "padding: 20px 30px 20px 30px;"
    "margin: 1em;"
    "border-width: 2px 0 2px 0;"
    "border-style: solid;"
    "border-color: #ca3766;"
"}"
".var_map_title"
"{"
    "font-weight: bold;"
    "font-size: large;"
"}"
".var_map"
"{"
    "border: 1px dotted;"
    "padding: 2px 3px 2px 3px;"
    "margin: 15px 0 15px 0;"
"}"
".var_pair"
"{"
    "border-top: 1px dotted;"
    "overflow: auto;"
    "padding: 0;"
    "margin: 0;"
"}"
".var_name"
"{"
    "position: relative;"
    "float: left;"
    "width: 30%;"
    "font-weight: bold;"
"}"
".var_value"
"{"
    "position: relative;"
    "float: left;"
    "width: 65%;"
    "left: 1%;"
    "border-left: 1px solid;"
    "padding: 0 5px 0 5px;"
    "overflow: auto;"
    "white-space: pre;"
"}"
"ul.nvpair"
"{"
    "list-style-type: none;"
    "padding: 0;"
    "margin: 0;"
"}"
"ul.nvpair li"
"{"
    "display: inline;"
    "width: 175px;"
    "float: left;"
"}"
"ul.nvpair li.name"
"{"
    "text-align: right;"
    "padding-right: 1em;"
    "clear: left;"
"}"
"ul.nvpair li.value"
"{"
    "text-align: left;"
    "padding-left: 1em;"
"}"
".clear { clear: both; }"
;


//
// This function writes the title and map contents to the ostream in an
// HTML-encoded format (to make them easier on the eye).
//
template<typename OStreamT, typename MapT>
void format_map(OStreamT& os, MapT& m, const std::string& title)
{
  os<< "<div class=\"var_map\">"
         "<div class=\"var_map_title\">"
    <<       title
    <<   "</div><ul>";

  if (m.empty())
    os<< "<ul class=\"var_pair\">EMPTY</ul>";
  else
    for (typename MapT::const_iterator i = m.begin(); i != m.end(); ++i)
    {
      os<< "<li class=\"var_pair\">"
             "<div class=\"var_name\">"
        <<       i->first
        <<   "</div>"
             "<div class=\"var_value\">"
        <<       i->second
        <<   "</div>"
           "</li>";
    }
  os<< "</ul></div>";
}

boost::uint64_t
get_microseconds(boost::chrono::process_clock::duration& tp)
{
  return boost::chrono::duration_cast<boost::chrono::microseconds>(tp).count();
}

}

TracingServer::TracingServer(void)
    : timer_()
{
}

TracingServer::~TracingServer(void)
{
}

void TracingServer::bomb_out(std::string const& error, response_type& response, request_type& request)
{
    boost::chrono::process_times times;
    timer_.elapsed(times);
    response_type resp2;
    resp2<< boost::cgi::content_type("text/html")
         << "<html>"
            "<head>"
               "<title>CGI Echo Example</title>"
               "<style type=\"text/css\">"
         <<       detail::gCSS_text <<
               "</style>"
            "<head>"
            "<body>"
            "<p style='font-weight: bold'>"
                "<h1 style='font-size: x-large'>Problem found.</h1>"
                "<p>The original response follows, after some details about the error that occurred and the request that caused it</p>"
                "<p>First, the error that we tried to detect:</p>"
            "</p>"
            "<div class=\"info\">"
         <<     error
         << "</div>"
            "The request took the following time to process:"
            "<div class=\"info\">"
            "<ul class='nvpair'>"
              "<li class='name'>Real time</li>"
              "<li class='value'>" << detail::get_microseconds(times.real) << " microseconds</li>"
              
              "<li class='name'>User time</li>"
              "<li class='value'>" << detail::get_microseconds(times.user) << " microseconds</li>"
              
              "<li class='name'>System time</li>"
              "<li class='value'>" << detail::get_microseconds(times.system) << " microseconds</li>"
            "</ul>"
            "<br class='clear' />"
            "</div>"

            "Some details about the request:"
            "<div class=\"info\">";
            //style='"
              //      "border-color: #ca3766;" "border-width: 1px 0 1px 0;" "border-style: solid;"
                //    "padding: 2px 8px 2px 8px;" "margin: 1em" "'>";
    detail::format_map(resp2, request.env, "Environment data");
    detail::format_map(resp2, request.form, "Form [" + request.request_method() + "] Variables");
    detail::format_map(resp2, request.cookies, "Cookies");
    resp2<< "</div>"
            "<p>The headers sent in the original (broken) response were:</p>"
            "<div class=\"info\">"
                "<pre style='white-space:pre; padding-left: 3em; margin-left: 1em'>"
                  "<ul>";
    BOOST_FOREACH(std::string& hstring, response.headers())
    {
        resp2<< "<li>" << hstring << "</li>";
    }
    resp2<<         "</ul>"
         <<     "</pre>"
            "</div>"
            "Below is the original response (inside borders, with (x=30;y=20)px of padding):"
            "<div id='response'>"
         <<     response // print out the original response
         << "</div>"
            "</body>"
            "</html>";
    resp2.send(request.client());
    request.close();
}

void TracingServer::bomb_out(boost::system::system_error* err, response_type& response, request_type& request) {
    typedef request_type::string_type string;
    string err_msg("<ul class=nvpair>boost::system::system_error*<li class=name>Code:</li>");
    err_msg += string("<li class=value>") + boost::lexical_cast<std::string>(err->code()) + "</li>" 
            + "<li class=name>What:</li>"
            + "<li class=value>" + err->what() + "</li>"
            + "<li class=name>Type:</li>"
            + "<li class=value>" + typeid(err).name() + "</li><br class=clear /></ul>";
    bomb_out(err_msg, response, request);
}

void TracingServer::bomb_out(std::exception& e, response_type& response, request_type& request) {
    typedef request_type::string_type string;
    string err_msg("<ul class=nvpair>std::exception<li class=name>What:</li>");
    err_msg += string("<li class=value>") + e.what() + "</li>" 
            + "<li class=name>Type:</li>"
            + "<li class=value>" + typeid(e).name() + "</li><br class=clear /></ul>";
    bomb_out(err_msg, response, request);
}

bool TracingServer::run(callback_type const& callback)
{
    timer_.start();
    request_type request;
    response_type response;
    try {
        request.load(boost::cgi::parse_all);
        int ret(callback(request, response));
        if (request.form["debug"] == "1") {
            bomb_out("** Debug mode ** - client callback returned code #" + boost::lexical_cast<std::string>(ret)
                    , response, request);
        }else
        if (!ret) {
            response.send(request.client());
            request.close();
            return true;
        } else {
            bomb_out("Callback returned code #" + boost::lexical_cast<std::string>(ret) + "; unknown error", response, request);
        }
    }catch(boost::system::system_error* err) {
        bomb_out(err, response, request); return false;
    }catch(std::exception& e) {
        bomb_out(e, response, request); return false;
    }catch(...){
        bomb_out("Unknown error (captor: CGI hello example TracingServer::run())", response, request); return false;
    }
    return false;
}
