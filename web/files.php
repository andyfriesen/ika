<?php

include "includes.php";

# Shows a nice columnized view of the file.
function ShowFile($file, $queued) {
    global $filedir, $admin, $_username, $safe_get;

    $td = "<td>";
    
    if (stristr($file["filename"], ".py")) {
        $url = "source.php?file=$file[id]";
    } else if (strpos($file["filename"], "://") == False) {
        $url = $filedir . $file["filename"];
        if (!file_exists($url))
            $td = "<td style='background-color: #ecc'>";
    } else {
        $url = $file["filename"];
        if (!file_exists($url))
            $td = "<td style='background-color: #dcc'>";
    }
    
    
    if (substr($url, 0, 7) == "http://")
        $td = "<td style='background-color: #cdc'>";

    echo '<tr>';
    echo $td, "<a href='$url'>", NukeHTML($file["name"]), "</a></td>";
    echo $td, FormatName($file["author"]), "</td>";
    echo $td, "$file[date]</td>";
    
    if (strlen($file["description"]) > 1)
        echo $td, NukeHTML($file["description"]), "</td>";
    else
        echo $td, "No description.</td>";

    if ($admin and $file["queued"])
        echo "<td><a href='$PHP_SELF?approve=$file[id]'>Approve</a></td>";
    if ($admin or $_username == $file["author"])
        echo "<td><a href='$PHP_SELF?edit=$file[id]'>Edit</a></td>";
    if ($admin)
        echo "<td><a href='$PHP_SELF?remove=$file[id]'>Remove</a></td>";

    echo "</tr>";
}


function BrowseFiles($categoryid, $queued) {
    global $fileCategory;

    $result = mysql_query("SELECT * FROM files WHERE queued=$queued AND ".
                          "category=$categoryid ORDER BY date DESC")
              or MySQL_FatalError();

    $filelist = array();
    for ($files = 0; $file = mysql_fetch_array($result); $files++)
        $filelist[$files] = $file;

    # no files at all?
    if (!$files) return;
    
    echo '<table class="box">';
    echo "<tr><th class='main' colspan='4'>{$fileCategory[$categoryid]}</th></tr>";
    echo '<tr>';
    echo '<th style="width: 10%">Name</th>';              # Move to stylesheet!
    echo '<th style="width: 10%">Author</th>';            # Move to stylesheet!
    echo '<th style="width: 15%">Date</th>';              # Move to stylesheet!
    echo '<th style="width: 65%">Description</th></tr>';  # Move to stylesheet!

    foreach ($filelist as $file) {
        ShowFile($file, $queued);
    }

    echo "</table>";
}


function CreateFile()
{
    CreateForm("$PHP_SELF?submit=1",
    "Hdr",         "header",    "Submit File",
    "Title",       "input",     "",
    "Author",      "input",     isset($_username) ? $_username : "Anonymous",
    "Filename",    "filename",  isset($Filename)  ? $Filename  : "",
    "Category",    "select",    $fileCategory, $fileCategory, $fileCategory[0],
    "Description", "smalltext", "",
    "Submit",      "submit",    "");
}

function AddFile($title, $author, $description, $category) {
    global $Title, $Author, $Category, $Filename, $Description, $Filename_name,
           $fileCategory, $filedir, $PHP_SELF, $URL;

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
    echo "<p><a href='$PHP_SELF'>Return to the files page.</a></p>";
    EndBox();
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
    echo "<table><tr><td><a class='button' href='$PHP_SELF?create=1'>submit file</a></td></tr></table>";
    EndBox();
}

# -----------------------------------------------------------------------------

include_once "bin/main.php";

GenerateHeader("Files");
VerifyLogin();

DisplayFileOptions();

if (isset($submit))
    AddFile($safe_post["Title"], $safe_post["Name"], $safe_post["Description"], $safe_post["Category"]);
else if (isset($approve) and isset($admin))
    ApproveFile($safe_get["approve"]);
else if (isset($remove) and isset($admin))
    RemoveFile($safe_get["remove"]);

if (isset($queued) and isset($admin)) {
    StartBox("Browse Queued Files");
    for ($i = 0; $i < 5; $i++) {
        BrowseFiles($i, 1);
    }
    EndBox();
} else {
    StartBox("Browse Files");
    for ($i = 0; $i < 5; $i++) {
        BrowseFiles($i, 0);
    }
    EndBox();
}

DisplayFileOptions();

?>
