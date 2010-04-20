#include "TracebackServer.hpp"
#include <boost/chrono/chrono.hpp>

#define TBS_TEMPLATE__DEBUG_VIEW "../stencils/debug_view.html"

namespace detail {

boost::uint64_t
get_microseconds(boost::chrono::process_clock::duration const& tp)
{
  return boost::chrono::duration_cast<boost::chrono::microseconds>(tp).count();
}

} // namespace detail


TracebackServer::TracebackServer(void)
    : timer_()
{
}

TracebackServer::~TracebackServer(void)
{
}

template<typename MapT>
void TracebackServer::expand_map(dictionary_type& dict, MapT& data, std::string const& title)
{
    dictionary_type* sub_dict = dict.AddSectionDictionary("REQUEST_DATA_MAP");
    sub_dict->SetValue("REQUEST_DATA_MAP_TITLE", title);
    if (data.empty())
        sub_dict->ShowSection("REQUEST_DATA_MAP_EMPTY");
    else {
        for(typename MapT::const_iterator iter(data.begin()), end(data.end()); iter != end; ++iter)
        {
            dictionary_type* nvpair_dict = sub_dict->AddSectionDictionary("REQUEST_DATA_PAIR");
            nvpair_dict->SetValue("REQUEST_DATA_NAME", iter->first.c_str());
            nvpair_dict->SetValue("REQUEST_DATA_VALUE", iter->second.c_str());
        }
    }
}

void TracebackServer::stop_timer()
{
    timer_.elapsed(stop_times_);
}

void TracebackServer::dump_times(dictionary_type& dict)
{
    dict.SetValue("RUNNING_TIME_RESOLUTION", "microseconds");
    dict.SetFormattedValue("REAL_TIME", "%d us", detail::get_microseconds(stop_times_.real));
    dict.SetFormattedValue("CPU_TIME", "%d us", detail::get_microseconds(stop_times_.user + stop_times_.system));
    dict.SetFormattedValue("USER_TIME", "%d us", detail::get_microseconds(stop_times_.user));
    dict.SetFormattedValue("SYSTEM_TIME", "%d us", detail::get_microseconds(stop_times_.system));
    dict.ShowSection("RUNNING_TIME");
}

void TracebackServer::bomb_out(std::string const& error, response_type& response, request_type& request)
{
    stop_timer();
    response_type resp2;
    template_ = ctemplate::Template::GetTemplate(TBS_TEMPLATE__DEBUG_VIEW, ctemplate::STRIP_WHITESPACE);
    dictionary_type dict("debug_view");
    dict.SetValue("ERROR", error);
    expand_map(dict, request.env, "Environment Data");
    expand_map(dict, request.form, "Form (" + request.method() + ") data");
    expand_map(dict, request.cookies, "Cookies");
    BOOST_FOREACH(std::string& hstring, response.headers())
    {
        dict.SetValueAndShowSection("HEADER", hstring, "RESPONSE_HEADER");
    }
    dump_times(dict);

    //dict.SetValue("ORIGINAL_RESPONSE", response.rdbuf()->data());
    std::string debug_output;
    template_->Expand(&debug_output, &dict);
    resp2<< boost::cgi::content_type("text/html")
         << debug_output
         << response;
    resp2.send(request.client());
    request.close();
}


void TracebackServer::bomb_out(boost::system::system_error* err, response_type& response, request_type& request) {
    //dictionary_type* sub_dict = dict.AddSectionDictionary("REQUEST_DATA_MAP");
    typedef request_type::string_type string;
    string err_msg("boost::system::system_error*<ul class=nvpair><li class=name>Code:</li>");
    err_msg += string("<li class=value>") + boost::lexical_cast<std::string>(err->code()) + "</li>" 
            + "<li class=name>What:</li>"
            + "<li class=value>" + err->what() + "</li>"
            + "<li class=name>Type:</li>"
            + "<li class=value>" + typeid(*err).name() + "</li><br class=clear /></ul>";
    bomb_out(err_msg, response, request);
}

void TracebackServer::bomb_out(std::exception* e, response_type& response, request_type& request) {
    typedef request_type::string_type string;
    string err_msg("std::exception*<ul class=nvpair><li class=name>What:</li>");
    err_msg += string("<li class=value>") + e->what() + "</li>" 
            + "<li class=name>Type:</li>"
            + "<li class=value>" + typeid(*e).name() + "</li><br class=clear /></ul>";
    bomb_out(err_msg, response, request);
}

void TracebackServer::bomb_out(std::exception& e, response_type& response, request_type& request) {
    typedef request_type::string_type string;
    string err_msg("std::exception<ul class=nvpair><li class=name>What:</li>");
    err_msg += string("<li class=value>") + e.what() + "</li>" 
            + "<li class=name>Type:</li>"
            + "<li class=value>" + typeid(e).name() + "</li><br class=clear /></ul>";
    bomb_out(err_msg, response, request);
}

bool TracebackServer::run(callback_type const& callback)
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
            return 0 == boost::cgi::commit(request, response);
        } else {
            bomb_out("Callback returned code #" + boost::lexical_cast<std::string>(ret) + "; unknown error", response, request);
        }
    }catch(boost::system::system_error* err) {
        bomb_out(err, response, request); return false;
    }catch(std::exception* e) {
        bomb_out(e, response, request); return false;
    }catch(std::exception& e) {
        bomb_out(e, response, request); return false;
    }catch(...){
        bomb_out("Unknown exception (captor: CGI hello example TracebackServer::run())", response, request); return false;
    }
    return false;
}

