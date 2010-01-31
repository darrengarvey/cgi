<html>
<head>
  <link rel="stylesheet" type="text/css" href="/amortization.css">

  <title>Fixed Mortgage Loan Calculator</title>
  <script type="text/javascript" language="JavaScript1.2" src="/amortization.js"></script>
</head>

<body>

{{!--Amortization form}}
<form name="AmortBrowser" action="{{SCRIPT_NAME}}" method="post">
  <input type="hidden" name="Amortize" value="1">
  <table width="550" border="0" cellpadding="0" cellspacing="1" class="tblbase">
    <tr>
      <td colspan=4 class=tblhead align=center>FIXED MORTGAGE LOAN CALCULATOR</td>
    </tr>
    <tr>
      <td colspan=2 class=tblhead align=center>Loan Information</td>
      <td colspan=2 class=tblhead align=center>Prepayment Information (optional)</td>
    </tr>
    <tr>
      <td width="250" class="td1">Mortgage Amount</td>
      <td class="td2">
        <input name="LoanAmt" type="text" onBlur="this.value=formatCurrency(this.value);" value="{{LoanAmt}}" size="10">
      </td>
      <td width="250" class="td1">Prepayment Frequency</td>
      <td class="td2">
        <select name="PrePmtFreq" {{PrePmtFreqInvalid}}>{{PrePmtFreq}}</select>
      </td>
    </tr>
    <tr>
      <td class="td1">Interest (APR)</td>
      <td class="td2">
        <input name="YearlyIntRate" type="text" value="{{YearlyIntRate}}" size="10">
      </td>
      <td class="td1">Prepayment Amount</td>
      <td class="td2">
        <input name="PrePmtAmt" type="text" value="{{PrePmtAmt}}" size="17" {{PrePmtFreqInvalid}}>
      </td>
    </tr>
    <tr>
      {{!-- Length of Loan selection list }}
      <td class="td1">Length of Loan</td>
      <td class="td2">
        <select name="TermYrs">
          {{#SELECT_TERM_YEARS}}
            <option value="{{TermYrs}}">{{TermYrs}} Years</option>
          {{/SELECT_TERM_YEARS}}
        </select>
      </td>
      <td class="td1">Start with Payment</td>
      <td class="td2">
        <input name="PrePmtBegin" type="text" value="{{PrePmtBegin}}" size="17">
      </td>
    </tr>
    <tr>
      <td class="td1"></td>
      <td class="td2">
        <input type="submit" value="Calculate">
      </td>
    </tr>

    {{#NotAmortize}}
    <tr>
      <td colspan="4" class="td2"><span class="small">
        DISCLAIMER: The figures above are based upon conventional program guidelines.<br>
        Calculations by this tool are believed to be accurate, yet are not guaranteed. Further review is<br>
        necessary to obtain an exact qualification.  If you have less than 20% equity in your home, a monthly<br>
        mortgage insurance payment may be required.
      </span></td>
    </tr>
    {{/NotAmortize}}

  </table>
</FORM>

{{!--Prepayment summary table}}
{{#PrePmtSummary}}
  <table width="550" border="0" cellpadding="0" cellspacing="1" class="tblbase">
    <tr>
      <td colspan=4 class=tblhead align=center>PREPAYMENT vs REGULAR PAYMENT</td>
    </tr>
    <tr>
      <td colspan=2 class=tblhead align=center>Prepayment Results</td>
      <td colspan=2 class=tblhead align=center>Regular Payment Results</td>
    </tr>
    <tr>
      <td class="td1">Number of months to pay loan</td>
      <td class="td2" width="125">
        <center><b>{{PrePmt_iPmtNo}}</b></center>
      </td>
      <td class="td1"></td>
      <td class="td2" width="125">
        <center><b>{{RegPmt_iPmtNo}}</b></center>
      </td>
    </tr>
    <tr>
      <td class="td1">Total Interest Paid</td>
      <td class="td2" align="right">{{PrePmt_TotalIntPd}}</td>
      <td class="td1"></td>
      <td class="td2" align="right">{{RegPmt_TotalIntPd}}</td>
    </tr>
    <tr>
      <td width="250" class="td1">Total Payments</td>
      <td class="td2" align="right">{{PrePmt_TotalPmts}}</td>
      <td width="250" class="td1"></td>
      <td class="td2" align="right">{{RegPmt_TotalPmts}}</td>
    </tr>
    <tr>
      <td colspan="4" class="td2"><span class="small">
        <br>
        By applying the prepayments in the schedule above, you would <u>realize these savings</u>:
        <ul>
          <li>
            You pay the <b>{{RegPmt_term_to_words}}</b> loan in <b>{{PrePmt_term_to_words}}</b>
            <br><br>
          <li>The total loan payments will be <b>{{PrePmt_TotalPmts}}</b> instead of <b>{{RegPmt_TotalPmts}}</b>
        </ul>
      </span></td>
    </tr>
  </table>
  <br>
{{/PrePmtSummary}}

{{#RegPmtSummary}}
  {{!--Regular Payment summary table}}
  <table width="550" border="0" cellpadding="0" cellspacing="1" class="tblbase">
    <tr>
      <td colspan=4 class=tblhead align=center>REGULAR PAYMENT SUMMARY</td>
    </tr>
    <tr>
      <td class="td1">Monthly Payment</td>
      <td class="td2" width="125" align="right"><b>${{MonthlyPmt}}</b></td>
    </tr>
    <tr>
      <td class="td1">Total Interest Paid</td>
      <td class="td2" width="125" align="right">${{RegPmt_TotalIntPd}}</td>
    </tr>
    <tr>
      <td class="td1">Total Payments</td>
      <td class="td2" width="125" align="right">${{RegPmt_TotalPmts}}</td>
    </tr>
  </table>
  <br>
{{/RegPmtSummary}}

{{!--Amortization table}}

{{#Amortize}}
  <table border=0 cellpadding=0 cellspacing=1 width=550 class="tblbase">
    <tr>
      <td colspan=6 class=tblhead align=center>AMORTIZATION TABLE</td>
    </tr>
    <td class=tblhead align=center>Payment</td>
    <td class=tblhead align=center>Mo. Payment</td>
    <td class=tblhead align=center>Interest Pd.</td>
    <td class=tblhead align=center>Principal Pd.</td>
    <td class=tblhead align=center>New Balance</td>
    {{#PaymentEntry}}
      <tr class=row{{ROW_TYPE}}>
        <td ALIGN=CENTER>{{ROW_NUM}}</td>
        <td>${{Payment}}</td>
        <td>${{InterestPaid}}</td>
        <td>${{PrincipalPaid}}</td>
        <td>${{Balance}}</td>
      </tr>
    {{/PaymentEntry}}
  </table>
{{/Amortize}}

</body>
</html>

