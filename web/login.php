<?php

function Logout()
{
    global $_username, $_password, $logout;

    setcookie("_username", "", 0);
    setcookie("_password", "", 0);

    unset($_username);
    unset($_password);
    unset($logout);

    GenerateHeader("Authentication", "Anonymous", "Anonymous");
    
    StartBox("Notice");
    echo "<p>You are now logged out.</p>";
    echo "<p><a href='index.php'>Go back to the main page.</a>";
    EndBox();
    
    die();
}


function Login()
{
    global $safe_post, $_username, $_password;

    $Username = $safe_post["Username"];
    $Password = $safe_post["Password"];
    
    ConnectToDB();

    $sha1 = sha1($Password);

    $result = mysql_query("SELECT passwd='$sha1' AS pass_is_good FROM users WHERE name='$Username'");
    $row = mysql_fetch_array($result);

    //if ($Password == $row["password"])
    if ($row["pass_is_good"])
    {
        setcookie("_username", $Username, time() + (60 * 60 * 24) * 365);
        setcookie("_password", $sha1, time() + (60 * 60 * 24) * 365);

        $_username = $Username;
        $_password = $Password;

        GenerateHeader("Authentication", $_username, $sha1);
        
        StartBox("Notice");
        echo "<p>You are now logged in as $_username.</p>";
        echo "<p><a href='index.php'>Go back to the main page.</a>";
        EndBox();
        
        die();
    }
    else if ($row)
    {
        GenerateHeader("Authentication");
        
        Error("The password you entered is incorrect.");
    }
    else
    {
        GenerateHeader("Authentication");
        
        Error("The username you entered does not exist.");
    }
}

# -----------------------------------------------------------------------------

include "bin/main.php";

if (isset($logout) and isset($_username))
    Logout();
else if (isset($login) and isset($_POST["Submit"]))
    Login();
else
    GenerateHeader("Authentication");

if (!isset($_username))
{
    
    StartBox("Log In");
    
    CreateForm("login.php?login=1",
        "Username", "input",    "",
        "Password", "password", "",
        "Submit",   "submit",   "");
        
    EndBox();
}
else
{
    StartBox("Notice");
    echo "<p>You are already logged in as $_username.</p>";
    echo "<p><a href='index.php'>Go back to the main page.</a>";
    EndBox();
}
?>