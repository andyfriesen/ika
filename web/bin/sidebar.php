<div class="sidebar">
    <div class="menu">
        <a href="news.php">News</a><br />
        <a href="gallery.php">Gallery</a><br />
        <a href="articles.php?view=1"><abbr title="Frequently Asked Questions">faq</abbr></a><br />
        <a href="articles.php">Articles</a><br />
        <a href="files.php">Files</a><br />
        <a href="links.php">Links</a><br />
        <a href="forum.php">Forum</a>
        <?php if ($admin == True): ?>
            <hr />
            <a href="board.php">Forum-Old</a>
            <hr />
            <a href="news.php?add=1">Add News</a><br />
            <a href="news.php?edit=1">Edit News</a><br />
            <a href="articles.php?queued=1">Q&rsquo;d Articles</a><br />
            <a href="files.php?queued=1">Q&rsquo;d Files</a><br />
            <a href="links.php?queued=1">Q&rsquo;d Links</a><br />
            <a href="admin.php">More Admin</a>
        <?php endif; if (isset($_username)): ?>
            <hr />
            <?php echo "<b>" . $_username . "</b><br /><br />"; ?>
            <a href="profile.php">Edit Profile</a><br />
            <a href="login.php?logout=1">Log Out</a>
        <?php elseif (!isset($_username)): ?>
            <hr />
            <a href="register.php">Register</a><br />
            <a href="login.php">Log In</a>
        <?php endif; ?>
    </div>
    <div style="text-align: right"><a href="http://sourceforge.net">
    <img src="http://sourceforge.net/sflogo.php?group_id=38780&amp;type=1" width="88" height="31" alt="SourceForge.net Logo" /></a></div></div>