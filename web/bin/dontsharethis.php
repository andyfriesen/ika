<?php

# This PHP file has stuff that is best left private.  Passwords and such.

function ConnectToDB() {
  global $db;
  $db = mysql_connect("localhost", "ika", "ikapassword");
  mysql_select_db("i38780_ika");
}

$filedir = "files/";

?>
