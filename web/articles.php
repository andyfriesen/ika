<?php

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


function BrowseArticles($categoryid, $queued)
{
    global $PHP_SELF, $admin, $_username, $_GET, $articleCategory;

    $result = mysql_query("SELECT * FROM articles WHERE queued=$queued AND ".
                          "category_id=$categoryid ORDER BY view_count DESC, edit_date DESC")
              or MySQL_FatalError();

    $articlelist = array();
    for ($articles = 0; $article = mysql_fetch_array($result); $articles++)
        $articlelist[$articles] = $article;

    if (!$articles) return False;

    echo '<table class="box">';
    echo "<tr><th class='main' colspan=3>" . $articleCategory[$categoryid] . "</th></tr>";
    echo "<tr><th>Title/Author</th><th>Description</th><th>Views</th>";
    echo "</tr>";
    foreach ($articlelist as $a)
    {
        echo "<tr>";
        echo "<td width='30%' style='text-align: right; vertical-align: top'><div><a href='$PHP_SELF?view=$a[id]'>", NukeHTML($a["title"]), "</a></div>";

        $d = substr($a["edit_date"], 0, 10);
        $t = substr($a["edit_date"], 11, 8);
        echo "<div class='medium'>by ", FormatName($a["author"]), " on <i>", $a["date"], "</i></div>";
        echo "</td>";

        echo "<td style='vertical-align: top'><div class='medium'>", NukeHTML($a["description"]);

        echo " &mdash; (edited ", $a["edit_date"], ")";
        if (($admin == True or ($_username == $a["author"])) and $_username)
            echo " &mdash; ";
        if ($admin == True and $a["queued"])
            echo "<a href='$PHP_SELF?approve=$a[id]'>[Approve]</a> ";
        if (($admin == True or ($_username == $a["author"])) and $_username)
            echo "<a href='$PHP_SELF?edit=$a[id]'>[Edit]</a> ";
        if ($admin == True)
        {
            if (isset($_GET["queued"]))
                echo "<a href='$PHP_SELF?remove=$a[id]&queued=1'>[Delete]</a> ";
            else
                echo "<a href='$PHP_SELF?remove=$a[id]'>[Remove]</a> ";
        }
        echo "</td>";

        #echo "<td><div class='tiny'><strong>Edit:</strong> $a[date]<br /><strong>Made:</strong> $a[date]</div></td>";
        echo "<td class='tiny' width='1%' style='text-align: center'>" . $a["view_count"] . "</td>";

        echo "</tr>";
    }

    echo "</table>";

    return True;
}


function ShowArticle($id)
{
    global $admin, $_username;

    $result = mysql_query("SELECT author, title, text, queued FROM articles WHERE id='$id'")
              or MySQL_FatalError();

    $result = mysql_fetch_array($result);

    if ($result["queued"] == 0)
        mysql_query("UPDATE articles SET view_count=view_count+1 WHERE id=$id");

    if ($admin==False and $result["author"] != $_username and $result["queued"]==1)
        return;

    StartBox("Article Body");
    MakePost($result["title"], $result["text"], $result["author"], LVL_COMPLEX_HTML);
    EndBox();
    die();
}


function PreviewArticle($id, $title, $name, $text)
{
    StartBox("Preview Article Body");
    MakePost($title, $text, $name, LVL_COMPLEX_HTML);
    EndBox();
}


function CreateArticle()
{
    global $PHP_SELF, $_username, $safe_post, $_POST, $admin, $articleCategory;

    $a = array("title" => GetValue($safe_post, "Title"), "author" => isset($safe_post["Author"]) ? $safe_post["Author"] : (isset($_username) ? $_username : "Anonymous"), "description" => GetValue($safe_post, "Description"), "text" => GetValue($safe_post, "Text"), "category_id" => GetValue($safe_post, "Category", $articleCategory[0]));

    StartBox("Submit Article");

    CreateForm("$PHP_SELF?add=1",
        "Title",       "input",     NukeHTML($a["title"]),
        "Author",        ($admin==True) ? "input" : "hidden",     NukeHTML($a["author"]),
        "Category",    "select",    $articleCategory, $articleCategory, $a["category_id"],
        "Description", "smalltext", NukeHTML($a["description"]),
        "Text",        "text",      NukeHTML($a["text"]),
        "Pre/Sub",     "preview+submit", ""
    );

    EndBox();
}


function EditArticle($id)
{
    global $PHP_SELF, $admin, $safe_post, $_post, $articleCategory;

    if (!isset($safe_post["Preview"]))
    {
        $result = mysql_query("SELECT * FROM articles WHERE ID='$id'");
        $a = mysql_fetch_array($result);
        $a["category_id"] = $articleCategory[$a["category_id"]];
    }
    else
    {
        $a = array("id" => $id, "title" => $safe_post["Title"], "author" => $safe_post["Author"], "description" => $safe_post["Description"], "text" => $_POST["Text"], "category_id" => GetValue($safe_post, "Category", $articleCategory[0]));
    }

    StartBox("Edit Article");

    CreateForm("$PHP_SELF?update=$a[id]",
        "Title",       "input",     NukeHTML($a["title"]),
        "Author",        ($admin==True) ? "input" : "hidden",     NukeHTML($a["author"]),
        "Category",    "select",    $articleCategory, $articleCategory, $a["category_id"],
        "Description", "smalltext", NukeHTML($a["description"]),
        "Text",        "text",      NukeHTML($a["text"]),
        "Pre/Sub",     "preview+submit", ""
    );

    EndBox();
}


