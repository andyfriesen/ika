<?php

include "bin/dontsharethis.php";
include "bin/htmlparse.php";
include "bin/misc.php";

# -----------------------------------------------------------------------------

function GenerateMinimalHeader($title, $style, $author=Null, $description=Null)
{
    $encoding = 'ISO-8859-1';
    $language = 'en';
    $base     = 'http://ika.sourceforge.net';
    $content  = 'text/html';
    $layout   = 'default';

    header("Content-Type: $content; charset=$encoding");
    header("Content-Language: $language");

    echo "<?xml version='1.0' encoding='$encoding'?>";
    echo '<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" ';
    echo '"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">';
    echo "<html xmlns='http://www.w3.org/1999/xhtml' xml:lang='$language' lang='$language'>";
    echo '<head>';
    echo "<title>$title</title>";
    echo "<link href='favicon.ico' rel='shortcut icon' type='image/x-icon'/>";
    echo "<meta http-equiv='Content-Type' content='$content; charset=$encoding'/>";
    echo "<meta http-equiv='Content-Language' content='$language' />";
    if ($description !== Null)
        echo "<meta name='description' content='$description' />";
    if ($author !== Null)
        echo "<meta name='author' content='$author'/>";
    echo "<style type='text/css'>$style</style>";
    echo '</head>';
    echo '<body>';

    echo '<div class="body">';
}


# We may want to seperate some of this meta data into function arguments.
function GenerateHeader($subtitle, $userhack="", $pwhack="")
{
    global $PHP_SELF, $layout, $HTTP_USER_AGENT, $_username, $_password, $admin;

    if ($userhack != "" and $pwhack != "")
    {
        $_username = $userhack;
        $_password = $pwhack;
    }
        
    $encoding = 'ISO-8859-1';
    $language = 'en';
    $author   = 'Andy Friesen';
    $base     = 'http://ika.sourceforge.net';
    //if (stristr(getenv('HTTP_ACCEPT'), 'application/xhtml+xml'))
        //$content = 'application/xhtml+xml';
    //else
        $content = 'text/html';

    header("Content-Type: $content; charset=$encoding");
    header("Content-Language: $language");

    echo "<?xml version='1.0' encoding='$encoding'?>";
    echo "<?xml-stylesheet href='layouts/$layout/style.css' type='text/css' title='default'?>";
    echo '<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" ';
    echo '"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">';
    echo "<html xmlns='http://www.w3.org/1999/xhtml' xml:lang='$language' lang='$language'>";
    echo '<head>';
    echo "<title>ika - $subtitle</title>";
    echo "<link href='layouts/$layout/style.css' rel='stylesheet' type='text/css' title='$layout'/>";
    echo "<link href='favicon.ico' rel='shortcut icon' type='image/x-icon'/>";
    echo "<meta http-equiv='Content-Type' content='$content; charset=$encoding'/>";
    echo "<meta http-equiv='Content-Language' content='$language' />";
    echo '<meta name="keywords" content="ika, games, GCE, Role-Playing, RPG, Python"/>';
    echo '<meta name="description" content="ika is a game engine designed to allow one to make a console style RPG with a minimum of code, while allowing one who is willing to code an impressive level of power over the look and feel of one&apos;s project, thanks to the embedded Python interpreter." />';
    echo "<meta name='author' content='$author'/>";
    echo "<meta name='copyright' content='Copyright &copy; 2004 $author'/>";
    echo '<meta name="ROBOTS" content="NOFOLLOW, NOIMAGEINDEX, NOIMAGECLICK"/>';
    //echo "<base href='$base'/>";
    ?>
    
    <script language="javascript">
    function quoteText(id)
    {
        var name = self.document.getElementById("name" + id).textContent;
        var text = self.document.getElementById("text" + id).innerHTML;
        text = text.replace(/<.+>/g, "");
        self.document.forms[0].elements[2].value += '<quote name="' + name + '">' + text + '</quote>';
    }
    </script>
    <?php
    echo '</head>';
    echo '<body>';

    if (!strstr($HTTP_USER_AGENT, "MSIE")) {
        echo '<div class="logo"></div>';
    } else {
        echo '<div class="ielogo"></div>';
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

    if (file_exists("layouts/$layout/sidebar.php"))
        include "layouts/$layout/sidebar.php";
    else
        include "bin/sidebar.php";

    echo '<div class="body">';
    echo "<div class='header'><h1>$subtitle</h1></div>";
}


function GenerateFooter()
{
    echo '</div>';
    echo '<div class="disclaimer">All site content copyright &copy; 2007.<br />The ika engine and its related tools are copyright &copy; 2001-2007 Andy Friesen.<br />E-mail <a href="mailto:troy.potts@gmail.com">Thrasher</a> with any questions, comments, or concerns.</div>';
    echo '</body>';
    echo '</html>';
}
register_shutdown_function("GenerateFooter");

# -----------------------------------------------------------------------------

# YOU CANNOT HAVE THE SOURCE FOR THIS FUNCTION BC IT HAS PASSWERDS AND STUFF K
ConnectToDB();

$safe_get = SanitizeData($_GET);
$safe_post = SanitizeData($_POST);

$_username = GetValue($_COOKIE, "_username", null);
$_password = GetValue($_COOKIE, "_password", null);

if (isset($_username) and !isset($layout) and 0)   # CURRENT HACK FOR COMPATIBILITY
{
    $userinfo = GetUserInfo($_username);
    $layout = $userinfo["layout"];
}

if (!isset($layout))
    $layout = "default";
    
$admin = False;

?>