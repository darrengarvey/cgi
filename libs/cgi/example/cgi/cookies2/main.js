function switch_method(ident) {
    var f = document.getElementById(ident);
    if (f.method == 'post') f.method = 'GET';
    else f.method = 'POST';
}
function switch_value(elem) {
    if (elem.value == 'Switch to POST') {
        elem.value = 'Switch to GET';
    }else{
        elem.value = 'Switch to POST';
    }
}
function switch_form(elem) {
    var g = document.getElementById('getform');
    var p = document.getElementById('postform');
    if (elem.value == 'Switch to POST') {
        p.style.visibility = 'visible';
        p.style.display = 'block';
        g.style.visibility = 'hidden';
        g.style.display = 'none';
        elem.value = 'Switch to GET';
    } else {
        g.style.visibility = 'visible';
        g.style.display = 'block';
        p.style.visibility = 'hidden';
        p.style.display = 'none';
        elem.value = 'Switch to POST';
    }
}

