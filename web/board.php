<?php

function ShowThread($post) {
    global $PHP_SELF;

    echo "<a href='$PHP_SELF?post=$post[id]'>";
    echo NukeHTML($post["subject"]);
    echo '</a>';
    if (strlen($post["text"]) < 2)
        echo ' (<abbr title="no text">NT</abbr>) ';
    echo " &mdash; ", FormatName($post["name"]), " $post[time] $post[date]";
}


# displays replies to post id=$id
function ShowReplies($id) {
    $result = mysql_query("SELECT * FROM board WHERE parentid=$id AND deleted=0 ORDER BY id DESC;");
    if (!$result) {
        return;
    }

    $post = mysql_fetch_array($result);
    if ($post) {
        echo "<ul>";
        do {
            echo "<li>";
            ShowThread($post);
            ShowReplies($post["id"]);
            echo "</li>";
        } while ($post = mysql_fetch_array($result));

        echo "</ul>";
    }
}


function ShowPost($id) {
    
    global $admin, $_username, $PHP_SELF;

    $result = mysql_query("SELECT * FROM board WHERE id=$id")
              or FatalError("No post $id.");

    $result = mysql_fetch_array($result);

    $poster = GetUserInfo($result["name"]);
    
    $sig = "";
    if (strlen($poster["signature"]) > 2) {
        $sig .= NukeHTML($poster["signature"], LVL_BASIC_HTML);
    }
    
    $sig .= "&nbsp;&mdash; ".FormatName($result["name"]);  # CHANGE TO CSS!
    if ($admin and $result["ip"]) {
        $sig .= "<p>ip: $result[ip]</p>";
    }
    
    $text = $result["text"];
    if (strlen($text) < 2) {
        $text = "No text.";
    }
    
    StartBox("Post");
    MakeSigPost($result["subject"], $text, $sig, LVL_COMPLEX_HTML);
    EndBox();
    
    # This shouldn't fail, unless the database is set up wrong
    $blah = mysql_fetch_row(mysql_query("SELECT COUNT(id) FROM board WHERE parentid=$id AND deleted=0"));
    
    # fucking retarded.  I *must* make a temporary variable. :P
    if ($blah[0]) {
        StartBox("Replies");
        ShowReplies($id);
        EndBox();
    }

    $myPost = $_username == $result["name"];
    
    if ((!$result["locked"] and $myPost) or $admin or $result["parentid"] != 0) {
        StartBox("Options");
        echo '<table><tr>';
        if ($result["parentid"] != 0)
            echo '<td><a class="button" href="?post=', $result["parentid"], '">view parent</a></td>';
        echo "<td><a class='button' href='$PHP_SELF?edit=$id'>Edit</a></td>";
        echo "<td><a class='button' href='$PHP_SELF?delete=$id'>Delete</a></td>";

        if ($admin) {
            if (!$result["locked"]) {
                echo "<td><a class='button' href='$PHP_SELF?lock=$id'>Lock</a></td>";
            } else {
                echo "<td><a class='button' href='$PHP_SELF?unlock=$id'>Unlock</a></td>";
            }
        }

        echo "</tr></table>";
        EndBox();
    }

    StartBox("Write Reply");
    if ($result["locked"] and !$admin) {
        FatalError("This post has been locked and cannot be replied to.");
    } else if (strncmp($result["subject"], "Re: ", 4)) {
        ShowPostForm("Re: $result[subject]");
    } else {
        ShowPostForm($result["subject"]);
    }
    EndBox();
}


function ShowAllPosts($amount) {
    global $showall;

    // Temp hack so that pre-forum nuke threads do not show up.
    $query = "SELECT * FROM board WHERE parentid=0 AND deleted=0 AND date > '2006-01-01' ORDER BY id DESC";
    if (!$showall) {
        $query .= " LIMIT $amount";
    }

    StartBox("Posts");
    
    $result = mysql_query($query);
    if ($result) {
        while ($post = mysql_fetch_array($result)) {
            StartBox();
            echo "<ul><li>";
            ShowThread($post);
            ShowReplies($post["id"]);
            echo "</li></ul>";
            EndBox();
        }
    }

    EndBox();
    
    StartBox("Write New Post");
    ShowPostForm();
    EndBox();
}


