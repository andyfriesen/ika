<?

function GetArticles($queued)
{
    $result = mysql_query("SELECT * FROM articles WHERE queued=$queued order by title");
    if (!$result)
        return Array();

    $articles = Array();

    while (True)
    {
        $a = mysql_fetch_array($result);
        if ($a === False)
            break;
        array_push($articles, $a);
    }

    return $articles;
}


function BrowseArticles($articles)
{
    global $PHP_SELF, $admin, $_username;

    if (!sizeof($articles))
    {
        Notice('No articles.');
        return;
    }

    echo '<table class="box">';
    echo "<tr><th>Title</th><th>Author</th><th>Description</th><th>Date</th></tr>";
    foreach ($articles as $a)
    {
        echo "<tr>";
        echo "<td><a href='$PHP_SELF?view=$a[id]'>", NukeHTML($a["title"]), "</a></td>";
        echo "<td>", FormatName($a["author"]), "</td>";
        echo "<td>", NukeHTML($a["description"]), "</td>";
        echo "<td>$a[date]</td>";
        if ($admin and $a["queued"])
            echo "<td><a href='$PHP_SELF?approve=$a[id]'>Approve</a></td>";
        if ($admin or $_username == $a["author"])
            echo "<td><a href='$PHP_SELF?edit=$a[id]'>Edit</a></td>";
        if ($admin)
            echo "<td><a href='$PHP_SELF?remove=$a[id]'>Remove</a></td>";
        echo "</tr>";
    }

    echo "</table>";
}


function ShowArticle($id)
{
    $result = mysql_query("SELECT author, title, text FROM articles WHERE id=$id")
              or MySQL_FatalError();

    $result = mysql_fetch_array($result);

    StartBox();
    MakePost($result["title"], $result["text"], $result["author"], LVL_COMPLEX_HTML);
    EndBox();
    die();
}


function CreateArticle()
{
    global $PHP_SELF;

    CreateForm("$PHP_SELF?add=1",
        "Title",       "input",  "",
        "Name",        "input",  isset($_username) ? $_username : "Anonymous",
        "Description", "smalltext",   "",
        "Text",        "text",   "",
        "Submit",      "submit", ""
    );
}


function EditArticle($id)
{
    global $PHP_SELF;

    $result = mysql_query("SELECT * FROM articles WHERE ID=$id");
    $a = mysql_fetch_array($result);

    CreateForm("$PHP_SELF?update=$a[id]",
        "Title",       "input",     NukeHTML($a["title"]),
        "Name",        "static",    $a["author"],
        "Description", "smalltext", NukeHTML($a["description"]),
        "Text",        "text",      NukeHTML($a["text"]),
        "Submit",      "submit",    ""
    );
}


function AddArticle($title, $author, $description, $text)
{
    if (!trim($title))
        FatalError("Your article needs a title.");
    else if (!trim($author))
        FatalError("You forgot to sign your name.");
    else if (!trim($description))
        FatalError("You need to fill in a description for your article.");
    else if (!trim($text))
        FatalError("Articles that have no body text aren't very interesting.");

    $date = date("Y-m-d");

    $query = "INSERT INTO articles ".
             "(title, author, date, description, text) ".
             "values ('$title', '$author', '$date', '$description', '$text')";

    $result = mysql_query($query) or MySQL_FatalError();

    Notice("The article has been submitted.  It will be reviewed by an " .
           "administrator within the next few days.");
}


function ApproveArticle($id)
{
    $result = mysql_query("UPDATE articles SET queued=0 WHERE id=$id")
              or MySQL_FatalError();

    Notice("The article has been approved.");
}


function RemoveArticle($id)
{
    StartBox();
    $result = mysql_query("SELECT queued FROM articles WHERE id=$id");
    $result = mysql_fetch_array($result);
    if ($result["queued"])
    {
        $result = mysql_query("DELETE FROM articles WHERE id=$id")
                  or MySQL_FatalError();

        Notice("The article has been completely erased.");
    }
    else
    {
        $result = mysql_query("UPDATE articles SET queued=1 WHERE id=$id")
                  or MySQL_FatalError();

        Notice("The article has been moved to the queue.");
    }
}


function UpdateArticle($id, $title, $author, $description, $text)
{
    $date = date("Y-m-d");

    $query = "UPDATE articles SET ".
             "title='$title', date='$date', description='$description', ".
             "text='$text' WHERE id=$id";
    $result = mysql_query($query) or MySQL_FatalError();

    Notice("The article has been updated.");
}


function CanModify($id, $username)
{
    global $admin;
    
    if ($admin)
        return True;

    $result = mysql_query("SELECT * FROM articles WHERE id=$id");
    $result = mysql_fetch_array($result);
    return $result[$author] == $username;
}

# -----------------------------------------------------------------------------

include_once "bin/main.php";

GenerateHeader("Articles");

if (isset($view))
    ShowArticle($view);
else if (isset($add))
    AddArticle($Title, $Name, $Description, $Text);
else if ($admin and isset($approve))
    ApproveArticle($approve);
else if (isset($create))
    CreateArticle();
else if (isset($edit) and CanModify($edit, $_username))
    EditArticle($edit);
else if (isset($remove) and CanModify($remove, $_username))
    RemoveArticle($remove);
else if (isset($update) and CanModify($update, $_username))
    UpdateArticle($update, $Title, $Name, $Description, $Text);

$articles = GetArticles(isset($queued) ? $queued : 0);
BrowseArticles($articles);

Box("<a href='$PHP_SELF?create=1'>Submit an article.</a>");

?>