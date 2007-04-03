<?php

function ShowPost($result, $row)
{
    MakePost(
        $row["subject"], $row["text"], $row["name"], LVL_COMPLEX_HTML,
        $row["date"],
        $row["time"]
    );
}


function ShowNews($showall) {
    $query = "SELECT * FROM news ORDER BY id DESC";

    if (!$showall) {
        $query .= " LIMIT 5";
    }

    $result = mysql_query($query)
             or MySQL_FatalError();

    StartBox("Most Recent News Entries");

    while ($row = mysql_fetch_array($result)) {
        StartBox();
        ShowPost($result, $row);
        EndBox();
    }
    EndBox();
}


function EditNews($view)
{
    global $_username, $submit, $view, $Subject, $Text, $safe_post;

    if (!$view)
    {
        StartBox("Instructions");
        echo "<p>Pick a news post to edit.</p>";
        EndBox();

        StartBox("All News Entries");

        $user = GetUserInfo($_username);
        if (!$user["admin"])
            FatalError("You are not an administrator.");

        $result = mysql_query("SELECT id, subject, name, date FROM news ORDER BY id DESC");

        $year = "0";

        echo "<table class='box'>";
        while ($post = mysql_fetch_array($result))
        {
            $thisYear = substr($post["date"], 0, 4);
            if ($thisYear != $year)
            {
                $year = $thisYear;
                echo "<tr><th class='main' colspan=3>$year</th></tr>";
                echo "<tr><th>Date</th><th>Subject</th><th>Author</th></tr>";
            }
            echo "<tr><td width='15%'>", $post["date"], "</td><td><a href='$PHP_SELF?view=", $post["id"], "&amp;edit=1'>", $post["subject"], "</td><td width='15%'>", FormatName($post["name"]), "</td></tr>";
        }
        echo "</table>";
        EndBox();
    }

    if (isset($safe_post["Preview"]))
    {
        $result = mysql_query("SELECT date, time FROM news WHERE id=$view");
        $post = mysql_fetch_array($result);

        StartBox("Preview News Entry");
        MakePost($_POST["Subject"], $_POST["Text"], $safe_post["Name"], LVL_COMPLEX_HTML,
        $post["date"],
        $post["time"]);
        EndBox();
    }
    if (isset($safe_post["Submit"]))
    {
        $query = "UPDATE news SET Subject='$Subject', text='$Text' WHERE id=$view";
        $result = mysql_query($query) or MySQL_FatalError();

        StartBox("Notice");
        echo "<p>News updated.</p>";
        echo '<p><a href="index.php">Return to the news page.</a></p>';
        EndBox();

        return;
    }
    else if (isset($view))
    {
        if (!isset($safe_post["Preview"]))
        {
            $result = mysql_query("SELECT name, subject, text FROM news WHERE id=$view");
            $post = mysql_fetch_array($result);
        }
        else
            $post = array("name" => $_POST["Name"], "subject" => $_POST["Subject"], "text" => $_POST["Text"]);

        StartBox("Edit News");
        CreateForm("$PHP_SELF?view=$view&amp;edit=1",
            "Name",    "hidden", $post["name"],
            "Subject", "input",  NukeHTML($post["subject"]),
            "Text",    "text",   NukeHTML($post["text"]),
            "PreSub",  "preview+submit", ""
        );
        EndBox();

        StartBox("Options");
        echo "<table><tr>";
        echo "<td><a class='button' href='$PHP_SELF?edit=1'>back to news list</a></td>";
        echo "<td><a class='button' href='$PHP_SELF?view=$view&amp;destroy=1'>delete</a></td>";
        echo "</tr></table>";
        EndBox();
    }
}


function DeleteNews($id)
{
    if (!IsAdmin())
        FatalError("You are not an admin.");

    $query  = "DELETE FROM news WHERE id=$id";
    $result = mysql_query($query) or MySQL_FatalError();

    StartBox("Notice");
    echo "<p>News post deleted.</p>";
    echo '<p><a href="index.php">Return to the news page.</a></p>';
    EndBox();
}


function AddNews()
{
    global $PHP_SELF, $_password, $_username, $safe_post;

    if (!IsAdmin())
        FatalError("You are not an administrator.");

    if (isset($safe_post["Submit"]))
    {
        $date = date("Y-m-d");
        $time = date("G:i:s");

        $Subject = $safe_post["Subject"];
        $Text = $safe_post["Text"];

        $query = "INSERT INTO news (subject, name, text, date, time) ".
                 "values ('$Subject', '$_username', '$Text', '$date', '$time')";
        $result = mysql_query($query) or MySQL_FatalError();

        StartBox("Notice");
        echo '<p>News article added.</p>';
        echo '<p><a href="index.php">Return to the news page.</a></p>';
        EndBox();
    }
    else if (isset($safe_post["Preview"]))
    {
        $subject = $safe_post["Subject"];
        $text = $safe_post["Text"];

        $date = date("Y-m-d");
        $time = date("G:i:s");

        StartBox("Preview News Entry");
        MakePost($_POST["Subject"], $_POST["Text"], $safe_post["Name"], LVL_COMPLEX_HTML,
        $date,
        $time);
        EndBox();

        StartBox("Write New News Entry");
        CreateForm("$PHP_SELF?add=1&submit=1",
            "Name",    "hidden", $_username,
            "Subject", "input",  isset($subject)? $subject : "",
            "Text",    "text",   isset($text) ? $text : "",
            "PreSub",  "preview+submit", "");
        EndBox();
    }
    else
    {
        StartBox("Write New News Entry");
        CreateForm("$PHP_SELF?add=1&submit=1",
            "Name",    "hidden", $_username,
            "Subject", "input",  isset($subject)? $subject : "",
            "Text",    "text",   isset($text) ? $text : "",
            "PreSub",  "preview+submit", "");
        EndBox();
    }
}

# -----------------------------------------------------------------------------

include_once "bin/main.php";

GenerateHeader("News");

VerifyLogin();

# echo '<p style="text-align: center;"><a href="board.php?post=5164">Click here for information on the ika revitalization project codenamed "ika: Redux".</a></p><br><br>';

if (isset($edit))
    EditNews($view);
else if (isset($add))
    AddNews();
else if (isset($destroy))
    DeleteNews($view);
else
    ShowNews(isset($showall));

?>
