<?php

function ShowPost($result, $row)
{
    MakePost(
        $row["subject"], $row["text"], $row["name"], LVL_COMPLEX_HTML,
        $row["date"],
        $row["time"]
    );
}


function ShowNews($showall)
{
    $result = mysql_query("SELECT * FROM news ORDER BY id DESC;")
             or MySQL_FatalError();

    StartBox('news');
    if (!$showall)
        $numposts = 5;
    else
        $numposts = 9999;
    for ($i = 0; $i < $numposts; $i++)
    {
        $row = mysql_fetch_array($result);
        if (!$row)
            break;
        ShowPost($result, $row);
    }
    EndBox();
}


function EditNews()
{
    global $_username, $submit, $view, $Subject, $Text;

    StartBox();
    echo '<p style="text-align: center">Edit news.</p>';  # CHANGE TO CSS!
    
    $user = GetUserInfo($_username);
    if (!$user["admin"])
        FatalError("You are not an administrator.");
    
    $result = mysql_query("SELECT id, subject FROM news ORDER BY id DESC");
    
    while ($post = mysql_fetch_array($result))
    {
        echo "<p><a href='$PHP_SELF?view=$post[id]&amp;edit=1'>";
        echo "$post[subject]</a></p>";
    }
    EndBox();

    StartBox();
    if (isset($submit))
    {
        $query = "UPDATE news SET Subject='$Subject', text='$Text' WHERE id=$view";
        $result = mysql_query($query) or MySQL_FatalError();
        Notice("News updated.");
        return;
    }
    else if (isset($view))
    {
        $result = mysql_query("SELECT name, subject, text FROM news WHERE id=$view");
        $post = mysql_fetch_array($result);
    
        CreateForm("$PHP_SELF?view=$view&amp;edit=1&amp;submit=1",
            "Name",    "static", $post["name"],
            "Subject", "input",  NukeHTML($post["subject"]),
            "Text",    "text",   NukeHTML($post["text"]),
            "Submit",  "submit", ""
        );
    
        echo "<p><a href='$PHP_SELF?view=$view&amp;destroy=1'>Delete this news update.</a></p>";
    }
    else
        echo "<p>Pick a news post to edit.</p>";
    EndBox();
}


function DeleteNews($id)
{
    if (!IsAdmin())
        FatalError("You are not an admin.");

    $query  = "DELETE FROM news WHERE id=$id";
    $result = mysql_query($query) or MySQL_FatalError();

    Notice("News post deleted.");
}


function AddNews()
{
    global $PHP_SELF, $_password, $_username, $Subject, $Text, $subject, $text,
           $submit;

    if (!IsAdmin())
        FatalError("You are not an administrator.");

    if (isset($submit))
    {
        $date = date("Y-m-d");
        $time = date("G:i:s");

        $query = "INSERT INTO news (subject, name, text, date, time) ".
                 "values ('$Subject', '$_username', '$Text', '$date', '$time')";
        $result = mysql_query($query) or MySQL_FatalError();

        StartBox();
        echo '<p>News article added.</p>';
        echo '<p><a href="index.php">Return to the news page.</a></p>';
        EndBox();
    }
    else
    {
        StartBox();
        CreateForm("$PHP_SELF?add=1&submit=1",
            "Name",    "static", $_username,
            "Subject", "input",  isset($subject)? $subject : "",
            "Text",    "text",   isset($text) ? $text : "",
            "Submit",  "submit", "");
        EndBox();
    }
}

# -----------------------------------------------------------------------------

include_once "bin/main.php";

GenerateHeader("News");

VerifyLogin();

echo '<p style="text-align: center;"><a href="board.php?post=5164">Click here for information on the ika revitalization project codenamed "ika: Redux".</a></p><br><br>';

if (isset($edit))
    EditNews();
else if (isset($add))
    AddNews();
else if (isset($destroy))
    DeleteNews($view);
else
    ShowNews(isset($showall));

?>