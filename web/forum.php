<?php

# displays replies to post id=$id
function ShowReplies($id) {
    $result = mysql_query("SELECT * FROM board WHERE parentid=$id AND deleted=0 ORDER BY id;");
    if (!$result)
        return;
    $post = mysql_fetch_array($result);
    if ($post)
        do {
            ShowPost($post["id"]);
        } while ($post = mysql_fetch_array($result));
}


function ShowThread($id) {
    global $admin, $_username, $PHP_SELF;
    $result = mysql_query("SELECT * FROM board WHERE id=$id")
              or FatalError("No post $id.");
    $post = mysql_fetch_array($result);
    echo "<h1>$post[subject]</h1>";
    StartBox();
    echo '<table><tr>';
    echo "<td><a class='button' href='$PHP_SELF'>return to forum</a></td>";
    if ($admin) {
        echo "<td><a class='button' href='$PHP_SELF?delete=$id'>delete</a></td>";
        if (!$post["locked"])
            echo "<td><a class='button' href='$PHP_SELF?lock=$id'>lock</a></td>";
        else
            echo "<td><a class='button' href='$PHP_SELF?unlock=$id'>unlock</a></td>";
    }
    echo '</tr></table>';
    EndBox();
    ShowPost($id);
    if ($admin) {
        if ($post["locked"] and !$admin)
            Box('This post has been locked and cannot be replied to.');
        else
            ShowPostForm($post['subject']);
    }
}


function ShowPost($id) {
    global $admin, $_username;
    $result = mysql_query("SELECT * FROM board WHERE id=$id")
              or FatalError("No post $id.");
    $post = mysql_fetch_array($result);
    $text = NukeHTML($post["text"], LVL_BASIC_HTML);
    $poster = GetUserInfo($post["name"]);
    $signature = "";
    if (strlen($poster["signature"]) > 2)
        $signature .= NukeHTML($poster["signature"], LVL_BASIC_HTML);
    echo '<div class="postbox">';
    echo '<div class="subbox">';
    echo FormatName($post["name"]), '<br /><br />';
    echo '<strong>Subject:</strong> ', NukeHTML($post["subject"]);
    if ((!$post["locked"] and $myPost) or $admin) {
        echo " [<a class='button' href='$PHP_SELF?edit=$id'>edit post</a>]";
        #echo " [<a class='button' href='$PHP_SELF?delete=$id'>delete post</a>]";
    }
    echo '<br />';
    echo "<strong>Date:</strong> $post[date], $post[time]";
    if ($admin and $post["ip"])
        echo "<br /><strong><abbr>ip</abbr> address:</strong> $post[ip]";
    $myPost = $_username == $post["name"];

    echo '</div>';
    echo '<div class="post">';
    echo $text;
    if (strlen($signature))
        echo "<div class='signature'>$signature</div>";
    echo '</div>';
    echo '</div>';
    # This shouldn't fail, unless the database is set up wrong
    $blah = mysql_fetch_row(mysql_query("SELECT COUNT(id) FROM board WHERE parentid=$id AND deleted=0"));
    # fucking retarded.  I *must* make a temporary variable. :P
    if ($blah[0])
        ShowReplies($id);
}



function ShowForum($amount) {
    global $showall, $PHP_SELF;
    $query = "SELECT * FROM board WHERE parentid=0 AND deleted=0 ORDER BY id DESC";
    if (!$showall)
        $query .= " LIMIT $amount";
    $result = mysql_query($query);
    if (!$result)
        return;
    StartBox();
    echo '<table><tr>';
    echo '<td><a class="button" href="#post">post a new message</a></td>';
    echo '</tr></table>';
    EndBox();
    echo '<table class="forum">';
    echo '<tr><th>Title</th><th>Author</th><th>Date</th></tr>';
    while ($post = mysql_fetch_array($result)) {
        echo '<tr>';
        echo "<td><a href='$PHP_SELF?post=$post[id]'>";
        echo NukeHTML($post["subject"]), '</a></td>';
        echo '<td>', FormatName($post["name"]), '</td>';
        echo "<td>$post[date]</td>";
        echo '</tr>';
    }
    echo '</table>';
    ShowPostForm();
}


function ShowPostForm($title="") {
    global $PHP_SELF, $_username, $Subject, $Text, $post;

    CreateForm("$PHP_SELF?post=$post",
        "Name",    "static", isset($_username) ? $_username : "Anonymous",
        "Subject", "input",  $title ? NukeHTML($title) : (isset($Subject) ? NukeHTML($Subject) : ""),
        "Text",    "text",   isset($Text) ? NukeHTML($Text) : "",
        "Submit",  "submit", "submit",
        "Posting instructions", "static",
        "Use the <strong>&lt;code&gt;</strong> tag around blocks of code and ".
        "<strong>&lt;pre&gt;</strong> around blocks of preformatted text that".
        " are not code. The forum also recognizes <strong>&lt;em&gt;</strong>".
        " for <em>emphasis</em>."
    );
}


function AddPost($parentid) {
    global $REMOTE_ADDR, $_username, $Subject, $Text;

    $Subject = trim($Subject) or FatalError("Please enter a subject.");
    
    $Text = trim($Text) or FatalError("Please enter fill out the message area.");

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


function DoEditPost($id) {
    global $Subject, $Text, $_username, $admin;

    $result = mysql_query("SELECT * FROM board WHERE id=$id")
              or MySQL_FatalError();
    $post = mysql_fetch_array($result);

    if (!$admin and ($post["locked"] or $post["deleted"])) {
        FatalError("This post is locked and cannot be edited.");
    }

    if (!$admin and $_username != $post["name"]) {
        FatalError("You do not have sufficient permission to edit this post.");
    }

    $result = mysql_query("UPDATE board SET subject='$Subject', text='$Text' where id=$id")
              or MySQL_FatalError();

    Notice("The post was edited successfully.");
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

    CreateForm("$PHP_SELF?post=$id",
            "Name",    "static", isset($_username) ? $_username         : "Anonymous",
            "Subject", "input",  isset($Subject)   ? NukeHTML($Subject) : "",
            "Text",    "text",   isset($Text)      ? NukeHTML($Text)    : "",
            "Submit",  "submit", "submitedit"
    );
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

if (!isset($post))
    $post = 0;

if (isset($submitedit))
    DoEditPost($post);
elseif (isset($submit))
    AddPost($post);
elseif (isset($delete))
    DeletePost($delete);
elseif (isset($edit))
    EditPost($edit);
elseif (isset($lock))
    LockPost($lock);
elseif (isset($unlock))
    UnlockPost($unlock);

if ($post != 0)
    ShowThread($post);
else
    ShowForum(50);

?>