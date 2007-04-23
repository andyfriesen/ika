<?php

include "bin/main.php";
include "includes.php";

#select distinct layout, count(*) as c from users group by layout order by c desc;

VerifyLogin();

GenerateHeader("Admin Functions");

if ($admin == True)
{
    
    if (isset($stats))          $subtitle = "Statistics";
    else if (isset($users))     $subtitle = "User List";
    else if (isset($privs))     $subtitle = "Admin Rights";
    else if (isset($abil))      $subtitle = "Functions";
    else if (isset($explan))    $subtitle = "Site Explanation";
    else if (isset($collapse))  $subtitle = "Collapse Forum (P)";
    else                        $subtitle = "Instructions";
    
    StartBox($subtitle);
    
    echo "<div class='post'>";
    
    $layoutsCopy = $layouts;
    
    if (isset($stats))
    {
        $query = mysql_query("SELECT DISTINCT layout, COUNT(*) as c from users GROUP BY layout ORDER BY c DESC");
        
        echo "<table class='box'>";
        echo "<tr><th class='main' colspan=3>Layout Distribution</th></tr>";
        echo "<tr><th>Layout</th><th>Layout Name</th><th>Users</th></tr>";
        while ($res = mysql_fetch_array($query))
        {
            $idx = array_search($res["layout"], $layouts);
            $layoutsCopy[$idx] = False;
            echo "<tr><td width='20%'><a href='$PHP_SELF?stats=1&layout=" . $res["layout"] . "'>" . $res["layout"] . "</td><td>" . $layoutNames[$idx] . "</td><td width='20%'>" . $res[c] . "</td></tr>";
        }
        foreach ($layoutsCopy as $k)
        {
            if ($k)
            {
                $idx = array_search($k, $layouts);
                echo "<tr><td width='20%'><a href='$PHP_SELF?stats=1&layout=" . $k . "'>" . $k . "</td><td>" . $layoutNames[$idx] . "</td><td width='20%'>0</td></tr>";
            }
        }
        echo "</table>";
    }
    else if (isset($users))
    {
        $admins = mysql_query("SELECT name FROM users WHERE admin=1 ORDER BY name ASC");
        $users = mysql_query("SET SQL_BIG_SELECTS=1;");
        $users = mysql_query("SELECT DISTINCT users.name AS name FROM users, board WHERE users.name = board.name AND users.admin = 0 ORDER BY users.name ASC");
        $num_admins = mysql_num_rows($admins);
        $num_users = mysql_num_rows($users);
        echo "<table class='box'>";
        echo "<tr><th class='main' colspan=5>Administrators</th></tr>";
        echo "<tr><th colspan=5>Total: $num_admins</th></tr>";
        
        $cnt = 0;
        $td = 0;
        
        echo "<tr>";
        while ($row = mysql_fetch_array($admins))
        {
            if ($cnt == 0)
                echo "<td width='20%' style='vertical-align: top'>";
            echo FormatName($row["name"]);
            if (($cnt+1) >= ((int) ($num_admins / 5)) + ((($num_admins % 5) > $td) ? 1 : 0))
            {
                $cnt = 0;
                $td++;
                echo "</td>";
            }
            else
            {
                echo "<br />";
                $cnt++;
            }
        }
        echo "</tr>";

        $cnt = 0;
        $td = 0;
        
        echo "<tr><th class='main' colspan=5>Users</th></tr>";
        echo "<tr><th colspan=5>Total: $num_users</th></tr>";        
        echo "<tr>";
        while ($row = mysql_fetch_array($users))
        {
            if ($cnt == 0)
                echo "<td width='20%' style='vertical-align: top'>";
            echo FormatName($row["name"]);
            if (($cnt+1) >= ((int) ($num_users / 5)) + ((($num_users % 5) > $td) ? 1 : 0))
            {
                $cnt = 0;
                $td++;
                echo "</td>";
            }
            else
            {
                echo "<br />";
                $cnt++;
            }
        }
        echo "</tr>";
        
        echo "</table>";
    }
    else if (isset($privs))
    {
        
        $valid = False;
        
        if (isset($safe_post["Submit"]))
        {
            $result = mysql_query("SELECT name, admin FROM users WHERE name='" . $safe_post["Username"] . "'");
            
            if (mysql_num_rows($result) > 0)
            {
                $row = mysql_fetch_array($result);
                if ($row["admin"] == 1)
                    Error("User '" . $safe_post["Username"] . "' already has admin access.");
                else
                    $valid = True;
            }
            else
                Error("User '" . $safe_post["Username"] . "' doesn't exist.");
        }
        
        if ($valid == True)
        {
            $result = mysql_query("UPDATE users SET admin=1 WHERE name='" . $safe_post["Username"] . "'");
            
            if ($result)
            {
                StartBox("Notice");
                echo "<p>Admin access granted.</p>";
                echo "<p><a href='$PHP_SELF'>Return to more admin functions.</a></p>";
                EndBox();
            }
            else
            {
                FatalError("MySQL error?");
            }
        }
        else
        {
            StartBox("Give Admin Access");
            CreateForm("$PHP_SELF?privs=True&giveadmin=True",
            "Username",    "input", "",
            "Submit",  "submit", ""
            );
            EndBox();
        }
    }
    else if (isset($abil))
    {
        echo "<ul>";
        echo "<li><b>Delete Posts.</b> You have the ability to delete any offensive, irrelevant, or spam posts on the board.</li>";
        echo "<li><b>Use 'layout=whatever' in the URL.</b> You can quickly see what a page looks like in a different layout by appending this to the GET data list.</li>";
        echo "<li><b>Give admin rights.</b> Click on 'Admin Rights' below to administer admin status. Only super-admins may take it away. (Super-admins don't exist yet.)</li>";
        echo "</ul>";
    }
    else if (isset($explan))
    {
        echo "<h3>Site Make-Up Explanation</h3>";
        
        echo "<p>I don't really feel comfortable giving out the FTP/shell/MySQL info here, so ask me (Thrasher) for it on IRC. Until we get some kind of access level thing for users going, it'll be like this, and this section will be rather general-purpose.</p>";
        
        echo "<p>If you have FTP access, go to <b>/home/groups/i/ik/ika/htdocs</b> to get to the root web directory. From there, you can upload and change any files you like.</p>";
        
        echo "<p>To make a new layout, create a new folder under <b>/layouts</b> and add the necessary entries in <b>profile.php</b>. Put the necessary stylesheets in the layout folder. If your layout uses a different sidebar, name it <b>sidebar.php</b> and put it in that directory. The site code will automatically use this instead.</p>";
        
        echo "<p>I... can't really think of much else to put here right now. Oops.</p>";
    }
    else if (isset($collapse))
    {
        if ($_POST["Submit"])
        {
            echo "<p>Fixing anomalies...</p>";
            mysql_query("UPDATE board SET parentid=0 WHERE id=parentid AND id >= " . $_POST["Start"] . " AND id <= " . $_POST["End"]);
            echo "<p>Anomalies fixed.</p>";
            
            echo "<p>Collapsing forum...</p>";
            $all = mysql_query("SELECT id, parentid FROM board WHERE parentid!=0 AND id >= " . $_POST["Start"] . " AND id <= " . $_POST["End"] . " ORDER BY id");
            
            while ($row = mysql_fetch_array($all))
            {
                echo "<p>Post " . $row["id"] . "... (";
                $row2 = $row;
                while ($row2["parentid"] != 0)
                {
                    echo $row["parentid"] . ", ";
                    $row2 = mysql_fetch_array(mysql_query("SELECT id, parentid FROM board WHERE id=" . $row2["parentid"]));
                    $row["parentid"] = $row2["id"];
                }
                echo ") new parent is " . $row["parentid"] . ".</p>";
                mysql_query("UPDATE board SET parentid=" . $row["parentid"] . " WHERE id=" . $row["id"]);
            }
        
            echo "<p>Forum collapsed.</p>";
        }
        else
        {
            StartBox();
            CreateForm($_SERVER["PHP_SELF"] . "?collapse=True",
            "Instructions", "static",   "Enter a post range to collapse.",
            "Start",   "input", "",
            "End", "input",  "",
            "Submit",  "submit", "");
            EndBox();
        }
    }
    else
        echo "Choose an action below.";
        
    echo "</div>";
    EndBox();
    
    StartBox("Admin Options");
    echo "<table><tr>";
    echo "<td><a class='button' href='?stats=True'>statistics</a></td>";
    echo "<td><a class='button' href='?users=True'>user list</a></td>";
    echo "<td><a class='button' href='?privs=True'>admin rights</a></td>";
    echo "<td><a class='button' href='?abil=True'>functions</a></td>";
    echo "<td><a class='button' href='?explan=True'>site explanation</a></td>";
    echo "</tr><tr>";
    echo "<td><a class='button' href='?collapse=True'>collapse forum (p)</a></td>";
    echo "</tr></table>";
    EndBox();
}
else
{
    Error("You are not an admin.");
}

?>