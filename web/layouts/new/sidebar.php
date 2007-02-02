<div class="sidebar">
    <div class="menu">
        <a href="news.php">news</a>
        <a href="gallery.php">gallery</a>
        <a href="articles.php">articles</a>
        <a href="files.php">files</a>
        <a href="links.php">links</a>
        <a href="board.php">forum</a>
        <?php if ($admin == True): ?>
            <a href="news.php?edit=1">edit&nbsp;news</a>
            <a href="news.php?add=1">add&nbsp;news</a>
            <a href="articles.php?queued=1">queued&nbsp;articles</a>
            <a href="files.php?queued=1">queued&nbsp;files</a>
            <a href="links.php?queued=1">queued&nbsp;links</a>
        <?php endif; if (isset($_username)): ?>
            <a href="profile.php">edit&nbsp;profile</a>
            <a href="login.php?logout=1">log&nbsp;out</a>
        <?php elseif (!isset($_username)): ?>
            <a href="register.php">register</a>
            <a href="login.php">log&nbsp;in</a>
        <?php endif; ?>
    </div>
    <!--<a href="http://sourceforge.net">
    <img src="http://sourceforge.net/sflogo.php?group_id=38780&amp;type=1" width="88" height="31" alt="SourceForge.net Logo" />
    </a>-->
</div>