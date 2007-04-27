<?php

function LogDownload($file) {
    echo "YAY";
    mysql_query("UPDATE files SET download_count=download_count+1 WHERE id=" . $file . " AND queued=0");
}

# Shows a nice columnized view of the file.
function ShowFile($file, $queued) {
    global $filedir, $admin, $_username, $safe_get;

    $td = "";
    $action = "view";

    if (stristr($file["filename"], ".py")) {
        $url = "source.php?file=$file[id]";
        $td = "background-color: #ccd;";
    } else if (strpos($file["filename"], "://") == False) {
        $url = $filedir . $file["filename"];
        if (!file_exists($url))
            $td = "background-color: #ecc;";
    } else {
        $url = $file["filename"];
        if (!file_exists($url))
            $td = "background-color: #dcc;";
    }

    $sub = substr($url, 0, 7);
    if ($sub == "http://")
    {
        $td = "background-color: #cdc;";
    }
    /*
    echo '<tr>';
    echo $td, "<a href='$url'>", NukeHTML($file["name"]), "</a></td>";
    echo $td, FormatName($file["author"]), "</td>";
    echo $td, "$file[date]</td>";

    if (strlen($file["description"]) > 1)
        echo $td, NukeHTML($file["description"]), "</td>";
    else
        echo $td, "No description.</td>";

    if ($admin and $file["queued"])
        echo "<td><a href='" . $_SERVER["PHP_SELF"] . "?approve=$file[id]'>Approve</a></td>";
    if ($admin or $_username == $file["author"])
        echo "<td><a href='" . $_SERVER["PHP_SELF"] . "?edit=$file[id]'>Edit</a></td>";
    if ($admin)
        echo "<td><a href='" . $_SERVER["PHP_SELF"] . "?remove=$file[id]'>Remove</a></td>";

    echo "</tr>";*/

        echo "<tr>";
        #echo "<td width='30%' style='text-align: right; vertical-align: top; " . $td . "'><div><a href='" . $url . "'>", NukeHTML($file["name"]), "</a></div>";
        echo "<td width='30%' style='text-align: right; vertical-align: top; " . $td . "'><div><a href='" . $_SERVER["PHP_SELF"] . "?" . $action . "=" . $file["id"] . "'>", NukeHTML($file["name"]), "</a></div>";

        $d = substr($file["edit_date"], 0, 10);
        $t = substr($file["edit_date"], 11, 8);
        echo "<div class='medium'>by ", FormatName($file["author"]), " on <i>", $file["date"], "</i></div>";
        echo "</td>";

        echo "<td style='vertical-align: top; " . $td . "'><div class='medium'>", NukeHTML($file["description"]);

        echo " &mdash; (edited ", $file["edit_date"], ")";
        if (($admin == True or ($_username == $file["author"])) and $_username)
            echo " &mdash; ";
        if ($admin == True and $file["queued"])
            echo "<a href='" . $_SERVER["PHP_SELF"] . "?approve=" . $file["id"] . "'>[Approve]</a> ";
        if (($admin == True or ($_username == $file["author"])) and $_username)
            echo "<a href='" . $_SERVER["PHP_SELF"] . "?edit=" . $file["id"] . "'>[Edit]</a> ";
        if ($admin == True)
        {
            if (isset($_GET["queued"]))
                echo "<a href='" . $_SERVER["PHP_SELF"] . "?remove=" . $file["id"] . "&queued=1'>[Delete]</a> ";
            else
                echo "<a href='" . $_SERVER["PHP_SELF"] . "?remove=" . $file["id"] . "'>[Remove]</a> ";
        }
        echo "</td>";

        #echo "<td><div class='tiny'><strong>Edit:</strong> $a[date]<br /><strong>Made:</strong> $a[date]</div></td>";
        echo "<td class='tiny' width='1%' style='text-align: center; " . $td . "'";

        if ($sub == "http://")
            echo " colspan=2";

        echo ">" . $file["view_count"] . "</td>";

        if ($sub != "http://")
            echo "<td class='tiny' width='1%' style='text-align: center; " . $td . "'>" . $file["download_count"] . "</td>";

        echo "</tr>";

}


