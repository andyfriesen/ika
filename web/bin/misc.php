<?php

$cachedUserInfo = array();


function GetUserInfo($userName)
{
    global $cachedUserInfo;

    if (array_key_exists($userName, $cachedUserInfo))
        return $cachedUserInfo[$userName];
    else
    {
        $result = mysql_query("SELECT * FROM users WHERE name='$userName'");
        $row = mysql_fetch_array($result);
        $cachedUserInfo[$userName] = $row;
        return $row;
    }
}


function VerifyLogin()
{
    global $_username, $_password;

    if (!isset($_username) and !isset($_password))  # not logged in, nothing fishy going on
        return;

    if (!isset($_password))
        FatalError("Something fishy with the login stuff.");

    $result = mysql_query("SELECT passwd, '$_password'=passwd pass_is_good from users where name='$_username'");
    $row = mysql_fetch_array($result);

    if (!$row["pass_is_good"]) {
        echo $_password . "<br />\n" . $row["passwd"] . "<br />" . $row["pass_is_good"] . "<br />";
        FatalError("Something fishy with the login stuff.");
    }
}


# This should work in-place.
# Also, we need a better solution.

function ObfuscateEmail($email)
{
    return str_replace(
        array("@", "."),
        array(" @AT@ ", " ...dot.. "),
        $email
    );
}


function SanitizeData($src)
{
    $dest = array();
    foreach($src as $k => $v)
        $dest[$k] = mysql_real_escape_string($v);
    return $dest;
}

function FormatName($name)
{
    $user = GetUserInfo($name);
    if (!$user)
      return "<em>$name</em>";  // CHANGE TO CSS!

    $name = $user["name"];
    return "<a href='profile.php?view=$name'>$name</a>";

    return $s;
}


function MakePost($title, $text, $signature, $level=LVL_NO_HTML, $date="", $time="")
{
    echo '<div class="post">';
    echo '<h2>';
    echo NukeHTML($title);
    if ($date and $time)
        echo " <span class='date'>&mdash; $date $time</span>";
    echo '</h2>';
    echo '<div class="text">', NukeHTML($text, $level), '</div>';
    echo '<div class="signature">&mdash; ', FormatName($signature), '</div>';
    echo '</div>';
}


function MakeSigPost($title, $text, $signature, $level=LVL_NO_HTML, $date="", $time="")
{
    echo '<div class="post">';
    echo '<h2>';
    echo NukeHTML($title);
    if ($date and $time)
        echo " <span class='date'>&mdash; $date $time</span>";
    echo '</h2>';
    echo '<div class="text">', NukeHTML($text, $level), '</div>';
    echo '<div class="signature">', $signature, '</div>';
    echo '</div>';
}


function Box($text) {
    echo "<div class='box'><p>$text</p></div>";
}

function StartBox($hdr='', $class='') {
    if ($hdr)
        echo "<div class='main'>$hdr</div>";
    echo "<div class='box $class'>";
}

function EndBox() {
    echo '</div>';
}

function Error($text) {
    echo "<div class='error'>$text</div>";
}

function FatalError($text) {
    die("<div class='error'>$text</div>");
}

function Notice($text) {
    echo "<div class='notice'>$text</div>";
}

function MySQL_NonFatalError() {
    Error("MySQL error (".mysql_errno()."): ".mysql_error());
}

function MySQL_FatalError() {
    FatalError("MySQL error (".mysql_errno()."): ".mysql_error());
}


function IsAdmin()
{
    global $_username;
    $user = GetUserInfo($_username);
    return $user["admin"];
}


