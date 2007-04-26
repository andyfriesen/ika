<?php

include "bin/main.php";

GenerateHeader("Register");

do {
    if (isset($_GET['register'])) {
        $Username = htmlspecialchars($_POST['Username']);

        $result = mysql_query('SELECT count(*) AS the_count FROM users ' .
                              "WHERE name='$Username'");
        $row = mysql_fetch_array($result);
        if ($row['the_count'] > 0) {
            Error('A user by that name already exists.');
            unset($_GET['register']);
            break;
        }

        if ($_POST['Password'] != $_POST['Retype']) {
            Error('The passwords don\'t match! Please try again.');
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
<p><a href="index.php">Return to the main page.</a></p>
        <?php
        EndBox();
    }
}
while (FALSE);

if (!isset($_GET['register'])) {
    $username = isset($_GET['Username']) ? $_GET['Username'] : '';
    $password = isset($_GET['Password']) ? $_GET['Password'] : '';
    $retype = isset($_GET['Retype']) ? $_GET['Retype'] : '';
    StartBox('Registration Form');
    CreateForm("$PHP_SELF?register=1", 'Username', 'input', $username,
               'Password', 'password', $password, 'Retype', 'password',
               $retype, 'Submit', 'submit', '');
    EndBox();
}

?>
