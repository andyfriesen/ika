<?php

# Enums, so we don't have to mess with the table format for every new category.
$sCategory = array('Engine', 'Games', 'Technical Demonstrations', 'Python Code',
                   'Artwork', 'Miscellaneous');

# Shows a nice columnized view of the file.
function ShowFile($file, $queued) {
    global $filedir, $admin;

    if (stristr($file["filename"], ".py")) {
        $url = "source.php?file=$file[id]";
    } else if (strpos($file["filename"], "://") == False) {
        $url = $filedir . $file["filename"];
    } else {
        $url = $file["filename"];
    }

    echo '<tr>';
    echo "<td><a href='$url'>", NukeHTML($file["name"]), "</a></td>";
    echo "<td>", FormatName($file["author"]), "</td>";
    echo "<td>$file[date]</td>";
    echo "<td>", NukeHTML($file["description"]), "</td>";

    if ($admin) {
        if ($queued) {
            echo "<td><a href='$PHP_SELF?queued=1&amp;approve=$file[id]'>Approve</a></td>";
            echo "<td><a href='$PHP_SELF?queued=1&amp;delete=$file[id]'>Remove</a></td>";
        } else {
            echo "<td><a href='$PHP_SELF?delete=$file[id]'>Remove</a></td>";
        }
    }
    echo "</tr>";
}


function ShowFiles($categoryid, $queued) {
    global $sCategory;

    $result = mysql_query("SELECT * FROM files WHERE queued=$queued AND ".
                          "category=$categoryid ORDER BY date DESC")
              or MySQL_FatalError();

    $filelist = array();
    for ($files = 0; $file = mysql_fetch_array($result); $files++)
        $filelist[$files] = $file;

    # no files at all?
    if (!$files) return;

    echo '<table class="box">';
    echo "<tr><th colspan='4'>{$sCategory[$categoryid]}</th></tr>";
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


function SubmitFile() {
    global $Title, $Author, $Category, $Filename, $Description, $Filename_name,
           $sCategory, $filedir, $PHP_SELF, $URL;

    if (!is_uploaded_file($Filename)) {
        FatalError("$Filename did not upload properly.");
    }

    if (file_exists($filedir.$Filename_name)) {
        FatalError("$Filename: A file with that name already exists.  Rename it and try again.");
    }

    $date = date("Y-m-d");

    for ($c = 0, $i = 0; $i < sizeof($sCategory); $i++) {
        if ($Category == $sCategory[$i]) {
            $c = $i;
            break;
        }
    }

    move_uploaded_file($Filename, $filedir.$Filename_name)
        or FatalError("Upload failed.");

    $query = "INSERT INTO files "
           . "(filename, name, author, description, category, size, date) "
           . "values ('$Filename_name', '$Title', '$Author', '$Description', $c, 0, '$date')";
    $result = mysql_query($query) or MySQL_FatalError();

    StartBox();
    echo '<p>The file has been added to the queue.  It will be reviewed by an admin within a few days.</p>';
    echo "<p><a href='$PHP_SELF'>Return to the files page.</a></p>";
    EndBox();
    die();
}


function ApproveFile() {
    global $approve;
    mysql_query("UPDATE files SET queued=0 WHERE id=$approve");
    Notice("The file has been approved.");
}


function DeleteFile() {
    global $delete, $filedir;

    $result = mysql_query("SELECT queued, filename FROM files WHERE id=$delete");
    $row = mysql_fetch_array($result);

    if ($row["queued"]) {
        unlink($filedir.$row["filename"]);
        mysql_query("DELETE FROM files WHERE id=$delete");
        Box('The file has been deleted and removed from the database.');
    } else {
        mysql_query("UPDATE files SET queued=1 WHERE id=$delete");
        Box('The file has been returned to the file queue.  ' .
            'Remove it from the queue to completely erase it.');
    }
}

# -----------------------------------------------------------------------------

include_once "bin/main.php";

GenerateHeader("Files");
VerifyLogin();

if (isset($submit))
    SubmitFile();
else if (isset($approve) and isset($admin))
    ApproveFile();
else if (isset($delete) and isset($admin))
    DeleteFile();

if (isset($queued) and isset($admin)) {
    for ($i = 0; $i < 5; $i++) {
        ShowFiles($i, 1);
    }
} else {
    for ($i = 0; $i < 5; $i++) {
        ShowFiles($i, 0);
    }
}

CreateForm("$PHP_SELF?submit=1",
    "Title",       "input",     "",
    "Author",      "input",     isset($_username) ? $_username : "Anonymous",
    "Filename",    "filename",  isset($Filename)  ? $Filename  : "",
    "Category",    "select",    $sCategory, $sCategory, $sCategory[0],
    "Description", "smalltext", "",
    "Submit",      "submit",    "");

?>
