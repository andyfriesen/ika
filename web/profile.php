<?php

include "bin/main.php";

VerifyLogin();

if (!isset($view) and isset($_username)) {
    $view = $_username;
}

$editable = $view == $_username;

GenerateHeader("Profile for $view");

StartBox();

if (isset($submit) and $editable) {
    
    $Email = $safe_post["Email"];
    $Layout = $safe_post["Layout"];
    $Signature = $safe_post["Signature"];
    
    $query = "UPDATE users SET "
           . "email='$Email', layout='$Layout', signature='$Signature' "
           . "where name='$_username'";

    $result = mysql_query($query) or MySQL_FatalError();

    Notice("The record was updated successfully.");
}

$layouts = array(
    "default",
    "default2",
    "default3",
    "ika2",
    "clean",
    "cmethod",
    "pandemonium",
    "plain"
);

$layoutNames = array("Default layout &mdash; Shlock",
                     "andy's crazy staticy blue layout &mdash; andy",
                     "Shlock's old default layout &mdash; Shlock",
		     "Ear's unfinished proto-layout &mdash; Ear",
                     "Clean &mdash; JL",
                     "Crystal Method &mdash; Shlock",
                     "Pandemonium &mdash; Khross",
                     "Plain &mdash; andy"
);

$user = GetUserInfo($view);

if ($user)
{
    // gather some stats
    $postcount = mysql_fetch_row(
        mysql_query("SELECT count(id) FROM board WHERE name='$view'")
    ) or 0;

    $threadcount = mysql_fetch_row(
        mysql_query("SELECT count(id) FROM board WHERE name='$view' AND parentid=0")
    ) or 0;

    if ($postcount) {
        $postcount = $postcount[0];
    }

    if ($threadcount) {
        $threadcount = $threadcount[0];
    }

    echo "<table>";
    echo "<tr><th colspan='2'>Statistics</th></tr>";
    echo "<tr><td>Post count</td><td>$postcount</td></tr>";
    echo "<tr><td>Thread count</td><td>$threadcount</td></tr>";
    if ($user["admin"]) {
         echo "<tr><td>Status</td><td>Administrator</td></tr>";
    }
    echo "</table>";
    echo "<br />";

    if ($editable) {
        CreateForm("$PHP_SELF",
            "Header",    "header",  "Edit Profile",
            "Name",      "static",  $_username,
            "Email",     "input",   $user["email"],
            "Layout",    "select",  $layouts, $layoutNames, $user["layout"],
            "Signature", "text",    NukeHTML($user["signature"]),
            "Submit",    "submit", "submit"
        );
    } else {
        $email = ObfuscateEmail($user["email"]);
        echo "<table>";
        echo "<tr><td>Email</td><td><a href='mailto:$email'>$email</a></td></tr>";
        echo "<tr><td>Signature</td><td>$user[signature]</td></tr>";
        echo "</table>";
    }
}
else
{
    echo "<p>User does not exist.</p>";
}

EndBox();

?>