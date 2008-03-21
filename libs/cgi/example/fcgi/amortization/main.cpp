//                     -- main.hpp --
//
//           Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
//
// Amortization Calculator
// -----------------------
//
// This file uses Google cTemplate to show the benefits of using an
// HTML template engine. The code isn't commented, but should be 
// reasonably self-explanatory.
//
// It is a very basic amortization calculator.
//
#include <iostream>
#include <iomanip>
#include <boost/cgi/fcgi.hpp>
#include <boost/algorithm/string/regex.hpp>
#include <google/template.h>
#include <boost/high_resolution_timer.hpp>

using namespace boost::fcgi;

/// Convert a string like '$250,000' into one like '250000'.
std::string string_from_currency(std::string amt)
{
  // this is much too hardcore, but it works fine...
  boost::algorithm::erase_all_regex(amt, boost::regex("[$, ]"));
  return amt;
}

/// This function fills the dictionary and sub-dictionaries with relevant values.
template<typename Request>
void fill_amortization_dictionary(google::TemplateDictionary& dict, Request& req)
{
  std::string tmp( req.POST("LoanAmt") );
  dict.SetValue("LoanAmt", tmp.empty() ? "$250,000" : tmp);

  tmp = req.POST("YearlyIntRate");
  dict.SetValue("YearlyIntRate", tmp.empty() ? "6.000" : tmp);

  boost::array<std::string, 8> year_opts
    = {{ "5", "7", "10", "20", "30", "40", "50" }};
    
  BOOST_FOREACH(std::string& year, year_opts)
  {
    dict.SetValueAndShowSection("TermYrs", year, "SELECT_TERM_YEARS");
  }

  if (req.POST("Amortize").empty())
    dict.ShowSection("NotAmortize");
  else
  {
    double P = boost::lexical_cast<double>(string_from_currency(req.POST("LoanAmt")));
    double i = boost::lexical_cast<double>(req.POST("YearlyIntRate")) / 1200;
    double n = boost::lexical_cast<double>(req.POST("TermYrs")) * 12;
    double monthly_payments = (P*i) / (1 - std::pow((1+i), -n));
    
    google::TemplateDictionary* sub_dict = dict.AddSectionDictionary("RegPmtSummary");
    sub_dict->ShowSection("RegPmtSummary");
    sub_dict->SetFormattedValue("MonthlyPmt", "%.2f", monthly_payments);

    dict.ShowSection("Amortize");
    dict.SetValue("SCRIPT_NAME", req.script_name());

    double balance = P;
    int row_num = 0;
    double interest;
    double principal_paid;
    double total_interest = 0;
    do{
      google::TemplateDictionary* sub_dict2 = dict.AddSectionDictionary("PaymentEntry");
      sub_dict2->ShowSection("PaymentEntry");
      sub_dict2->SetFormattedValue("Payment", "%.2f", monthly_payments);
      sub_dict2->SetIntValue("ROW_NUM", ++row_num);
      sub_dict2->SetIntValue("ROW_TYPE", (row_num % 2) + 1);
      interest = balance * i;
      total_interest += interest;
      sub_dict2->SetFormattedValue("InterestPaid", "%.2f", interest);
      principal_paid = monthly_payments - interest;
      sub_dict2->SetFormattedValue("PrincipalPaid", "%.2f", principal_paid);
      balance -= principal_paid; // Note: balance can increase.
      sub_dict2->SetFormattedValue("Balance", "%.2f", balance);

    }while(balance > 0);

    sub_dict->SetFormattedValue("RegPmt_TotalIntPd", "%.2f", total_interest);
    sub_dict->SetFormattedValue("RegPmt_TotalPmts", "%.2f", total_interest + P);
  }
}