function ShowFileDetails($id) {
    global $admin, $_username;
    mysql_query("UPDATE files SET view_count=view_count+1 WHERE id=$id");
    $cursor = mysql_query("select * from files where id=$id") or MySQL_FatalError();
    $file = mysql_fetch_array($cursor) or MySQL_FatalError();
    $filename = $file["filename"];
    $title = NukeHTML($file["name"]);
    $description = $file["description"];
    $author = $file["author"];
    $date = $file["date"];
    $editdate = $file["edit_date"];
    $views = $file["view_count"];
    $dls = $file["download_count"];

    StartBox("File Details");
    echo "<h2>", $title;
    if ($date)
        echo " <span class='date'>&mdash; $date</span>";
    echo "</h2>";

    echo "<p>Created by " . FormatName($author) . ".</p><br />";
    echo "<p class='medium'>" . NukeHTML($description, LVL_BASIC_HTML) . "</p><br />";

    echo "<p class='medium'><b>Last edited:</b> " . $editdate . "</p><br />";

    echo "<p class='tiny'><strong>Downloads:</strong> $dls<br /><strong>Views:</strong> $views</p><br />";

    echo "<p class='tiny'>";
    if ($admin == True and $file["queued"])
        echo "<a href='" . $_SERVER["PHP_SELF"] . "?approve=" . $file["id"] . "'>[Approve]</a> ";
    if (($admin == True or ($_username == $file["author"])) and $_username)
        echo "<a href='" . $_SERVER["PHP_SELF"] . "?edit=" . $file["id"] . "'>[Edit]</a> ";
    if ($admin == True)
    {
        if ($file["queued"])
            echo "<a href='" . $_SERVER["PHP_SELF"] . "?remove=" . $file["id"] . "&queued=1'>[Delete]</a> ";
        else
            echo "<a href='" . $_SERVER["PHP_SELF"] . "?remove=" . $file["id"] . "'>[Remove]</a> ";
    }
    echo "</p>";

    echo "<p style='font-size: 1.5em; text-align: center'>Download: <a href='" . $_SERVER["PHP_SELF"] . "?download=" . $id . "'>" . $filename . "</a> (" . FormatSize(filesize("files/" . $filename)) . ")</p>";
    EndBox();
}


function BrowseFiles($categoryid, $queued) {
    global $fileCategory;

    $result = mysql_query("SELECT * FROM files WHERE queued=$queued AND ".
                          "category=$categoryid ORDER BY edit_date DESC")
              or MySQL_FatalError();

    $filelist = array();
    for ($files = 0; $file = mysql_fetch_array($result); $files++)
        $filelist[$files] = $file;

    # no files at all?
    if (!$files) return False;

    echo '<table class="box">';
    echo "<tr><th class='main' colspan=4>" . $fileCategory[$categoryid] . "</th></tr>";
    echo "<tr><th>Title/Author</th><th>Description</th><th>Views</th><th>DLs</th></tr>";

    foreach ($filelist as $file) {
        ShowFile($file, $queued);
    }

    echo "</table>";

    return True;
}


function CreateFile()
{
    global $fileCategory, $_username, $admin;

    StartBox("Add New File");

    CreateForm($_SERVER["PHP_SELF"] . "?create",
    "Title",       "input",     "",
    "Author",      ($admin==True) ? "input" : "hidden",     isset($_username) ? $_username : "Anonymous",
    "Filename",    "filename",  isset($Filename)  ? $Filename  : "",
    "Category",    "select",    $fileCategory, $fileCategory, $fileCategory[0],
    "Description", "smalltext", "",
    "Submit",      "submit",    "");

    EndBox();
}


function EditFile($id)
{
    global $PHP_SELF, $admin, $safe_post, $_post, $fileCategory;

    if (!isset($safe_post["Submit"]))
    {
        $result = mysql_query("SELECT * FROM files WHERE id='$id'");
        $a = mysql_fetch_array($result);
    }
    else
    {
        $a = array("id" => $id, "name" => $safe_post["Title"], "author" => $safe_post["Name"], "description" => $safe_post["Description"], "text" => $safe_post["Text"], "category" => GetValue($safe_post, "Category", $fileCategory[0]));
    }

    StartBox("Edit Article");

    CreateForm($_SERVER["PHP_SELF"] . "?edit=$a[id]",
        "Title",       "input",     NukeHTML($a["name"]),
        "Author",      ($admin==True) ? "input" : "hidden",     NukeHTML($a["author"]),
        "Category",    "select",    $fileCategory, $fileCategory, $fileCategory[$a["category"]],
        "Description", "smalltext", NukeHTML($a["description"]),
        "Submit",      "submit", ""
    );

    EndBox();
}


function AddFile($title, $author, $description, $category) {
    global $Title, $Author, $Category, $Filename, $Description, $Filename_name,
           $fileCategory, $filedir, $URL;

    if (!is_uploaded_file($Filename)) {
        FatalError("$Filename did not upload properly.");
    }

    if (file_exists($filedir.$Filename_name)) {
        FatalError("$Filename: A file with that name already exists.  Rename it and try again.");
    }

    $date = date("Y-m-d");

    for ($c = 0, $i = 0; $i < sizeof($fileCategory); $i++) {
        if ($category == $fileCategory[$i]) {
            $c = $i;
            break;
        }
    }

    move_uploaded_file($Filename, $filedir.$Filename_name)
        or FatalError("Upload failed.");

    $query = "INSERT INTO files "
           . "(filename, name, author, description, category, size, date) "
           . "values ('$filename_name', '$title', '$author', '$description', $c, 0, '$date')";
    $result = mysql_query($query) or MySQL_FatalError();

    StartBox();
    echo '<p>The file has been added to the queue.  It will be reviewed by an admin within a few days.</p>';
    echo "<p><a href='" . $_SERVER["PHP_SELF"] . "'>Return to the files page.</a></p>";
    EndBox();
}


