<?php

# displays replies to post id=$id
function ShowReplies($id, $start_post=0, $end_post=0) {
    $result = "SELECT * FROM board WHERE parentid=$id AND deleted=0 ORDER BY date ASC, time ASC LIMIT $start_post,10";
    $result = mysql_query($result);
    
    if (!$result)
        return;
    $post = mysql_fetch_array($result);
    $cnt = 0;
    if ($post)
        do {
            $cnt++;
            ShowPost($post["id"], $id, $start_post + $cnt);
        } while ($post = mysql_fetch_array($result));
}


function ShowThread($id) {
    global $admin, $_username, $PHP_SELF;
    mysql_query("UPDATE board SET view_count=view_count+1 WHERE id=$id");
    
    $result = mysql_query("SELECT * FROM board WHERE id=$id")
              or FatalError("No post $id.");
    $post = mysql_fetch_array($result);
    
    $query = "SELECT COUNT(id) AS c FROM board WHERE parentid=$id AND deleted=0";
    $num_posts = mysql_fetch_array(mysql_query($query));
    $num_posts = $num_posts["c"];
    
    $num_pages = (int) ((($num_posts - 1) / 10) + 1);
    if (array_key_exists("pg", $_GET) and ctype_alnum($_GET["pg"]))
        $pg = min($num_pages, (int) $_GET["pg"]);
    else
        $pg = 1;
    $start_post = (($pg-1) * 10);
    $end_post = min($num_posts, ($pg * 10)) - 1;

    StartBox("Options");
    echo '<table><tr>';
    echo "<td><a class='button' href='$PHP_SELF'>return to forum</a></td>";
    if ($admin) {
        if (!$post["locked"])
            echo "<td><a class='button' href='$PHP_SELF?lock=$id'>lock</a></td>";
        else
            echo "<td><a class='button' href='$PHP_SELF?unlock=$id'>unlock</a></td>";
        echo "<td><a class='button' href='/board.php?post=$id'>branch view</a></td>";
    }
    echo '</tr></table>';
    
    EndBox();
    
    StartBox("Thread: " . $post['subject']);
    
    echo "<p class='tiny' style='float: left'>Page <strong>" . $pg . "</strong> of <strong>" . $num_pages . "</strong>. Posts <strong>" . ($start_post+1) . "</strong>-<strong>" . ($end_post+1) . "</strong> of <strong>" . $num_posts . "</strong>.</p>";
    
    echo "<p class='tiny' style='float: right; text-align: right'>";
    PrintPagination($_SERVER["PHP_SELF"] . "?post=" . $id . "&", $num_pages, $pg);
    echo "</p><br />";
    
    echo "<table class='box' style='overflow: hidden'>";
    ShowReplies($id, $start_post, $end_post);
    echo "</table>";
    
    echo "<p class='tiny' style='float: left'>Page <strong>" . $pg . "</strong> of <strong>" . $num_pages . "</strong>. Posts <strong>" . ($start_post+1) . "</strong>-<strong>" . ($end_post+1) . "</strong> of <strong>" . $num_posts . "</strong>.</p>";
    
    echo "<p class='tiny' style='float: right; text-align: right'>";
    PrintPagination($_SERVER["PHP_SELF"] . "?post=" . $id . "&", $num_pages, $pg);
    echo "</p><br />";
    
    EndBox();
    
    echo "<div id='last'></div>";
    
    if ($post["locked"] and !$admin) {
        FatalError("This post has been locked and cannot be replied to.");
    } else if (strncmp($post["subject"], "Re: ", 4)) {
        ShowPostForm("Write Reply", "Re: $post[subject]");
    } else {
        ShowPostForm("Write Reply", $post["subject"]);
    }
}


