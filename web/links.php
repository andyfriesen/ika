<?php

$sCategory = array("ika", "ika Games", "Python", "General Programming",
                   "Software", "Miscellaneous");

function ShowLinks($category, $queued) {
    global $sCategory, $admin, $PHP_SELF;

    $result = mysql_query("SELECT * FROM links WHERE queued='$queued' AND category='$category'");
    $link = mysql_fetch_array($result);

    if (!$link) {
        return;
    }

    echo '<table class="box">';
    echo "<tr><th colspan='2'>{$sCategory[$category]}</th></tr>";

    do {
        // Prepend "http://" if not present in the database entry
        if (preg_match('|^[a-zA-Z]*\\://|', $link["url"]) == 0) {
        $link["url"] = "http://" . $link["url"];
    }

        echo '<tr>';
        echo "<td><a href='$link[url]'>", NukeHTML($link["name"]), "</a></td>";
        echo "<td style='width: 80%'>", NukeHTML($link["description"]), "</td>";  // CHANGE TO CSS!
        if ($admin) {
            if ($queued) {
               echo "<td><a href='$PHP_SELF?approve=$link[id]'>Approve</a></td>";
            }
            echo "<td><a href='$PHP_SELF?delete=$link[id]'>Delete</a></td>";
            echo "<td><a href='$PHP_SELF?edit=$link[id]'>Edit</a></td>";
        }
        echo '</tr>';
    } while ($link = mysql_fetch_array($result));
    echo "</table>";
}


function SubmitLink($url, $title, $category, $description) {
    global $PHP_SELF;

    $result = mysql_query("INSERT INTO links (url, name, category, description) values " .
                          "('$url', '$title', $category, '$description')")
              or MySQL_FatalError();

    Notice("<p>Link queued!  An admin will review it, and add it shortly. " .
           "(well, eventually, at any rate)</p>" .
           "<p><a href='$PHP_SELF'>Return to the links page.</a></p>");
    die();
}


function EditLink($id) {
    global $PHP_SELF, $admin, $sCategory;

    if (!$admin) {
        FatalError("You are not an administrator.");
    }

    $result = mysql_query("SELECT * FROM links WHERE id=$id");
    $a = mysql_fetch_array($result);

    CreateForm("$PHP_SELF?update=$a[id]",
               "URL",         "input" ,    $a["url"],
               "Title",       "input",     NukeHTML($a["name"]),
               "Category",    "select",    $sCategory, $sCategory, $sCategory[$a["category"]],
               "Description", "smalltext", NukeHTML($a["description"]),
               "Submit",      "submit",    ""
    );
}


function UpdateLink($id, $url, $name, $category, $description) {
    $query = "UPDATE links SET "
           . "url='$url', name='$name', category='$category', "
           . "description='$description' WHERE id=$id";
    $result = mysql_query($query) or MySQL_FatalError();

    Notice("Updated successfully.");
    die();
}


function DeleteLink($id) {
    global $admin;
    if (!$admin) {
        FatalError("You are not an administrator.");
    }

    $result = mysql_query("DELETE FROM links WHERE ID='$id'")
              or MySQL_FatalError();

    Notice("Link removed.");
    die();
}


function ApproveLink($id) {
    global $admin;
    if (!$admin) {
        FatalError("You are not an administrator.");
    }

    $result = mysql_query("UPDATE links SET queued=0 WHERE ID='$id'")
              or MySQL_FatalError();

    Notice("Link approved.");
    die();
}

// -----------------------------------------------------------------------------

include "bin/main.php";

VerifyLogin();
GenerateHeader("Links");

if (isset($submit) or isset($update)) {
    for ($c = 0, $i = 0; $i < sizeof($sCategory); $i++) {
        if ($Category == $sCategory[$i]) {
            $c = $i;
            break;
        }
    }

    if (isset($update)) {
        UpdateLink($update, $URL, $Title, $c, $Description);
    } else {
        SubmitLink($URL, $Title, $c, $Description);
    }

} else if (isset($edit)) {
    EditLink($edit);

} else if (isset($delete)) {
    DeleteLink($delete);

} else if (isset($approve)) {
    ApproveLink($approve);

} else if (isset($admin) and isset($queued)) {
    for ($i = 0; $i < sizeof($sCategory); $i++) {
        ShowLinks($i, True);
    }

} else {
    for ($i = 0; $i < sizeof($sCategory); $i++) {
        ShowLinks($i, False);
    }
}

CreateForm("$PHP_SELF?submit=1",
    "URL",         "input" ,    "",
    "Title",       "input",     "",
    "Category",    "select",    $sCategory, $sCategory, $sCategory[0],
    "Description", "smalltext", "",
    "Submit",      "submit",    ""
);

?>
