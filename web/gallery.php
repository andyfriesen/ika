<?php

function ShowThumbNails($cursor) {
    global $PHP_SELF;

    $count = 0;
    $die = False;
    StartBox("Gallery Thumbnails");
    echo '<div style="clear: both;">&nbsp;</div>';  // CONVERT TO CSS!
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
        echo '<table style="table-layout: fixed; overflow: hidden">';
        echo '<tr><td>';
        echo "<a href='$PHP_SELF?view=$id'>";
        echo "<img alt='$filename' src='gallery/thumb/$filename' />";
        echo '</a></td></tr><tr><td style="overflow: hidden; width: 200px">';
        echo "<div class='text' style='text-align: center; overflow: hidden; width: 200px; white-space: nowrap'>$title</div>";  // CONVERT TO CSS!
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

function ShowThumbNailsList($cursor) {
    global $admin, $_username;

    $count = 0;
    $die = False;
    StartBox("Gallery List");
    echo '<div style="clear: both;">&nbsp;</div>';  // CONVERT TO CSS!
    
    echo "<table>";
    echo "<th>Thumb</th><th>Title/Submitter</th><th>Description</th>";
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

        echo "<tr>";
        
        echo "<td><a href='" . $_SERVER["PHP_SELF"] . "?view=$id'>";
        echo "<img alt='$filename' src='gallery/thumb/$filename' width='100' height='75'/>";
        echo "</a></td>";
        
        echo "<td style='vertical-align: top'><div><a href='" . $_SERVER["PHP_SELF"] . "?view=$id'>$title</a></div><div class='medium'>submitted by " . FormatName($image["name"]) . "<br />on " . $image["date"] . "</div><br /><div class='tiny'><strong>Views:</strong> " . $image["view_count"] . "</div></td>";  // CONVERT TO CSS!
        
        echo "<td style='vertical-align: top' class='medium'>" . NukeHTML($image["description"], LVL_BASIC_HTML);
        
        if (($admin == True or ($_username == $image["name"])) and $_username) {
            echo " &mdash; ";
            echo "<a href='" . $_SERVER["PHP_SELF"] . "?edit=$image[id]'>[Edit]</a> ";
            echo "<a href='" . $_SERVER["PHP_SELF"] . "?delete=$image[id]'>[Delete]</a> ";
        }
        
        echo "</td>";
        echo "</tr>";

        if ($die) {
            break;
        }
    }
    echo "</table>";
    
    echo '<div style="clear: both">&nbsp;</div>';  // CONVERT TO CSS!
    if ($count == 0) {
        Notice("The gallery is empty.");
    }
    EndBox();
}

function ShowMicroThumbNails($cursor) {
    global $PHP_SELF;

    $count = 0;
    $die = False;
    StartBox("Micro Thumbnails");
    echo '<div style="clear: both;">&nbsp;</div>';  // CONVERT TO CSS!
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
        echo '<table style="table-layout: fixed; overflow: hidden">';
        echo '<tr><td>';
        echo "<a href='$PHP_SELF?view=$id'>";
        echo "<img alt='$filename' src='gallery/thumb/$filename' width='50' height='37' />";
        echo '</a>';
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
    mysql_query("UPDATE gallery SET view_count=view_count+1 WHERE id=$id");
    $cursor = mysql_query("select * from gallery where id=$id") or MySQL_FatalError();
    $img = mysql_fetch_array($cursor) or MySQL_FatalError();
    $title = NukeHTML($img["title"]);
    $name = $img["name"];
    $date = $img["date"];
    $views = $img["view_count"];
    
    StartBox("Thumbnail Details");
    echo "<h2>", $title;
    if ($date)
        echo " <span class='date'>&mdash; $date</span>";
    echo "</h2>";
    
    echo "<div style='clear: both'></div>";
    echo "<a href='gallery/$img[filename]'>";
    echo "<img alt='$title' style='float: left; margin-right: 30px' width='400' height='300' src='gallery/$img[filename]'/>";
    echo "</a>";
    echo "<p>Created by " . FormatName($img["name"]) . ".</p><br />";
    echo "<p class='medium'>" . NukeHTML($img["description"], LVL_BASIC_HTML) . "</p><br />";
    
    echo "<p class='medium'><b>Last edited:</b> " . $img["edit_date"] . "</p><br />";
    if ($admin or $_username == $name) {
        echo "<p class='tiny'>";
        echo "<a href='" . $_SERVER["PHP_SELF"] . "?edit=$id'>[Edit] </a>";
        echo "<a href='" . $_SERVER["PHP_SELF"] . "?delete=$id'>[Delete] </a>";
        echo "</p><br />";
    }
    
    echo "<p class='tiny'><strong>Views:</strong> $views</p>";
    
    echo "<div style='clear: both'></div>";
    EndBox();
}


function ShowSubmitImageForm() {
    global $PHP_SELF, $_username, $admin;
        
    StartBox("Submit Image");
    CreateForm($_SERVER["PHP_SELF"] . "?submit=1",
        "Title",       "input",    "",
        "Name",        ($admin == True) ? "input" : "hidden",   isset($_username) ? $_username : "Anonymous",
        "Filename",    "filename", "",
        "Description", "text",     "",
        "Submit",      "submit",   "submit");
    EndBox();
}


