//                     -- main.hpp --
//
//           Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
//
//[fcgi_amort
//
// Amortization Calculator
// -----------------------
//
// This file uses Google cTemplate to show the benefits of using an
// HTML template engine. The code isn't commented yet, but should be 
// *reasonably* self-explanatory.
//
// It is a very basic amortization calculator.
//
#include <iostream>
#include <iomanip>
#include <boost/cgi/fcgi.hpp>
#include <boost/algorithm/string/regex.hpp>
#include <ctemplate/template.h>

using namespace boost::fcgi;

/// Convert a string like '$250,000' into one like '250000'.
std::string string_from_currency(std::string amt)
{
  // this is much too hardcore, but it works fine...
  boost::algorithm::erase_all_regex(amt, boost::regex("[$, ]"));
  return amt;
}

/// This function fills a dictionary.
template<typename Request>
void fill_amortization_dictionary(
  ctemplate::TemplateDictionary& dict, Request& req)
{
  dict.SetValue("LoanAmt", req.post.count("LoanAmt")
      ? "$250,000" : req.post["LoanAmt"]);
  dict.SetIntValue("YearlyIntRate", req.post.pick("YearlyIntRate", 6.000));

  boost::array<std::string, 8> year_opts
    = {{ "5", "7", "10", "20", "30", "40", "50" }};
    
  BOOST_FOREACH(std::string& year, year_opts)
  {
    dict.SetValueAndShowSection("TermYrs", year, "SELECT_TERM_YEARS");
  }

  if (req.post["Amortize"].empty())
    dict.ShowSection("NotAmortize");
  else
  {
    double P = boost::lexical_cast<double>(
      string_from_currency(req.post["LoanAmt"]));
    double i = req.post.pick("YearlyIntRate", 1.0) / 1200;
    double n = req.post.pick("TermYrs", 1.0) * 12;
    double monthly_payments = (P*i) / (1 - std::pow((1+i), -n));
    
    ctemplate::TemplateDictionary* sub_dict
      = dict.AddSectionDictionary("RegPmtSummary");
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
      ctemplate::TemplateDictionary* sub_dict2
        = dict.AddSectionDictionary("PaymentEntry");
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

    sub_dict->SetFormattedValue(
      "RegPmt_TotalIntPd", "%.2f", total_interest);
    sub_dict->SetFormattedValue(
      "RegPmt_TotalPmts", "%.2f", total_interest + P);
  }
}

template<typename Request>
int write_amortization_template(Request& req, response& resp)
{
  ctemplate::TemplateDictionary dict("amortization");

  fill_amortization_dictionary(dict, req);

  ctemplate::Template* tmpl
    = ctemplate::Template::GetTemplate("../templates/amortization.html", ctemplate::STRIP_WHITESPACE);

  std::string h("Content-type: text/html\r\n\r\n");
  write(req.client(), buffer(h));

  int arg = req.get.pick("arg", 2); // 2 is the default.

  // Different, but equivalent ways of writing the output.
  std::string output;
  switch (arg)
  {
  case 1:
    tmpl->Expand(&output, &dict);
    resp<< output;
    break;
  case 2:
    tmpl->Expand(&output, &dict);
    write(req.client(), buffer(output));
    break;
  case 3:
//    // This requires a modified version of Google.cTemplate, so it won't work.
//    std::string s;
//    std::vector<boost::asio::const_buffer> out;
//
//    tmpl->Expand(&s, &out, &dict);
//    write(req.client(), out);
    break;
  default:
    resp<< "Error!";
    return 1;
  }

  return 0;
}

int handle_request(acceptor& a)
{
  boost::system::error_code ec;

  request req(a.protocol_service());
 
  int ret = 0;
  int num = 0;
  while(!ec && ret == 0)
  {
    response resp;
    ++num;

    // Accepting on a closed request is fine (and more efficient than 
    // constantly creating/destructing request objects).
    a.accept(req);

    req.load(parse_all);

    resp<< content_type("text/html")
        << "map size := " << req.post.size() << "<p>";
  
    ret = write_amortization_template(req, resp);

    ret = commit(req, resp, 0, ec);
  }
  return ret;
}

int main()
{
  try{

    service s;
    acceptor a(s, 8010); // Listen on port 8010.

    for(;;)
    {
      // Keep handling requests until something goes wrong.
      // An exception will be thrown.
      if (handle_request(a))
        break;
    }
    
    return 0;

  }catch(boost::system::system_error& err){
    std::cout<< "Content-type: text/plain\r\n\r\n"
             << "Error (" << err.code() << "): " << err.what();
    return 0;
  }catch(std::exception& e){
    std::cout<< "Content-type: text/html\r\n\r\n"
             << "Exception caught: " << e.what();
    return 0;
  }catch(...){
    std::cout<< "Content-type: text/html\r\n\r\n"
             << "Unknown error!";    
  }
}
//]

