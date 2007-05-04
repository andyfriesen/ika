<?php

include 'bin/main.php';

GenerateHeader('Register');

do {
    if (isset($_GET['register'])) {
        $Username = htmlspecialchars($_POST['Username']);

        $result = mysql_query('SELECT count(*) AS the_count FROM users ' .
                              "WHERE name='$Username'");
        
        if ($_POST["Username"] == "" or $_POST["Password"] == "") {
            Error('You must fill out all required fields.');
            unset($_GET['register']);
            break;
        }
        
        $row = mysql_fetch_array($result);
        if ($row['the_count'] > 0) {
            Error('A user by that name already exists.');
            unset($_GET['register']);
            break;
        }
        
        if ($_POST['Password'] != $_POST['Retype']) {
            Error("The passwords don't match! Please try again.");
            unset($_GET['register']);
            break;
        }

        $result = mysql_query('INSERT INTO users (name, passwd) ' .
                              "values ('$Username', sha1('$_POST[Password]'))");
        if (!$result) {
            StartBox("Notice");
            MySQL_NonFatalError();
            EndBox();
            unset($_GET['register']);
            break;
        }

        StartBox('Notice');
        ?>
<p>You are registered now.</p>
<p><a href="login.php">Click here to log in.</a></p>
        <?php
        EndBox();
        
        if (isset($_GET["forum"]))
        {
            $date = date("Y-m-d");
            $time = date("G:i:s");

            $parentid = $safe_post["ID"];
            $name = $safe_post["Username"];
            $subject = $safe_post["Subject"];
            $text = $safe_post["Text"];
            $query = "INSERT INTO board "
                   . "(parentid, subject, name, ip, text, date, time) values "
                   . "('$parentid', '$subject', '$name', '$REMOTE_ADDR', '$text', "
                   . "'$date', '$time');"
            ;
            
            $result = mysql_query($query) or MySQL_FatalError();
            mysql_query("UPDATE board SET parentid=id WHERE parentid=0");
            Success("Post added successfully.");
        }
    }
}
while (FALSE);

if (!isset($_GET['register'])) {
    $username = isset($_POST['Username']) ? $_POST['Username'] : '';
    $password = isset($_POST['Password']) ? $_POST['Password'] : '';
    $retype = isset($_POST['Retype']) ? $_POST['Retype'] : '';
    
    StartBox('Registration Form');
    
    if (!isset($_GET["forum"]))
        CreateForm($_SERVER["PHP_SELF"] . isset($_GET["forum"]) ? "?register&forum" : "?register", 
            'Username', 'input', $username,
            'Password', 'password', $password, 
            'Retype', 'password', $retype, 
            'Submit', 'submit', '');
    else
        CreateForm($_SERVER["PHP_SELF"] . isset($_GET["forum"]) ? "?register&forum" : "?register", 
            'Username', 'input', $username,
            'Password', 'password', $password, 
            'Retype', 'password', $retype, 
            'Submit', 'submit', '',
            'ID', 'hidden', $_POST["ID"],
            'Text', 'hidden', $_POST["Text"],
            'Subject', 'hidden', $_POST["Subject"]);
    
    EndBox();
    
}