function ShowPost($id, $baseid=0, $cnt=1) {
    global $admin, $_username;
        
    if ($baseid == 0)
        $baseid = $id;
        
    $result = mysql_query("SELECT * FROM board WHERE id=$id")
              or FatalError("No post $id.");
    $post = mysql_fetch_array($result);
    $text = $post["text"];
    if (!$text)
        $text = "No text.";
    $poster = GetUserInfo($post["name"]);
    $signature = "";
    if (strlen($poster["signature"]) > 2)
        $signature .= NukeHTML($poster["signature"], LVL_BASIC_HTML);

    $myPost = $_username == $post["name"];
    
    echo "<tr>";
    echo "<th class='main' id='r$id' width='5%' rowspan='2'><a href='" . $_SERVER["PHP_SELF"] . "?post=$baseid#r$id'>$cnt</a></th>";
    echo "<th class='main' colspan='4'>" . NukeHTML($post["subject"]) . "</th></tr>";
    echo "<tr>";
    echo "<th width='20%' id='name$id' name='$post[name]'>" . FormatName($post["name"]) . "</th>";
    echo "<td width='20%'><strong>Date:</strong> $post[date]</td>";
    echo "<td width='20%'><strong>Time:</strong> $post[time]</td>";

    echo "<td><strong>Options:</strong> ";
    
    if ((!$post["locked"] and $myPost) or $admin == True)
    {
        echo "<a class='button' href='" . $_SERVER["PHP_SELF"]. "?edit=$id'>[edit]</a> ";
        echo "<a class='button' href='" . $_SERVER["PHP_SELF"] . "?delete=$id'>[delete]</a> ";
    }
    echo "<a class='button' href='javascript:quoteText($id);'>[quote]</a>";
    
    echo "</td>";
    echo "</tr>";
    echo "<tr><td class='box' colspan='5' style='text-align: left; margin-left: 2em; margin-right: 2em; table-layout: fixed'> ";
    echo "<div class='text' id='text$id' style='margin-bottom: 1em;'>" . NukeHTML($text, LVL_COMPLEX_HTML) . "</div>";
    
    if ($post["edit_date"]) {
        $date = substr($post["edit_date"], 0, 10);
        $time = substr($post["edit_date"], 11, 8);
        echo "<div class='text tiny' id='text$id' style='margin-bottom: 1em;'><i><b>Edited:</b> " . $date . " at " . $time . "</div>";
    }
        
    if (strlen($signature))
        echo "<div class='signature'>$signature</div>";
    
    if ($admin == True and $post["ip"])
        echo "<div class='text'><br /><strong>IP:</strong> " . $post["ip"] . "</div>";

    echo "</td></tr>";
    
    /*
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
    echo '</div>';*/
}



