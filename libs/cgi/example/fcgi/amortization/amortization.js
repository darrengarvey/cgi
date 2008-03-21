function printable()
{
	document.temps.submit();
	
}

function clearForm(form) {
  form.income.value = "";
  form.auto.value = "";  
  form.rate.value="";
  form.payment.value = "";
  form.amount.value = "";
}

function housingRatio(income, insurance) {	
	housing = eval(income * .28)
  return housing;
}

function debtRatio(income, auto) {	
	debt = eval(income * .36) - auto;
  return debt;
}

function checkForm(toCheck) {	
  isNum = true;
  for (j = 0; j < toCheck.length; j++) {
    if (((((toCheck.substring(j,j+1) < "0") || (toCheck.substring(j,j+1) > "9")) && (toCheck.substring(j,j+1) != ",")) 
		|| (toCheck.substring(j,j+1) > "9")) && (toCheck.substring(j,j+1) != ".")){
      isNum = false;
      }
    }
  if ((isNum == false) || (toCheck.length == 0) || (toCheck == null)) {
  alert("Please enter only numerical data in all fields.");
  return false;
  }
  else {
  return true;
  }
}     
function computeForm(form) {
	basicedit();

	income = replaceChars(form.income.value);
	income = replaceChar(income);
	if (income <= "0"){ 
		alert("You need to be employed to finance a house!")
		form.payment.value = "0";
		form.payment2.value = "0";
		form.amount.value = "0";
		form.amount2.value = "0";
		return false;}	
	auto = replaceChars(form.auto.value);
	auto = replaceChar(auto);
	var a = form.rate.value;
	var a = replacePct(a);

	if (a > 20 )	
	{	
		alert ("Interest Rate must not be greater than " + 20 + " percent");			
		form.rate.value=addPct('20');		
		a = form.rate.value;
		a = replacePct(a);	
	}
	else
	if (a < 1 )	
	{	
		alert ("Interest Rate must be greater than " + 1 + " percent");			
		form.rate.value=addPct('1');		
		a = form.rate.value;
		a = replacePct(a);	
	}			
	
if (checkForm(income) && checkForm(auto) && checkForm(a)){	
    housingRatioResult = Math.round(housingRatio(income));
    debtRatioResult = Math.round(debtRatio(income, auto));
	
	if (debtRatioResult <= "0") 
	{
		alert("Either your income is too low or your debt is too high!");
		form.payment.value = "0";
		form.payment2.value = "0";
		form.amount.value = "0";
		form.amount2.value = "0";
		return false;
	}	
	
      if (housingRatioResult>debtRatioResult) {
		form.payment.value = formatCurrency(debtRatioResult);
		form.payment2.value = formatCurrency(debtRatioResult);
      }
      else {
		form.payment.value = formatCurrency(housingRatioResult);
		form.payment2.value = formatCurrency(housingRatioResult); 
      }
	
    
    var c = debtRatioResult;	
    var d = parseFloat(a / 1200); 
    var f = parseFloat(1 + d);
	var f2 = parseFloat(1 + d);
    var g = parseFloat(Math.pow(f, 360));
	var g2 = parseFloat(Math.pow(f2, 180));
    var h = parseFloat(1 / g);
	var h2 = parseFloat(1 / g2);
    var i = parseFloat(1 - h);
	var i2 = parseFloat(1 - h2);
    var j = parseFloat(i / d);
	var j2 = parseFloat(i2 / d);
    var k = parseFloat(c * j);
	var k2 = parseFloat(c * j2);
    form.amount.value = formatCurrency(Math.round(k));
	form.amount2.value = formatCurrency(Math.round(k2));


}
return;
}


function replaceChar(entry) {
	out = "$"; 
	add = ""; 
	temp = "" + entry;
	while (temp.indexOf(out)>-1) {
		pos= temp.indexOf(out);
		temp = "" + (temp.substring(0, pos) + add + 
		temp.substring((pos + out.length), temp.length));
	}
	return temp;
}

function replacePct(entry) {
	out = "%"; 
	add = ""; 
	temp = "" + entry;
	while (temp.indexOf(out)>-1) {
		pos= temp.indexOf(out);
		temp = "" + (temp.substring(0, pos) + add + 
		temp.substring((pos + out.length), temp.length));
	}
	return temp;
}	
	
function replaceChars(entry) {
	out = ","; 
	add = ""; 
	temp = "" + entry;
	while (temp.indexOf(out)>-1) {
		pos= temp.indexOf(out);
		temp = "" + (temp.substring(0, pos) + add + 
		temp.substring((pos + out.length), temp.length));
	}
	return temp;
}

function formatCurrency(num) {			
			num=replaceChar(num);
			num=replaceChars(num);
		if ( checkForm(num));
{
			num = num.toString().replace(/$|,/g,'');
			if(isNaN(num)) num = "0";
			num = Math.floor(num).toString();
		for (var i = 0; i < Math.floor((num.length-(1+i))/3); i++) 
			num = num.substring(0,num.length-(4*i+3))+','+num.substring(num.length-(4*i+3));
			 
	return ('$' + num );	
}
}

function basicedit()
{
	if 	(!IsMoney(document.temps.income.value) || document.temps.income.value.length ==0)
		document.temps.income.value='$3,500';
		
	if 	(!IsMoney(document.temps.auto.value) || document.temps.auto.value.length ==0)
		document.temps.auto.value='../../btn-red27px-R.gif';		

	if 	(!IsPct(document.temps.rate.value) || document.temps.rate.value.length ==0)
		document.temps.rate.value='7.5%';		
}

function IsMoney(val)
	{
		var number="0123456789$,.";

		for (var i=0;i<val.length;i++)
		{
			if (number.indexOf(val.charAt(i)) == -1)
			{
				return false;
			}
		}
		return true;
	}

function IsNumber(val)
	{
		var number="0123456789.";

		for (var i=0;i<val.length;i++)
		{
			if (number.indexOf(val.charAt(i)) == -1)
			{
				return false;
			}
		}
		return true;
	}
	
function IsPct(val)
	{
		var number="0123456789.%,";

		for (var i=0;i<val.length;i++)
		{
			if (number.indexOf(val.charAt(i)) == -1)
			{
				return false;
			}
		}
		return true;
	}

function addPct(pct) {
			pct=replacePct(pct);
			pct=parseFloat(pct);		
		if (checkForm(pct));
	{
			return (pct + '%');
		}
}

