<?php

include "bin/main.php";

GenerateHeader("Register");

do
{
    if (isset($register))
    {
        $Username = htmlspecialchars($Username);

        $result = mysql_query("SELECT count(*) AS the_count FROM users WHERE name='$Username'");
        $row = mysql_fetch_array($result);
        if ($row["the_count"] > 0)
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
            StartBox("Notice");
            MySQL_NonFatalError();
            EndBox();
            unset($register);
            break;
        }

        // Success!
        StartBox("Notice");
        echo "<p>You are registered now.</p>";
        echo '<p><a href="index.php">Return to the main page.</a></p>';
        EndBox();
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

    StartBox("Registration Form");
    CreateForm("$PHP_SELF?register=1",
        "Username", "input",    $Username,
        "Password", "password", $Password,
        "Retype",   "password", $Retype,
        "Submit",   "submit",   ""
    );
    EndBox();
}

?>