function ShowForum($amount) {
    global $showall, $PHP_SELF, $admin;
    
    $query = "SELECT COUNT(id) AS c FROM board WHERE parentid=id AND deleted=0";
    $num_threads = mysql_fetch_array(mysql_query($query));
    $num_threads = $num_threads["c"];
    
    $num_pages = (int) ((($num_threads - 1) / $amount) + 1);
    if (array_key_exists("pg", $_GET) and ctype_alnum($_GET["pg"]))
        $pg = min($num_pages, (int) $_GET["pg"]);
    else
        $pg = 1;
    $start_thread = (($pg-1) * $amount);
    $end_thread = min($num_threads-1, ($pg * $amount) - 1);
    
    #$query = "SELECT * FROM board WHERE parentid=0 AND deleted=0 ORDER BY date DESC, time DESC";
    #$query = "SELECT id, subject, CONCAT(board.date, " ", board.time) AS dt FROM board GROUP BY parentid ORDER BY dt DESC";
    $query = "SELECT b2.*, MAX(CONCAT(b1.date, ' ', b1.time)) AS m, COUNT(b1.id)-1 AS post_count FROM board AS b1 JOIN board AS b2 ON b2.id = b1.parentid WHERE b1.deleted=0 GROUP BY b1.parentid ORDER BY m DESC LIMIT $start_thread,$amount";
    #$query = "SELECT *, parentid AS 'pid' FROM board JOIN (SELECT MAX(CONCAT(board.date, ' ', board.time)) AS dt FROM board WHERE parentid='pid') AS sub WHERE deleted=0 AND parentid=id GROUP BY parentid ASC ORDER BY sub.dt DESC LIMIT $start_thread,$amount";
    #$query = "SELECT *, id AS bid FROM (SELECT * FROM board WHERE parentid=0 AND deleted=0 ORDER BY id DESC LIMIT $start_thread,20) AS b ORDER BY (SELECT id FROM board WHERE bid IN (parentid, id) AND deleted=0 ORDER BY id DESC LIMIT 1) DESC";
    #$query = "SELECT *, id AS bid FROM board WHERE parentid=0 AND deleted=0 ORDER BY (SELECT id FROM board WHERE bid IN (parentid, id) AND deleted=0 ORDER BY id DESC LIMIT 1) DESC LIMIT $start_thread,$end_thread";
    
    #SELECT *, id AS bid FROM board ORDER BY (SELECT date FROM board WHERE parentid=bid OR id=bid ORDER BY date DESC, time DESC LIMIT 1) DESC, (SELECT time FROM board WHERE parentid=bid OR id=bid ORDER BY date DESC, time DESC LIMIT 1) DESC;
    
    $result = mysql_query($query);
    
    echo mysql_error();
    if (!$result)
        return;
    StartBox("Options");
    echo '<table><tr>';
    echo '<td><a class="button" href="#post">post a new message</a></td>';
    echo '</tr></table>';
    EndBox();
    
    StartBox("Posts (Thread View)");
    
    echo "<p class='tiny' style='float: left'>Page <strong>" . $pg . "</strong> of <strong>" . $num_pages . "</strong>. Threads <strong>" . ($start_thread+1) . "</strong>-<strong>" . ($end_thread+1) . "</strong> of <strong>" . $num_threads . "</strong>.</p>";
    
    echo "<p class='tiny' style='float: right; text-align: right'>";
    PrintPagination($_SERVER["PHP_SELF"] . "?", $num_pages, $pg);
    echo "</p><br /><br />";
    
    echo '<table class="forum">';
    echo '<tr><th>Subject</th><th>Replies</th><th>Author</th><th>Views</th><th>Latest</th></tr>';
    
    while ($post = mysql_fetch_array($result)) {
        
        if (strlen($post["subject"]) > 30)
            $post["subject"] = substr($post["subject"], 0, 30) . "...";
            
        $query = "SELECT * FROM board WHERE parentid=$post[id] AND deleted=0 ORDER BY date DESC, time DESC LIMIT 1";
        $sub = mysql_query($query);
        $c = $post["post_count"];
        $sub_pages = (int) (((($c+1) - 1) / 10) + 1);
        
        echo '<tr>';
        echo "<td width='40%'><a href='$PHP_SELF?post=$post[id]' style='white-space: nowrap'>", NukeHTML($post["subject"]), '</a>';
        if ($sub_pages > 1)
            echo '<br /><div class="tiny">', PrintPagination($_SERVER["PHP_SELF"] . "?post=$post[id]&", $sub_pages), '</td>';
        echo "<td><p class='tiny' style='text-align: center'>" . $c . "</p></td>";
        echo "<td width='15%' style='text-align: center'>", FormatName($post["name"]), '</td>';
        echo "<td><p class='tiny' style='text-align: center'>" . $post["view_count"] . "</p></td>";
        
        $c++;
        
        $base = $post["id"];
        if (mysql_num_rows($sub) > 0)
            $post = mysql_fetch_array($sub);

        if (strlen($post["subject"]) > 30)
            $post["subject"] = substr($post["subject"], 0, 30) . "...";
            
        echo "<td width='25%'><p class='tiny' style='white-space: nowrap'><a href='$PHP_SELF?post=$base&pg=" . ((int) ($c / 10) + 1) . "#r" . $post["id"] . "'>" . NukeHTML($post["subject"]) . "</a><br />";
        echo "by ", FormatName($post["name"]);
        echo " <i>on</i> ", $post["date"], " <i>at</i> ", $post["time"];
        echo '</td>';
        
        echo '</tr>';
    }
    echo '</table>';
    
    echo "<p class='tiny' style='float: left'>Page <strong>" . $pg . "</strong> of <strong>" . $num_pages . "</strong>. Threads <strong>" . ($start_thread+1) . "</strong>-<strong>" . ($end_thread+1) . "</strong> of <strong>" . $num_threads . "</strong>.</p>";
    
    echo "<p class='tiny' style='float: right; text-align: right'>";
    PrintPagination($_SERVER["PHP_SELF"] . "?", $num_pages, $pg);
    echo "</p><br /><br />";
    
    EndBox();
    ShowPostForm("Write New Post");
}


function ShowPostForm($header="", $title="") {
    global $_username, $Subject, $Text, $post;
    
    StartBox($header, "", "id='post'");
    
    CreateForm($_SERVER["PHP_SELF"] . "?add=$post",
        "Name",    "hidden", isset($_username) ? $_username : "Anonymous",
        "Subject", "input",  $title ? NukeHTML($title) : (isset($Subject) ? NukeHTML($Subject) : ""),
        "Text",    "text",   isset($Text) ? NukeHTML($Text) : "",
        "PreSub",  "preview+submit", "",
        "Posting instructions", "static",
        "Use the <strong>&lt;code&gt;</strong> tag around blocks of code and ".
        "<strong>&lt;pre&gt;</strong> around blocks of preformatted text that".
        " are not code. The forum also recognizes <strong>&lt;em&gt;</strong>".
        " for <em>emphasis</em>."
    );
    
    EndBox();
}

function AddPost($parentid, $subject, $name, $text) {
    global $REMOTE_ADDR, $_username;

    $Subject = trim($subject);
    
    if ($Subject)
    {
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
        mysql_query("UPDATE board SET parentid=id WHERE parentid=0");
        Notice("Post added successfully.");
    }
    else
    {
        Error("Please enter a subject.");
        EditPost("add", $parentid);
        die();
    }
}