function ShowNewestPosts($amount) {
    global $PHP_SELF;

    $result = mysql_query("SELECT * FROM board WHERE deleted=0 ORDER by id DESC LIMIT $amount;")
              or MySQL_FatalError();

    echo '<table>';
    echo "<tr><th>Subject</th><th>Poster</th><th>Date</th></tr>";

    while ($post = mysql_fetch_array($result)) {
        echo '<tr>';
        echo "<td><a href='$PHP_SELF?post=$post[id]'>", NukeHTML($post[subject]), "</a></td>";
        echo "<td>$post[name]</td><td>$post[date]</td>";
        echo '</tr>';
    }

    echo "</table>";
}


function ShowPostForm($title="") {
    global $PHP_SELF, $_username, $Subject, $Text, $post;
    
    CreateForm("$PHP_SELF?add=$post",
        "Name",    "static", isset($_username) ? $_username : "Anonymous",
        "Subject", "input",  $title ? NukeHTML($title) : (isset($Subject) ? NukeHTML($Subject) : ""),
        "Text",    "text",   isset($Text) ? NukeHTML($Text) : "",
        "Submit",  "submit", "",
        "Posting instructions", "static",
        "Use the <strong>&lt;code&gt;</strong> tag around blocks of code and ".
        "<strong>&lt;pre&gt;</strong> around blocks of preformatted text that".
        " are not code. The forum also recognizes <strong>&lt;em&gt;</strong>".
        " for <em>emphasis</em>."
    );
}


function AddPost($parentid, $subject, $name, $text) {
    global $REMOTE_ADDR, $_username;

    $Subject = trim($subject) or FatalError("Please enter a subject.");
    
    $Text = trim($text);

    if (isset($_username)) {
        VerifyLogin();
        $name = $_username;
    } else {
        $name = "Anonymous";
    }

    $date = date("Y-m-d");
    $time = date("G:i:s");

    $query = "INSERT INTO board "
           . "(parentid, subject, name, ip, text, date, time) values "
           . "('$parentid', '$Subject', '$name', '$REMOTE_ADDR', '$Text', "
           . "'$date', '$time');"
    ;
    
    $result = mysql_query($query) or MySQL_FatalError();
    Notice("Post added successfully.");
}


function UpdatePost($id, $subject, $text) {
    

    $result = mysql_query("SELECT * FROM board WHERE id=$id")
              or MySQL_FatalError();
    $post = mysql_fetch_array($result);

    if (!$admin and ($post["locked"] or $post["deleted"])) {
        FatalError("This post is locked and cannot be edited.");
    }

    if (!$admin and $_username != $post["name"]) {
        FatalError("You do not have sufficient permission to edit this post.");
    }

    $result = mysql_query("UPDATE board SET subject='$subject', text='$text' where id=$id")
              or MySQL_FatalError();

    Notice("The post was edited successfully");
}


function DeletePost($delete) {
    global $admin, $_username;

    if (!$admin) {
        $result = mysql_query("SELECT name FROM board WHERE id=$delete");
        $post = mysql_fetch_array($result);
        if ($post["locked"]) {
            FatalError("The post is locked and cannot be deleted.");
        }
        if ($post["name"] != $_username) {
            FatalError("You do not have sufficient permission to delete this post.");
        }
    }

    //$result = mysql_query("DELETE FROM board WHERE id=$delete OR parentid=$delete");
    $result = mysql_query("UPDATE board SET deleted=1 WHERE id=$delete OR parentid=$delete");

    Notice("The post and all of its replies have been removed.");
    die();
}