template<typename Request>
int write_amortization_template(Request& req, response& resp)
{
  google::TemplateDictionary dict("amortization");

  fill_amortization_dictionary(dict, req);

  google::Template* tmpl
    = google::Template::GetTemplate("amortization.tpl", google::STRIP_WHITESPACE);

  boost::high_resolution_timer t;

  std::string h("Content-type: text/html\r\n\r\n");
  write(req.client(), buffer(h));

  std::string arg(req.GET("arg"));

  if (arg == "1")
  {
    std::string output;
    tmpl->Expand(&output, &dict);
    resp<< output;
  }else
  if (arg == "2")
  {
    std::string output;
    tmpl->Expand(&output, &dict);
    write(req.client(), buffer(output));
  }else
  if (arg == "3")
  {
    std::string s;
    std::vector<boost::asio::const_buffer> out;

    tmpl->Expand(&s, &out, &dict);
    write(req.client(), out);
  }else
  {
    resp<< "Error!";
    return 1;
  }
   //output.clear();
      //<< "<b>" << t.elapsed() << "</b><p />"
      //<< output << "<p /><p />"
      //<< "<b>" << t.elapsed() << "</b><p />";
  //}
 
  //std::cout<<
  //  "<pre>\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\nSTARTING OTHER THINGY\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n</pre>"
  //  ;
/*
    std::string s;
    std::vector<boost::asio::const_buffer> out;
for(int i=0; i < 50000; ++i) {
   // boost::high_resolution_timer t;

    tmpl->Expand(&s, &out, &dict);
    out.clear();
    s.clear();
    //std::cout<< "<b>" << t.elapsed() << "</b><p />";

      //<< "<p /><p />"
      //<< "<b>" << t.elapsed() << "</b><p />";
  }
 

  std::cerr<< "a took " << std::setiosflags(std::ios::fixed) << std::setprecision(5) << a/100000 << " secs<p />"
           << "b took " << b/100000 << " secs<p />";

  
  std::cerr
    << "Content-type: text/html\r\n\r\n"
    << output << "<p /><p />";
  
  resp<< content_type("text/html");
  resp.flush(req.client());
  write(req.client(), buffer(output));
  */
  return 0;
}

int handle_request(acceptor& a)
{
  boost::system::error_code ec;

  //std::ofstream of("/var/www/log/fcgi_reaccept.txt");
  using std::endl;
  //std::cerr<< "Eh?" << endl;
  //of<< "Opening request" << endl;

  request req(a.protocol_service());
 
  int ret = 0;
  int num = 0;
  while(!ec && ret == 0)
  {
    response resp;
    ++num;
    //std::cerr<< endl << endl << "request num := " << num << endl << endl;
    //of<< "Accepting now" << endl;
    a.accept(req);
    //of<< "Loading" << endl;
    req.load(true);

    resp<< content_type("text/html")
        << "map size := " << req.POST().size() << "<p>";
  
    //of<< "Writing template" << endl;
    ret = write_amortization_template(req, resp);

    //of<< "Sending" << endl;
    resp.send(req.client(), ec);
    //of<< "Closing" << endl;
    ret = ret ? ret : req.close(resp.status(), 0,  ec);
    //of<< "ok. ec := " << ec.message() << endl;
    //return 1;
  }
  return ret;
}

void accept_requests(acceptor& a)
{
  for(;;)
  {
    if (handle_request(a))
      break;
  }
}

int main()
{
  try{
  //std::cout
  //  << "Content-type: text/html\r\n\r\n";
  using std::endl;
    service s;
  std::cerr<< "Eh1?" << endl;
    acceptor a(s, true);
    //a.assign(boost::asio::ip::tcp::v4(), 0);
  std::cerr<< "Eh2?" << endl;

    accept_requests(a);
    
    return 0;

  }catch(boost::system::error_code& err){
    std::cerr<< "CGI error(" << err.value() << "): " << err.message() << std::endl;
  }catch(boost::system::system_error& err){
    std::cerr<< "System error(" << err.code() << "): " << err.what() << std::endl;
  }catch(...){
    std::cerr<< "ERROR!! BOOM!" << std::endl;
  }
}