function UpdatePost($id, $subject, $text) {
    
    global $admin, $_username;

    if ($subject)
    {
        $result = mysql_query("SELECT * FROM board WHERE id=$id")
                  or MySQL_FatalError();
        $post = mysql_fetch_array($result);

        if (!$admin and ($post["locked"] or $post["deleted"])) {
            FatalError("This post is locked and cannot be edited.");
        }

        if (!$admin and ($_username != $post["name"])) {
            FatalError("You do not have sufficient permission to edit this post.");
        }
        
        $dt = date("Y-m-d H:i:s");
        $result = mysql_query("UPDATE board SET subject='$subject', text='$text', edit_date='$dt' where id=$id")
                  or MySQL_FatalError();

        Notice("The post was edited successfully");
    }
    else
    {
        Error("Please enter a subject.");
        EditPost("edit", $id);
        die();
    }
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
}


function EditPost($context, $id) {
    global $_username, $Subject, $Text, $post, $safe_post;

    if (!isset($safe_post["Preview"]) and !isset($safe_post["Submit"]))
    {
        $result = mysql_query("SELECT * FROM board WHERE id=$id")
                  or MySQL_FatalError();
        $post = mysql_fetch_array($result);
    }
    else
    {
        $post = array("id" => $id, "title" => $_POST["Subject"], "name" => $_POST["Name"], "subject" => $_POST["Subject"], "text" => $_POST["Text"]);
    }
    $Subject = $post["subject"];
    $Text = $post["text"];

    StartBox("Edit Post");
    CreateForm($_SERVER["PHP_SELF"] . "?$context=$id",
            "Name",    "hidden", $post["name"],
            "Subject", "input",  isset($Subject)   ? NukeHTML($Subject) : "",
            "Text",    "text",   isset($Text)      ? NukeHTML($Text)    : "",
            "PreSub",  "preview+submit", ""
    );
    EndBox();
}


function PreviewPost($subject, $name, $text)
{   
    global $safe_post, $admin;
    
    $poster = GetUserInfo($name);
    
    $sig = "";
    if (strlen($poster["signature"]) > 2) {
        $sig .= NukeHTML($poster["signature"], LVL_BASIC_HTML);
    }
    
    if (strlen($text) < 2) {
        $text = "No text.";
    }
    
    StartBox("Preview Post");
    echo "<table class='box' style='overflow: hidden'>";
    echo "<tr>";
    echo "<th id='post' width='5%' rowspan='2'>#</th>";
    echo "<th class='main' width='20%' rowspan='2'>" . FormatName($name) . "</th>";
    echo "<th colspan='3'>" . NukeHTML($subject) . "</th></tr>";
    echo "<tr>";
    echo "<td width='20%'><strong>Date:</strong> " . date("Y-m-d") . "</td>";
    echo "<td width='20%'><strong>Time:</strong> " . date("H:i:s") . "</td>";
    echo "<td><strong>Options:</strong></td>";
    echo "</tr>";
    echo "<tr><td class='box' colspan='5' style='text-align: left; margin-left: 2em; margin-right: 2em; table-layout: fixed'> ";
    
    echo "<div class='text' style='margin-bottom: 1em;'>" . NukeHTML($text, LVL_COMPLEX_HTML) . "</div>";
    if (strlen($sig))
        echo "<div class='signature'>$sig</div>";

    echo "</td></tr>";
    echo "</table>";
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

if (!isset($_GET["post"]))
    $post = 0;
else
    $post = $_GET["post"];

if (GetValue($_GET, "delete"))
    DeletePost($_GET["delete"]);
elseif (isset($_GET["add"]))
{
    if (isset($safe_post["Submit"]))
    {
        AddPost($safe_get["add"], $safe_post["Subject"], $safe_post["Name"], $safe_post["Text"]);
    }
    else if (isset($safe_post["Preview"]))
    {
        PreviewPost($safe_post["Subject"], $safe_post["Name"], $_POST["Text"]);
        EditPost("add", $safe_get["add"]);
        die();
    }
    else
    {
        EditPost($safe_get["edit"]);
        die();
    }
}
else if (GetValue($_GET, "edit"))
{
    if (isset($safe_post["Submit"]))
        UpdatePost($safe_get["edit"], $safe_post["Subject"], $safe_post["Text"]);
    else if (isset($safe_post["Preview"]))
    {
        PreviewPost($safe_post["Subject"], $safe_post["Name"], $_POST["Text"]);
        EditPost("edit", $safe_get["edit"]);
        die();
    }
    else
    {
        EditPost("edit", $safe_get["edit"]);
        die();
    }
}
elseif (GetValue($_GET, "lock"))
    LockPost($_GET["lock"]);
elseif (GetValue($_GET, "unlock"))
    UnlockPost($_GET["unlock"]);

if ($post != 0)
    ShowThread($post);
else
    ShowForum(20);

?>