function EditPost($id) {
    global $PHP_SELF, $_username, $Subject, $Text, $post;

    $result = mysql_query("SELECT * FROM board WHERE id=$id")
              or MySQL_FatalError();
    $post = mysql_fetch_array($result);

    $Subject = $post["subject"];
    $Text = $post["text"];

    CreateForm("$PHP_SELF?edit=$id",
            "Hdr",     "header", "Edit Post",
            "Name",    "static", isset($_username) ? $_username         : "Anonymous",
            "Subject", "input",  isset($Subject)   ? NukeHTML($Subject) : "",
            "Text",    "text",   isset($Text)      ? NukeHTML($Text)    : "",
            "PreSub",  "preview+submit", ""
    );
}


function PreviewPost($subject, $name, $text)
{   

    $poster = GetUserInfo($name);
    
    $sig = "";
    if (strlen($poster["signature"]) > 2) {
        $sig .= NukeHTML($poster["signature"], LVL_BASIC_HTML);
    }
    
    $sig .= "&nbsp;&mdash; ".FormatName($name);  # CHANGE TO CSS!
    
    $text = NukeHTML($text, LVL_BASIC_HTML);
    if (strlen($text) < 2) {
        $text = "No text.";
    }
    
    StartBox();
    MakePost($subject, $text, $sig, LVL_COMPLEX_HTML);
    EndBox();
}


function LockPost($id) {
    global $admin;
    if (!$admin) {
        FatalError("You do not have sufficient permission to lock this post.");
    }

    $result = mysql_query("UPDATE board SET locked=1 WHERE id=$id")
              or MySQL_FatalError();

    Notice("The post has been locked.  Normal users can no longer reply.");
}


function UnlockPost($id) {
    global $admin;
    if (!$admin) {
        FatalError("You do not have sufficient permission to unlock this post.");
    }

    $result = mysql_query("UPDATE board SET locked=0 WHERE id=$id")
              or MySQL_FatalError();

    Notice("The post has been unlocked.  Normal users can reply again.");
}

# -----------------------------------------------------------------------------

include_once "bin/main.php";

GenerateHeader("Forum");
VerifyLogin();

# echo '<p style="text-align: center;"><a href="board.php?post=5164">Click here for information on the ika revitalization project codenamed "ika: Redux".</a></p><br><br>';

StartBox();
echo '<table><tr>';
echo "<td><a class='button' href='$PHP_SELF?newest=1'>read newest posts</a></td>";
echo '<td><a class="button" href="#post">post a new message</a></td>';
echo '</tr></table>';
EndBox();

if (!isset($post))
    $post = 0;

if (isset($delete))
    DeletePost($delete);
elseif (isset($add))
{
    if (isset($safe_post["Submit"]))
    {
        AddPost($safe_get["add"], $safe_post["Subject"], $safe_post["Name"], $safe_post["Text"]);
    }
    else if (isset($preview))
    {
        PreviewPost($safe_post["Title"], $safe_post["Name"], $safe_post["Text"]);
        EditPost($safe_get["edit"]);
        die();
    }
    else
    {
        EditPost($safe_get["edit"]);
        die();
    }
}
elseif (isset($edit))
{
    if (isset($submit))
        UpdatePost($safe_get["edit"], $safe_post["Subject"], $safe_post["Text"]);
    else if (isset($preview))
    {
        PreviewPost($safe_post["Subject"], $safe_post["Name"], $safe_post["Text"]);
        EditPost($safe_get["edit"]);
        die();
    }
    else
    {
        EditPost($safe_get["edit"]);
        die();
    }
}
elseif (isset($lock))
    LockPost($lock);
elseif (isset($unlock))
    UnlockPost($unlock);
elseif (isset($newest)) {
    ShowNewestPosts(20);
    die();
}

if ($post != 0)
    ShowPost($post);
else
    ShowAllPosts(10);

?>