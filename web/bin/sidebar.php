<?php #strong should be replaced with a span as it is purely presentational and
      #the br tags need to be removed as well. ?>
<div class="sidebar" id="sidebar">
    <div class="menu" id="menu">
        <a href="news.php">News</a><br />
        <a href="gallery.php">Gallery</a><br />
        <a href="articles.php?view=1"><abbr title="Frequently Asked Questions">FAQ</abbr></a><br />
        <a href="articles.php">Articles</a><br />
        <a href="files.php">Files</a><br />
        <a href="links.php">Links</a><br />
        <a href="forum.php">Forum</a>
        <?php if ($admin == TRUE) { ?>
            <hr />
            <a href="board.php">Old forum</a>
            <hr />
            <a href="news.php?add=1">Add news</a><br />
            <a href="news.php?edit=1">Edit news</a><br />
            <a href="articles.php?queued=1">Q&rsquo;d articles</a><br />
            <a href="files.php?queued=1">Q&rsquo;d files</a><br />
            <a href="links.php?queued=1">Q&rsquo;d links</a><br />
            <a href="admin.php">More admin</a>
        <?php } if (isset($_username)) { ?>
            <hr />
            <span class="username"><?php echo $_username ?></span><br />
            <a href="profile.php">Edit profile</a><br />
            <a href="login.php?logout=1">Log out</a>
        <?php } elseif (!isset($_username)) { ?>
            <hr />
            <a href="register.php">Register</a><br />
            <a href="login.php">Log in</a>
        <?php } ?>
    </div>
    <a href="http://sourceforge.net" id="sflogo">
        <img src="http://sourceforge.net/sflogo.php?group_id=38780"
             width="88" height="31" alt="SourceForge.net Logo" />
    </a>
</div>