function AddArticle($title, $author, $description, $text, $category)
{
    global $articleCategory;

    $error = "";
    if (!trim($title))
        $error = "Your article needs a title.";
    else if (!trim($author))
        $error = "You forgot to sign your name.";
    else if (!trim($description))
        $error = "You need to fill in a description for your article.";
    else if (!trim($text))
        $error = "Articles that have no body text aren't very interesting.";

    if ($error)
    {
        Error($error);
        CreateArticle();
        die();
    }

    for ($c = 0, $i = 0; $i < sizeof($articleCategory); $i++) {
        if ($category == $articleCategory[$i]) {
            $c = $i;
            break;
        }
    }

    $date = date("Y-m-d");
    $datetime = date("Y-m-d H:i:s");

    $query = "INSERT INTO articles ".
             "(title, author, date, edit_date, description, text, category_id) ".
             "values ('$title', '$author', '$date', '$datetime', '$description', '$text', $c)";

    $result = mysql_query($query) or MySQL_FatalError();

    Success("The article has been submitted.  It will be reviewed by an " .
           "administrator within the next few days.");
}


function ApproveArticle($id)
{
    global $admin;

    if ($admin == True)
    {
        $result = mysql_query("UPDATE articles SET queued=0 WHERE id='$id'")
                or MySQL_FatalError();

        Notice("The article has been approved.");
    }
}


function RemoveArticle($id)
{
    global $admin;
    $result = mysql_query("SELECT queued FROM articles WHERE id='$id'");
    $result = mysql_fetch_array($result);

    if ($admin == True)
    {
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
}


function UpdateArticle($id, $title, $author, $description, $text, $category)
{
    global $articleCategory;

    $date = date("Y-m-d H:i:s");

    for ($c = 0, $i = 0; $i < sizeof($articleCategory); $i++) {
        if ($category == $articleCategory[$i]) {
            $c = $i;
            break;
        }
    }

    $query = "UPDATE articles SET ".
             "title='$title', author='$author', edit_date='$date', description='$description', ".
             "text='$text', category_id=$c WHERE id='$id'";
    $result = mysql_query($query) or MySQL_FatalError();

    Notice("The article has been updated.");
}


function CanModify($id, $username)
{
    global $admin;

    if ($admin==True) return True;
    else if ($username == "") return False;

    $result = mysql_query("SELECT * FROM articles WHERE id=$id");
    $result = mysql_fetch_array($result);

    return $result["author"] == $username;
}

function DisplayArticleOptions()
{
    StartBox("Options");
    echo "<table><tr>";
    echo "<td><a class='button' href='" . $_SERVER["PHP_SELF"] . "?create=1'>submit article</a></td>";
    echo "</tr></table>";
    EndBox();
}

# -----------------------------------------------------------------------------

include "bin/main.php";

GenerateHeader("Articles");

VerifyLogin();

if (isset($_GET["view"]))
    ShowArticle($safe_get["view"]);
else if (isset($_GET["add"]))
{
    if (!isset($safe_post["Preview"]))
        AddArticle($safe_post["Title"], $safe_post["Author"], $safe_post["Description"], $safe_post["Text"], $safe_post["Category"]);
    else
    {
        PreviewArticle("0", $_POST["Title"], $_POST["Author"], $_POST["Text"]);
        CreateArticle();
        #BackToArticles();
        die();
    }
}
else if ($admin==True and isset($_GET["approve"]))
    ApproveArticle($safe_get["approve"]);
else if (isset($_GET["create"]))
    CreateArticle();
else if (isset($_GET["edit"]) and CanModify($safe_get["edit"], $_username))
    EditArticle($safe_get["edit"]);
else if (isset($_GET["remove"]) and CanModify($safe_get["remove"], $_username))
    RemoveArticle($safe_get["remove"]);
else if (isset($_GET["update"]) and CanModify($safe_get["update"], $_username))
{
    if (!isset($safe_post["Preview"]))
        UpdateArticle($safe_get["update"], $safe_post["Title"], $safe_post["Author"], $safe_post["Description"], $safe_post["Text"], $safe_post["Category"]);
    else
    {
        PreviewArticle($safe_get["update"], $_POST["Title"], $_POST["Author"], $_POST["Text"]);
        EditArticle($safe_get["update"]);
        #BackToArticles();
        die();
    }
}

if (GetValue($_GET, "queued") and $admin == False)
{
    Error("You do not have the necessary access to view queued articles.");
    die();
}

DisplayArticleOptions();

$empty = False;

#$articles = GetArticles(isset($queued) ? $safe_get["queued"] : 0);
if (isset($_GET["queued"]) and $admin == True) {

    StartBox("Browse Queued Articles");
    for ($i = 0; $i < 4; $i++) {
        $empty |= BrowseArticles($i, 1);
    }
    if (!$empty)
        echo "No queued articles.";
    EndBox();

} else {

    StartBox("Browse Articles");
    for ($i = 0; $i < 4; $i++) {
        $empty |= BrowseArticles($i, 0);
    }
    if (!$empty)
        echo "No articles.";
    EndBox();
}

DisplayArticleOptions();

?>
