<?php

function Logout()
{
    global $_username, $_password, $logout;

    setcookie("_username", "", 0);
    setcookie("_password", "", 0);

    unset($_username);
    unset($_password);
    unset($logout);

    GenerateHeader("Login");
    Notice("You are now logged out.");
}


function Login()
{
    global $Username, $Password, $_username, $_password;

    ConnectToDB();

    $result = mysql_query("SELECT * FROM users WHERE name='$Username'");
    $row = mysql_fetch_array($result);

    if ($Password == $row["password"])
    {
        setcookie("_username", $Username, time() + (60 * 60 * 24) * 365);
        setcookie("_password", $Password, time() + (60 * 60 * 24) * 365);

        $_username = $Username;
        $_password = $Password;

        GenerateHeader("Login");
        Box("You are now logged in as $_username.</p>" .
            "<p><a href='index.php'>Go back to the main page.</a>");   # THIS NEEDS WORK!
    }
    else
    {
        GenerateHeader("Login");
        FatalError("The password you entered is incorrect.");
    }
}

# -----------------------------------------------------------------------------

include "bin/main.php";

if (isset($logout))
    Logout();
else if (isset($login))
    Login();
else if (!isset($login) and !isset($_username))
{
    GenerateHeader("Login");
    StartBox();
    CreateForm("login.php?login=1",
        "Username", "input",    "",
        "Password", "password", "",
        "Submit",   "submit",   "");
    EndBox();
}

?>