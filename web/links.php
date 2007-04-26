<?php

# This needs to be added to the database someday.
$sCategory = array('ika', 'ika Games', 'Python', 'General Programming',
                   'Software', 'Miscellaneous');


function ShowLinks($category, $queued) {
    global $sCategory, $admin, $PHP_SELF;

    $result = mysql_query("SELECT * FROM links WHERE queued='$queued' AND
                           category='$category'");
    if (!($link = mysql_fetch_array($result))) {
        return FALSE;
    }

    ?>
<table class="box">
<tr><th colspan="2"><?php echo $sCategory[$category] ?></th></tr>
    <?php

    do {
        if (preg_match('|^[a-zA-Z]*\\://|', $link['url']) == 0) {
            # Prepend "http://" if not present in the database entry
            $link['url'] = "http://$link[url]";
        }
        $name = NukeHTML($link['name']);
        $description = NukeHTML($link['description']);
        echo "<tr><td><a href=\"$link[url]\">$name</a></td>
              <td style=\"width: 80%\">$description</td>";  # CHANGE TO CSS!
        if ($admin) {
            if ($queued) {
               echo "<td><a href=\"$PHP_SELF?queued=$queued&approve=$link[id]\">Approve</a></td>";
            }
            echo "<td><a href=\"$PHP_SELF?queued=$queued&delete=$link[id]\">Delete</a></td>
                  <td><a href=\"$PHP_SELF?queued=$queued&edit=$link[id]\">Edit</a></td>";
        }
        echo '</tr>';
    } while ($link = mysql_fetch_array($result));
    echo '</table>';

    return TRUE;
}


function SubmitLink($url, $title, $category, $description) {
    global $PHP_SELF;

    $result = mysql_query("INSERT INTO links (url, name, category, description) values
                           ('$url', '$title', $category, '$description')")
              or MySQL_FatalError();

    Notice("<p>Link queued! An admin will review it, and add it shortly. (Well,
            eventually, at any rate.)</p>");
}


function EditLink($id) {
    global $PHP_SELF, $admin, $sCategory;

    if (!$admin) {
        FatalError('You are not an administrator.');
    }

    $result = mysql_query("SELECT * FROM links WHERE id=$id");
    $a = mysql_fetch_array($result);

    CreateForm("$PHP_SELF?update=$a[id]",
               'URL',         'input',     $a['url'],
               'Title',       'input',     NukeHTML($a['name']),
               'Category',    'select',    $sCategory, $sCategory, $sCategory[$a['category']],
               'Description', 'smalltext', NukeHTML($a['description']),
               'Submit',      'submit',    '');
}


function UpdateLink($id, $url, $name, $category, $description) {
    $query = "UPDATE links SET
              url='$url', name='$name', category='$category',
              description='$description' WHERE id=$id";
    $result = mysql_query($query) or MySQL_FatalError();

    Notice('Updated successfully.');
}


function DeleteLink($id) {
    global $admin;
    if (!$admin) {
        FatalError("You are not an administrator.");
    }

    $result = mysql_query("DELETE FROM links WHERE ID='$id'")
              or MySQL_FatalError();

    Notice('Link removed.');
}


function ApproveLink($id) {
    global $admin;
    if (!$admin) {
        FatalError('You are not an administrator.');
    }

    $result = mysql_query("UPDATE links SET queued=0 WHERE ID='$id'")
              or MySQL_FatalError();

    Notice('Link approved.');
}

# -----------------------------------------------------------------------------

include 'bin/main.php';

VerifyLogin();
GenerateHeader('Links');

$empty = FALSE;

if (isset($_GET['submit']) or isset($_GET['update'])) {
    for ($c = 0, $i = 0; $i < sizeof($sCategory); $i++) {
        if ($_POST['Category'] == $sCategory[$i]) {
            $c = $i;
            break;
        }
    }
    if (isset($_GET['update'])) {
        UpdateLink($_GET['update'], $_POST['URL'], $_POST['Title'], $c, $_POST['Description']);
    } else {
        SubmitLink($_POST['URL'], $_POST['Title'], $c, $_POST['Description']);
    }
} else if (isset($_GET['edit'])) {
    EditLink($_GET['edit']);
} else if (isset($_GET['delete'])) {
    DeleteLink($_GET['delete']);
} else if (isset($_GET['approve'])) {
    ApproveLink($_GET['approve']);
}

if (isset($admin) and isset($_GET['queued'])) {
    StartBox('Browse Queued Links');
    for ($i = 0; $i < sizeof($sCategory); $i++) {
        $empty |= ShowLinks($i, 1);
    }
    if (!$empty) {
        echo 'No queued links.';
    }
} else {
    StartBox('Browse Links');
    for ($i = 0; $i < sizeof($sCategory); $i++) {
        $empty |= ShowLinks($i, 0);
    }
    if (!$empty) {
        echo 'No links.';
    }
    EndBox();
}

StartBox('Submit Link');
CreateForm("$PHP_SELF?submit=1",
           'URL',         'input' ,    '',
           'Title',       'input',     '',
           'Category',    'select',    $sCategory, $sCategory, $sCategory[0],
           'Description', 'smalltext', '',
           'Submit',      'submit',    '');
EndBox();