# generic form creating function thingo
# $accepturl is the URL that the form redirects to when it's submitted
# additional parameters are control name,type,initial value triplets
function CreateForm($accepturl)
{
    $args = func_get_args();
    
    echo "<form enctype='multipart/form-data' action='$accepturl' method='post'>";
    echo '<table>';
  
    $i = 1;
    $tabindex = 1;
    
    while ($i < func_num_args())
    {
        $name = $args[$i];
        $type = $args[$i + 1];
        $value = $args[$i + 2];
        
        echo "<tr>";
        
        switch ($type)
        {
            case "input":
                echo "<td><label for='form$i'>$name</label></td><td>";
                echo "<input type='text' name='$name' value='$value' id='form$i' tabindex='$tabindex' maxlength='80' size='40'/>";
                echo "</td>";
                $tabindex++;
                break;

            case "text":
                echo "<td><label for='form$i'>$name</label></td>";
                echo '<td>';
                echo "<textarea width='100%' name='$name' cols='80' rows='10' id='form$i' tabindex='$tabindex'>$value</textarea>";
                echo "</td>";
                $tabindex++;
                break;

            case "smalltext":
                echo "<td><label for='form$i'>$name</label></td>";
                echo '<td colspan="2">';
                echo "<textarea name='$name' cols='80' rows='3' id='form$i' tabindex='$tabindex'>$value</textarea>";
                echo "</td>";
                $tabindex++;
                break;

            case "static":
                echo "<td>$name</td><td>$value</td>";
                $tabindex++;
                break;
            
            case "header":
                if (!$value)
                    $value = $name;
                echo "<th class='main' colspan='2'>$value</td>";
                break;
                
            case "subheader":
                if (!$value)
                    $value = $name;
                echo "<th colspan='2'>$value</td>";
                break;
                
            case "description":
                if (!$value)
                    $value = $name;
                echo "<td colspan='2'>$value</td>";
                break;

            case "filename":
                echo "<td><label for='form$i'>$name</label></td><td>";
                echo "<input type='file' name='$name' value='$value' id='form$i' tabindex='$tabindex'/>";
                echo "</td>";
                $tabindex++;
                break;

            case "select":
                $names = $args[$i + 3];
                $default = $args[$i + 4];
                $i += 2;
                echo "<td><label for='form$i'>$name</label></td><td>";
                echo "<select name='$name' id='form$i' tabindex='$tabindex'>";
                for ($j = 0; $j < sizeof($value); $j++)
                {
                    if ($default == $value[$j])
                        echo "<option value='{$value[$j]}' selected='selected'>{$names[$j]}</option>";
                    else
                        echo "<option value='{$value[$j]}'>{$names[$j]}</option>";
                }
                echo "</select></td>";
                $tabindex++;
                break;

            case "password":
                echo "<td><label for='form$i'>$name</label></td><td>";
                echo "<input type='password' name='$name' value='$value' id='form$i' tabindex='$tabindex'/>";
                echo "</td>";
                $tabindex++;
                break;
            
            case "preview":
                if (!$value)
                    $value = $name;
                
                echo "<td colspan='2'><input type='submit' name='$value' value='$name' id='form$i' tabindex='$tabindex'/></td>";
                $tabindex++;
                break;
                
            case "submit":
                if (!$value)
                    $value = $name;

                echo "<td colspan='2'><input type='submit' name='$value' value='$name' id='form$i' tabindex='$tabindex'/></td>";
                $tabindex++;
                break;

            case "preview+submit":
                
                echo "<td colspan='2'><input type='submit' name='Preview' value='Preview' id='form$i' tabindex='$tabindex'/>";
                $tabindex++;
                echo "<input type='submit' name='Submit' value='Submit' id='form$i' tabindex='$tabindex'/></td>";
                $tabindex++;
                break;
                
            default:
                Error("CreateForm, unknown control type '$type'.");
        }

        echo "</tr>";
        
        $i += 3;
    }
    echo "</table></form>";
}


function LoadImage($fileName){
    $ext = strtolower(strrchr($fileName, "."));

    switch ($ext)    {
        case ".png":  return imagecreatefrompng($fileName);
        case ".gif":  return imagecreatefromgif($fileName);
        case ".jpg":
        case ".jpe":
        case ".jpeg": return imagecreatefromjpeg($fileName);
        case ".bmp":  return imagecreatefromwbmp($fileName);
        default:
            die("Unknown image extension '$ext'");
    }
}


function WriteImage($image, $fileName)
{
    $ext = strtolower(strrchr($fileName, "."));

    switch ($ext)
    {
        case ".png":  imagepng($image, $fileName);      break;
        case ".gif":  imagegif($image, $fileName);      break;
        case ".jpg":
        case ".jpe":
        case ".jpeg": imagejpeg($image, $fileName, 50); break;
        case ".bmp":  imagewbmp($image, $fileName);
        default:
            die("Unknown image extension '$ext'");
    }
}

?>