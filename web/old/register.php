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

        if ($Password != $Retype)
        {
            Error("The passwords don't match!  Please try again.");
            unset($register);
            break;
        }
        $result = mysql_query("SELECT * FROM users WHERE name='$username'");
        $row = mysql_fetch_array($result);
        if ($row)
        {
            Error("A user by that name already exists.");
            unset($register);
            break;
        }

        $query = "INSERT INTO users (name, password) "
               . "values ('$Username', '$Password')";
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