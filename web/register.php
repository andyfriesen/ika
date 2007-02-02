<?php

include "bin/main.php";

GenerateHeader("Register");

StartBox();
do
{
    if (isset($register))
    {
        echo "<p>TEST $Username</p>";
        $Username = htmlspecialchars($Username);

        $result = mysql_query("SELECT count(*) the_count FROM users WHERE name='$username'");
        $row = mysql_fetch_array($result);
        if ($row["the_count"])
        {
            Error("A user by that name already exists.");
            unset($register);
            break;
        }

        if ($Password != $Retype)
        {
            Error("The passwords don't match!  Please try again.");
            unset($register);
            break;
        }

        $query = "INSERT INTO users (name, passwd) "
               . "values ('$Username', sha1('$Password'))";
        $result = mysql_query($query);
        if (!$result)
        {
            MySQL_NonFatalError();
            unset($register);
            break;
        }

        // Success!
        echo "<p>You are registered now.</p>";
        echo '<p><a href="index.php">Return to the main page.</a></p>';
    }
}
while (False);

if (!isset($register))
{
    if (!isset($Username))
        $Username = "";
    if (!isset($Password))
        $Password = "";
    if (!isset($Retype))
        $Retype = "";

    CreateForm("$PHP_SELF?register=1",
        "Username", "input",    $Username,
        "Password", "password", $Password,
        "Retype",   "password", $Retype,
        "Submit",   "submit",   ""
    );
}
EndBox();

?>