function ShowEditImageForm($id) {
    global $PHP_SELF, $_username, $admin;

    CheckPermission($id);
    
    $cursor = mysql_query("SELECT * FROM gallery WHERE id=$id") or MySQL_FatalError();
    $img = mysql_fetch_array($cursor) or MySQL_FatalError();

    StartBox("Edit Image");
    CreateForm($_SERVER["PHP_SELF"] . "?edit=$id",
        "Title",       "input",  NukeHTML($img["title"]),
        "Name",        ($admin == True) ? "input" : "hidden",   $img["name"],
        "Description", "text",   NukeHTML($img["description"]),
        "Submit",      "submit", "submit"
    );
    EndBox();
}


function SubmitImage() {
    global $safe_post, $_username;
    
    $Title = $safe_post["Title"];
    $Description = $safe_post["Description"];
    
    $Filename = $_FILES["Filename"]["tmp_name"];
    $Filename_name = $_FILES["Filename"]["name"];
    
    if (!is_uploaded_file($Filename)) {
        FatalError("$Filename did not upload properly.");
    }
    if (file_exists("gallery/$Filename_name")) {
        FatalError("An image called $Filename_name already exists.");
    }
    
    #copy($filename, "/home/groups/i/ik/ika/htdocs/gallery/$Filename_name")
    #    or FatalError("Upload failed.");
    
    $name = $safe_post["Name"];

    $date = date("Y-m-d");
    $datetime = date("Y-m-d H:i:s");
    $result = mysql_query(
        "INSERT INTO gallery ".
        "(name, filename, title, description, date, edit_date) VALUES ".
        "('$name', '$Filename_name', '$Title', '$Description', '$date', '$datetime')"
    ) or MySQL_FatalError();
    
    move_uploaded_file($Filename, "gallery/$Filename_name")
        or FatalError("Upload failed.");

    CreateThumbNail($Filename_name);
    
    Notice("The image has been added successfully.");
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


function UpdateImage($id, $title, $name, $description) {
    CheckPermission($id);
    
    $date = date("Y-m-d H:i:s");
    mysql_query("UPDATE gallery SET name='$name', title='$title', description='$description', edit_date='$date' " .
		//($name && $admin ? ", name='$name'" : "") .
                "WHERE id=$id")
        or MySQL_FatalError();
}


function DeleteImage($id) {
    global $PHP_SELF, $_username;

    CheckPermission($id);

    if (isset($_GET["reallysure"])) {
        $cursor = mysql_query("SELECT filename FROM gallery WHERE id=$id");
        $row = mysql_fetch_array($cursor);
        @unlink("gallery/$row[filename]");
        @unlink("gallery/thumb/$row[filename]");
        mysql_query("DELETE FROM gallery WHERE id=$id") or MySQL_FatalError();
        Success("Image deleted.");
    } else {
        StartBox("Confirm Deletion");
        echo "<p><a href='$PHP_SELF?delete=$id&amp;reallysure'>I am sure.</a></p>";
        echo "<p><a href='" . $_SERVER["PHP_SELF"] . "'>Do not do anything.</a></p>";
        EndBox();
        die();
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


function ShowGalleryOptions() {
    
    global $_GET;
    
    StartBox("Options");
    echo "<p class='tiny'>";
    if (isset($_GET["thumbs"]))
        echo "<a href='" . $_SERVER["PHP_SELF"] . "'>[View List]</a> ";
    else
        echo "<a href='" . $_SERVER["PHP_SELF"] . "?thumbs'>[View Thumbs]</a> ";
    echo "<a href='" . $_SERVER["PHP_SELF"] . "?random'>[View Random]</a> ";
    EndBox();
}
// -----------------------------------------------------------------------------

include "bin/main.php";

GenerateHeader("Gallery");

if (GetValue($_GET, "delete")) {
    DeleteImage($_GET["delete"]);
} else if (GetValue($_POST, "submit")) {
    if (GetValue($_GET, "edit")) {
        UpdateImage($_GET["edit"], $safe_post["Title"], $safe_post["Name"], $safe_post["Description"]);
        Notice("Image updated!");
    } else {
        SubmitImage();
        die(); }
} else if (GetValue($_GET, "edit")) {
    ShowEditImageForm($_GET["edit"]);
    die();
} 
    
$cursor = mysql_query("select * from gallery ORDER BY view_count DESC");

if (GetValue($_GET, "view")) {
    ShowImage($_GET["view"]);
    ShowGalleryOptions();
    ShowMicroThumbNails($cursor);
    die();
} else if (isset($_GET["random"])) {
    $num = mysql_num_rows($cursor);
    $idx = rand(0, $num-1);
    $cnt = 0;
    while ($cnt <= $idx) {
        $cnt++;
        $row = mysql_fetch_array($cursor);
    }
    ShowImage($row["id"]);
    $cursor = mysql_query("select * from gallery");
    ShowGalleryOptions();
    ShowMicroThumbNails($cursor);
    die();
}

// die() to prevent this from being shown. (PHP has ass as far as flow control structures are concerned)

ShowGalleryOptions();

if (isset($thumbs))
    ShowThumbNails($cursor);
else
    ShowThumbNailsList($cursor);
    
ShowSubmitImageForm();

?>
