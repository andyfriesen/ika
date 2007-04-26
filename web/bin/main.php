<?php

include "bin/dontsharethis.php";
include "bin/htmlparse.php";
include "bin/misc.php";


function BasicHeader($title, $author, $description) {
    header("Content-Type: text/html; charset=ISO-8859-1");
    header("Content-Language: en");

?><!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN"
          "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">
<head>
<title><?php echo $title ?></title>
<link href="favicon.ico" rel="shortcut icon" type="image/x-icon" />
    <?php if ($description !== Null): ?>
<meta name="description" content="<?php echo $description ?>" />
    <?php endif; if ($author !== Null): ?>
<meta name="author" content="<?php echo $author ?>" />
    <?php endif;
}


function GenerateMinimalHeader($title, $style, $author=Null, $description=Null) {
    BasicHeader($title, $author, $description);
?><style type="text/css"><?php echo "$style" ?></style>
</head>
<body>
<div class="body">
    <?php
}


function GenerateHeader($subtitle, $userhack="", $pwhack="") {
    global $PHP_SELF, $layout, $HTTP_USER_AGENT, $_username, $_password, $admin;

    if ($userhack != "" and $pwhack != "") {
        $_username = $userhack;
        $_password = $pwhack;
    }

    if (isset($_username)) {
        $result = mysql_query("SELECT admin, passwd='$_password' AS pass_is_good FROM users WHERE name='$_username'");
        if (!$result) {
            unset($_username);
        } else {
            $row = mysql_fetch_array($result);

            if (!isset($_password) or !$row["pass_is_good"]) {
                unset($_username);
                unset($_password);
            } else {
                $admin = $row["admin"];
            }
        }
    }

    $logo = strstr($HTTP_USER_AGENT, "MSIE") ? 'ielogo' : 'logo';

    BasicHeader("ika" . " - " . $subtitle, "Andy Friesen",
                "ika is a game engine designed to allow one to make a" .
                "console-style RPG with a minimum of code, while allowing one " .
                "who is willing to code an impressive level of power over the " .
                "look and feel of one's project, thanks to the embedded " .
                "Python interpreter.");

?><link href="layouts/<?php echo $layout ?>/style.css" rel="stylesheet"
      type="text/css" title="<?php echo $layout ?>" />
<meta name="keywords" content="ika, games, GCE, Role-Playing, RPG, Python" />
<meta name="copyright" content="Copyright &copy; 2007 Andy Friesen" />
<meta name="ROBOTS" content="NOFOLLOW, NOIMAGEINDEX, NOIMAGECLICK" />
<script type="text/javascript" src="bin/script.js"></script>
</head>
<body>
<div class="<?php echo $logo ?>">

<?php
    if (file_exists("layouts/$layout/sidebar.php")) {
        include "layouts/$layout/sidebar.php";
    } else {
        include "bin/sidebar.php";
    }
?>

<div class="body">
<div class='header'>
<h1><?php echo $subtitle?></h1>
</div>
    <?php
}


function GenerateFooter() {
?></div>
<div class="disclaimer">All site content copyright &copy; 2007.<br />The ika
engine and its related tools are copyright &copy; 2001&ndash;2007 Andy Friesen.
<br />E-mail <a href="mailto:troy.potts@gmail.com">Thrasher</a> with any
questions, comments, or concerns.</div>
</body>
</html><?php
}
register_shutdown_function("GenerateFooter");


# YOU CANNOT HAVE THE SOURCE FOR THIS FUNCTION BC IT HAS PASSWERDS AND STUFF K
ConnectToDB();

$safe_get = SanitizeData($_GET);
$safe_post = SanitizeData($_POST);

$_username = GetValue($_COOKIE, "_username", NULL);
$_password = GetValue($_COOKIE, "_password", NULL);

# CURRENT HACK FOR COMPATIBILITY
if (isset($_username) and !isset($layout) and FALSE) {
    $userinfo = GetUserInfo($_username);
    $layout = $userinfo["layout"];
}

if (!isset($layout)) {
    $layout = "default";
}

$admin = False;

?>
