<?php

function ShowThumbNails($cursor) {
    global $PHP_SELF;

    $count = 0;
    $die = False;
    StartBox();
    echo '<div style="clear: both">&nbsp;</div>';  // CONVERT TO CSS!

    while (True) {
        $image = mysql_fetch_array($cursor);
        if (!$image) {
            $die = True;
            break;
        } else {
            $count++;
	}

        $filename = $image["filename"];
        $title = NukeHTML($image["title"]);
        $id = $image["id"];

        echo '<div style="float: left">';  // CONVERT TO CSS!
        echo '<table>';
        echo '<tr><td>';
        echo "<a href='$PHP_SELF?view=$id'>";
        echo "<img alt='$filename' src='gallery/thumb/$filename' />";
        echo '</a></td></tr><tr><td>';
        echo "<span style='text-align: center'>$title</span>";  // CONVERT TO CSS!
	echo "</td></tr></table></div>";

        if ($die) {
            break;
	}
    }
    echo '<div style="clear: both">&nbsp;</div>';  // CONVERT TO CSS!
    if ($count == 0) {
        Notice("The gallery is empty.");
    }
    EndBox();
}


function ShowImage($id) {
    global $admin, $_username;
    $cursor = mysql_query("select * from gallery where id=$id") or MySQL_FatalError();
    $img = mysql_fetch_array($cursor) or MySQL_FatalError();
    $title = NukeHTML($img["title"]);

    StartBox();
    echo "<img alt='$title' src='gallery/$img[filename]'/>";
    echo "<h2>", NukeHTML($title), "</h2>";
    echo "<p>Submitted by $img[name].</p>";
    echo NukeHTML($img["description"], LVL_BASIC_HTML);
    EndBox();

    if ($admin or $_username == $name) {
        StartBox();
        echo '<table><tr>';
        echo "<td><a href='$PHP_SELF?edit=$id'>Edit</a></td>";
        echo "<td><a href='$PHP_SELF?delete=$id'>Delete</a></td>";
        echo "</tr></table>";
        EndBox();
    }
}


function ShowSubmitImageForm() {
    global $PHP_SELF, $_username;

    //echo "<h2'>Submit an Image</h2>";
    CreateForm("$PHP_SELF",
        "Name",        "static",   isset($_username) ? $_username : "Anonymous",
        "Title",       "input",    "",
        "Filename",    "filename", "",
        "Description", "text",     "",
        "Submit",      "submit",   "submit");
}


function ShowEditImageForm($id) {
    global $PHP_SELF, $_username;

    $cursor = mysql_query("SELECT * FROM gallery WHERE id=$id") or MySQL_FatalError();
    $img = mysql_fetch_array($cursor) or MySQL_FatalError();

    StartBox();
    echo "<h2'>Edit Image</h2>";
    CreateForm("$PHP_SELF?edit=$id",
        "Title",       "input",  NukeHTML($img["title"]),
        "Description", "text",   NukeHTML($img["description"]),
        "Submit",      "submit", "submit"
    );
    EndBox();
}


function SubmitImage() {
    global $Filename, $Filename_name, $Title, $Description, $_username;
    if (!is_uploaded_file($Filename)) {
        FatalError("$Filename did not upload properly.");
    }

    if (file_exists("gallery/$Filename_name")) {
        FatalError("An image called $Filename_name already exists.");
    }

    move_uploaded_file($Filename, "gallery/$Filename_name")
        or FatalError("Upload failed.");

    $name = isset($_username) ? $_username : "Anonymous";

    $result = mysql_query(
        "INSERT INTO gallery ".
        "(name, filename, title, description) VALUES ".
        "('$name', '$Filename_name', '$Title', '$Description')"
    ) or MySQL_FatalError();

    Notice("The image has been added successfully.");
    CreateThumbNail($Filename_name);
}


function CheckPermission($id) {
    global $admin, $_username;
    $allowed = False;

    if ($admin)
        $allowed = True;
    if (!$allowed) {
        $cursor = mysql_query("SELECT name FROM gallery WHERE id=$id")
                  or MySQL_FatalError();
        $row = mysql_fetch_array($cursor)
               or MySQL_FatalError();
        if ($row["name"] == $_username) {
            $allowed = True;
	}
    }

    if (!$allowed) {
        FatalError("You are not allowed to do this.");
    }
}


function UpdateImage($id, $title, $description) {
    CheckPermission($id);

    mysql_query("UPDATE gallery SET title='$title', description='$description' " .
		//($name && $admin ? ", name='$name'" : "") .
                "WHERE id=$id")
        or MySQL_FatalError();
}


function DeleteImage($id) {
    global $PHP_SELF, $_username;

    CheckPermission($id);

    global $reallysure;
    if (isset($reallysure) and $reallysure == True) {
        $cursor = mysql_query("SELECT filename FROM gallery WHERE id=$id");
        $row = mysql_fetch_array($cursor);
        unlink("gallery/$row[filename]");
        unlink("gallery/thumb/$row[filename]");
        mysql_query("DELETE FROM gallery WHERE id=$id") or MySQL_FatalError();
        Box("Image deleted.");
    } else {
        StartBox();
        echo "<p><a href='$PHP_SELF?delete=$id&amp;reallysure=1'>I am sure.</a></p>";
        echo "<p><a href='$PHP_SELF'>Do not do anything.</a></p>";
        EndBox();
    }
}


function CreateThumbNail($fileName) {
    $image = LoadImage("gallery/$fileName") or FatalError("Unable to load $fileName.");
    $size = getimagesize("gallery/$fileName");

    $newImage = imagecreate(200, 150);
    imagecopyresized($newImage, $image, 0, 0, 0, 0, 200, 150, $size[0], $size[1]);
    WriteImage($newImage, "gallery/thumb/$fileName");
    imagedestroy($newImage);
    imagedestroy($image);
}

// -----------------------------------------------------------------------------

include_once "bin/main.php";

GenerateHeader("Gallery");

if (isset($submit)) {
    if (isset($edit)) {
        UpdateImage($edit, $Title, $Description);
        Notice("Image updated!");
    } else {
        SubmitImage();
        die();
    }
} else if (isset($view)) {
    ShowImage($view);
    die();
} else if (isset($edit)) {
    ShowImage($edit);
    ShowEditImageForm($edit);
    die();
} else if (isset($delete)) {
    DeleteImage($delete);
    die();
}

// die() to prevent this from being shown. (PHP has ass as far as flow control structures are concerned)
{
    $cursor = mysql_query("select * from gallery");
    ShowThumbNails($cursor);
    ShowSubmitImageForm();
}

?>