function UpdateFile($id)
{
    global $fileCategory, $safe_post;

    $date = date("Y-m-d H:i:s");

    for ($c = 0, $i = 0; $i < sizeof($fileCategory); $i++) {
        if ($safe_post["Category"] == $fileCategory[$i]) {
            $c = $i;
            break;
        }
    }

    $name = $safe_post["Title"];
    $author = $safe_post["Author"];
    $desc = $safe_post["Description"];

    $query = "UPDATE files SET ".
             "name='$name', author='$author', edit_date='$date', description='$desc', ".
             "category=$c WHERE id='$id'";
    $result = mysql_query($query) or MySQL_FatalError();

    Notice("The file has been updated.");
}


function ApproveFile($id) {

    mysql_query("UPDATE files SET queued=0 WHERE id=$approve");
    Notice("The file has been approved.");
}


function RemoveFile($id) {
    global $filedir;

    $result = mysql_query("SELECT queued, filename FROM files WHERE id=$id");
    $row = mysql_fetch_array($result);

    if ($row["queued"]) {
        unlink($filedir.$row["filename"]);
        mysql_query("DELETE FROM files WHERE id=$id");
        Box('The file has been deleted and removed from the database.');
    } else {
        mysql_query("UPDATE files SET queued=1 WHERE id=$id");
        Box('The file has been returned to the file queue.  ' .
            'Remove it from the queue to completely erase it.');
    }
}

function DisplayFileOptions()
{

    StartBox("Options");
    echo "<table><tr><td><a class='button' href='" . $_SERVER["PHP_SELF"] . "?create=1'>submit file</a></td></tr></table>";
    EndBox();
}

# -----------------------------------------------------------------------------

include_once "bin/main.php";

GenerateHeader("Files");

VerifyLogin();

if (isset($_GET["queued"]) and $admin == False)
{
    Error("You do not have the necessary access to view queued files.");
    die();
}

if (isset($_GET["download"]))
{
    $file = mysql_fetch_array(mysql_query("SELECT filename, queued FROM files WHERE id='" . $safe_get["download"] . "'"));

    if ($file["queued"] == 0)
    {
        mysql_query("UPDATE files SET download_count=download_count+1 WHERE id='" . $safe_get["download"] . "'");

        if (strpos($file["filename"], "://") == False) {
            $url = $filedir . $file["filename"];
        } else {
            $url = $file["filename"];
        }

        echo "<script language='javascript'>window.location.href = '" . $url . "';</script>";
    }
}

if (isset($_GET["view"])) {

    $file = mysql_fetch_array(mysql_query("SELECT filename, queued FROM files WHERE id='" . $safe_get["view"] . "'"));

    if ($file["queued"] == 0)
    {
        if (stristr($file["filename"], ".py")) {
            mysql_query("UPDATE files SET view_count=view_count+1 WHERE id='" . $safe_get["view"] . "'");
            $url = "source.php?file=" . $safe_get["view"];
            echo "<script language='javascript'>window.location.href = '" . $url . "';</script>";
        } else if (strpos($file["filename"], "://") == True) {
            mysql_query("UPDATE files SET view_count=view_count+1 WHERE id='" . $safe_get["view"] . "'");
            $url = $file["filename"];
            echo "<script language='javascript'>window.location.href = '" . $url . "';</script>";
        } else {
            ShowFileDetails($safe_get["view"]);
            DisplayFileOptions();
            die();
        }
    }

} elseif (isset($_GET["create"])) {
    if (!isset($_POST["Submit"]))
        CreateFile();
    else
        AddFile($safe_post["Title"], $safe_post["Name"], $safe_post["Description"], $safe_post["Category"]);
} else if (isset($_GET["edit"])) {
    if (!isset($_POST["Submit"]))
        EditFile($safe_get["edit"]);
    else
        UpdateFile($safe_get["edit"]);
} else if (isset($approve) and isset($admin))
    ApproveFile($safe_get["approve"]);
else if (isset($remove) and isset($admin))
    RemoveFile($safe_get["remove"]);

DisplayFileOptions();

$empty = False;

if (isset($queued) and $admin == True) {

    StartBox("Browse Queued Files");
    for ($i = 0; $i < 5; $i++) {
        $empty |= BrowseFiles($i, 1);
    }
    if (!$empty)
        echo "No queued files.";
    EndBox();

} else {

    StartBox("Browse Files");
    for ($i = 0; $i < 5; $i++) {
        $empty |= BrowseFiles($i, 0);
    }
    if (!$empty)
        echo "No files.";
    EndBox();
}

DisplayFileOptions();

?>
