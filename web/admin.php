<?php

include "bin/main.php";

#select distinct layout, count(*) as c from users group by layout order by c desc;

VerifyLogin();

GenerateHeader("Admin Functions");

if ($admin == True)
{
    StartBox();
    echo "<div class='post'>";
    
    if (isset($stats))
    {
        $query = mysql_query("SELECT DISTINCT layout, COUNT(*) as c from users GROUP BY layout ORDER BY c DESC");
        
        echo "<h3>Layout Distribution</h3><br />";
        
        echo "<table>";
        echo "<tr><th>Layout</th><th>Users</th></tr>";
        while ($res = mysql_fetch_array($query))
            echo "<tr><td>" . $res["layout"] . "</td><td>" . $res[c] . "</td></tr>";
        echo "</table>";
    }
    else if (isset($abil))
    {
        echo "<h3>Admin Abilities/Useful Tips</h3>";
        
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
    else
        echo "Choose an action below.";
        
    echo "</div>";
    EndBox();
    
    StartBox();
    echo "<a href='?stats=True'>User Statistics</a> - ";
    echo "<a href='?privs=True'>Admin Rights</a> - ";
    echo "<a href='?abil=True'>Abilities</a> - ";
    echo "<a href='?explan=True'>Site Make-Up Explanation</a>";
    EndBox();
}
else
{
    Notice("You are not an admin.");
}

?>