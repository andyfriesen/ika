<?php

include "includes.php";

function DisplayUserStats($view)
{
    global $user, $editable, $layouts, $layoutNames;
    
    // gather some stats
    $postcount = mysql_fetch_row(
        mysql_query("SELECT count(id) FROM board WHERE name='$view'")
    ) or 0;

    $threadcount = mysql_fetch_row(
        mysql_query("SELECT count(id) FROM board WHERE name='$view' AND parentid=0")
    ) or 0;
    
    $articlecount = mysql_fetch_row(
        mysql_query("SELECT count(id) FROM articles WHERE author='$view'")
    ) or 0;
    
    $filecount = mysql_fetch_row(
        mysql_query("SELECT count(id) FROM files WHERE author='$view'")
    ) or 0;

    if ($postcount) { $postcount = $postcount[0]; }
    if ($threadcount) { $threadcount = $threadcount[0]; }
    if ($articlecount) { $articlecount = $articlecount[0]; }
    if ($filecount) { $filecount = $filecount[0]; }

    $email = ObfuscateEmail($user["email"]);
    
    echo "<table class='box'>";
    echo "<tr><th class='main' colspan='2'>Statistics</th></tr>";
    echo "<tr><th colspan='2'>Contributions</th></tr>";
    echo "<tr><td width='20%'>Posts</td><td>$postcount</td></tr>";
    echo "<tr><td>Threads</td><td>$threadcount</td></tr>";
    echo "<tr><td>Articles</td><td>$articlecount</td></tr>";
    echo "<tr><td>Files</td><td>$filecount</td></tr>";
    
    if (!$editable)
    {
        echo "<tr><th colspan='2'>User Info</th></tr>";
        echo "<tr><td>Status</td><td>", $user["admin"] ? "Administrator" : "User", "</td></tr>";
        echo "<tr><td>Email</td><td><a href='mailto:$email'>$email</a></td></tr>";
        echo "<tr><td>Signature</td><td>$user[signature]</td></tr>";
        $key = array_search($user["layout"], $layouts);
        echo "<tr><td>Layout</td><td>", $layoutNames[$key], "</td></tr>";
    }    
    
    echo "</table>";
}

function DisplayUserPosts($view)
{
    $result = mysql_query("SELECT id, subject, date, time FROM board WHERE name='$view' AND deleted=0 ORDER BY date DESC, time DESC LIMIT 8");
    
    echo "<table class='box' style='margin-bottom: 0px'>";
    echo "<tr><th class='main' colspan=3>Recent Posts by ", $view, "</th></tr>";
    if (mysql_num_rows($result))
        while ($row = mysql_fetch_array($result))
            echo "<tr><td width='20%'>", $row["date"], "</td><td><a href='board.php?post=", $row["id"], "'>", $row["subject"], "</td><td width='15%'>", $row["time"], "</td></tr>";
    else
        echo "<tr><td>No posts.</td></tr>";
    echo "</table>";
}

function DisplayUserArticles($view)
{
    $result = mysql_query("SELECT id, title, date FROM articles WHERE author='$view' AND queued=0 ORDER BY date DESC LIMIT 8");
    
    echo "<table class='box' style='margin-bottom: 0px'>";
    echo "<tr><th class='main' colspan=2>Articles by ", $view, "</th></tr>";
    if (mysql_num_rows($result))
        while ($row = mysql_fetch_array($result))
            echo "<tr><td width='20%'>", $row["date"], "</td><td><a href='articles.php?view=", $row["id"], "'>", $row["title"], "</td></tr>";
    else
        echo "<tr><td>No articles.</td></tr>";
    echo "</table>";
}

function DisplayUserFiles($view)
{
    global $fileCategory;
    
    $result = mysql_query("SELECT filename, name, category, date FROM files WHERE author='$view' AND queued=0 ORDER BY date DESC LIMIT 8");
    
    echo "<table class='box' style='margin-bottom: 0px'>";
    echo "<tr><th class='main' colspan=3>Files by ", $view, "</th></tr>";
    if (mysql_num_rows($result))
        while ($row = mysql_fetch_array($result))
            echo "<tr><td width='20%'>", $row["date"], "</td><td><a href='", $row["filename"], "'>", $row["name"], "</td><td width='20%'>", $fileCategory[(int) $row["category"]], "</td></tr>";
    else
        echo "<tr><td>No files.</td></tr>";
    echo "</table>";
}

# --------------------------------

include "bin/main.php";

GenerateHeader("User Profile");

VerifyLogin();

if (!isset($view) and isset($_username)) {
    $view = $_username;
}

$editable = $view == $_username;

$user = GetUserInfo($view);

if ($user)
{

    StartBox($view);
    
    echo "<table class='box'>";
    
    echo "<tr><td class='blank' width='30%' style='vertical-align: top'>";
    DisplayUserStats($view);
    echo "</td>";
    
    echo "<td class='blank' width='70%' style='vertical-align: 'top'>";
    DisplayUserPosts($view);
    DisplayUserArticles($view);
    DisplayUserFiles($view);
    echo "</td></tr>";
    
    echo "</table>";
    
    EndBox();
    
    
    if ($editable) 
    {
        StartBox("Edit Profile");
        CreateForm("$PHP_SELF",
            "Name",      "static",  $_username,
            "Email",     "input",   $user["email"],
            "Layout",    "select",  $layouts, $layoutNames, $user["layout"],
            "Signature", "text",    NukeHTML($user["signature"]),
            "Submit",    "submit", "submit"
        );
        EndBox();
    }
}
else
{
    echo "<p>User does not exist.</p>";
}

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

EndBox();

?>