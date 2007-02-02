<?

function GetArticles($queued)
{
    $result = mysql_query("SELECT * FROM articles WHERE queued='$queued' order by title");
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

    StartBox();
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
    EndBox();
}


function ShowArticle($id)
{
    $result = mysql_query("SELECT author, title, text FROM articles WHERE id='$id'")
              or MySQL_FatalError();

    $result = mysql_fetch_array($result);

    StartBox();
    MakePost($result["title"], $result["text"], $result["author"], LVL_COMPLEX_HTML);
    EndBox();
    die();
}


function PreviewArticle($id, $title, $name, $text)
{   
    StartBox();
    MakePost($title, $text, $name, LVL_COMPLEX_HTML);
    EndBox();
}


function CreateArticle()
{
    global $PHP_SELF, $_username;

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
    global $PHP_SELF, $admin, $safe_post, $_post;

    if (!isset($safe_post["Preview"]))
    {
        $result = mysql_query("SELECT * FROM articles WHERE ID='$id'");
        $a = mysql_fetch_array($result);
    }
    else
    {
        $a = array("id" => $id, "title" => $_POST["Title"], "author" => $_POST["Name"], "description" => $_POST["Description"], "text" => $_POST["Text"]);
    }

    if ($admin == True)
    
        CreateForm("$PHP_SELF?update=$a[id]",
            "Hdr",         "header",    "Edit Article",
            "Title",       "input",     NukeHTML($a["title"]),
            "Name",        "input",     NukeHTML($a["author"]),
            "Description", "smalltext", NukeHTML($a["description"]),
            "Text",        "text",      NukeHTML($a["text"]),
            "Pre/Sub",     "preview+submit", ""
        );
    
    else
        
        CreateForm("$PHP_SELF?update=$a[id]",
            "Hdr",         "header",    "Edit Article",
            "Title",       "input",     NukeHTML($a["title"]),
            "Name",        "static",    $a["author"],
            "Description", "smalltext", NukeHTML($a["description"]),
            "Text",        "text",      NukeHTML($a["text"]),
            "Pre/Sub",     "preview+submit", ""
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
    $result = mysql_query("UPDATE articles SET queued=0 WHERE id='$id'")
              or MySQL_FatalError();

    Notice("The article has been approved.");
}


function RemoveArticle($id)
{
    StartBox();
    $result = mysql_query("SELECT queued FROM articles WHERE id='$id'");
    $result = mysql_fetch_array($result);
    if ($result["queued"])
    {
        $result = mysql_query("DELETE FROM articles WHERE id='$id'")
                  or MySQL_FatalError();

        Notice("The article has been completely erased.");
    }
    else
    {
        $result = mysql_query("UPDATE articles SET queued='1' WHERE id='$id'")
                  or MySQL_FatalError();

        Notice("The article has been moved to the queue.");
    }
}


function UpdateArticle($id, $title, $author, $description, $text)
{
    $date = date("Y-m-d");

    $query = "UPDATE articles SET ".
             "title='$title', author='$author', date='$date', description='$description', ".
             "text='$text' WHERE id='$id'";
    $result = mysql_query($query) or MySQL_FatalError();

    Notice("The article has been updated.");
}


function CanModify($id, $username)
{
    global $admin;
    
    if ($admin==True)
    {
        return True;
    }
    
    $result = mysql_query("SELECT * FROM articles WHERE id=$id");
    $result = mysql_fetch_array($result);
    
    return $result["author"] == $username;
}

# -----------------------------------------------------------------------------

include "bin/main.php";

GenerateHeader("Articles");

VerifyLogin();

if (isset($view))
    ShowArticle($safe_get["view"]);
else if (isset($add))
    AddArticle($safe_post["Title"], $safe_post["Name"], $safe_post["Description"], $safe_post["Text"]);
else if ($admin==True and isset($approve))
    ApproveArticle($safe_get["approve"]);
else if (isset($create))
    CreateArticle();
else if (isset($edit) and CanModify($safe_get["edit"], $_username))
    EditArticle($safe_get["edit"]);
else if (isset($remove) and CanModify($safe_get["remove"], $_username))
    RemoveArticle($safe_get["remove"]);
else if (isset($update) and CanModify($safe_get["update"], $_username))
{
    if (!isset($safe_post["Preview"]))
        UpdateArticle($safe_get["update"], $safe_post["Title"], $safe_post["Name"], $safe_post["Description"], $safe_post["Text"]);
    else
    {
        PreviewArticle($safe_get["update"], $_POST["Title"], $_POST["Name"], $_POST["Text"]);
        EditArticle($safe_get["update"]);
        #BackToArticles();
        die();
    }
}

$articles = GetArticles(isset($queued) ? $safe_get["queued"] : 0);
BrowseArticles($articles);

Box("<a href='$PHP_SELF?create=1'>Submit an article.</a>");

?>