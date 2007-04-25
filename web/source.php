<?php
include_once "bin/main.php";
$style = '\
body {
    font-size: 100%;
    margin: 2em 4% 2em 4%;
}
a:link, a:visited, a:active, a:focus, a:hover {
    background: transparent;
    cursor: pointer;
}
a, .notice, .error {font: 1em/1 Verdana, Geneva, "Bitstream Vera Sans", Helvetica, sans-serif}
body, code, div, html, span {font: 1em/1 "Bitstream Vera Sans Mono", Fixedsys, monospace}
html {margin: 0}
body, code, html {padding: 0}
a:link, a:visited {text-decoration: none}
a:active, a:focus, a:hover {text-decoration: underline}
a:link, a:visited {color: #303060}
a:active, a:focus {color: #303040}
.error {background: #ecc}
.notice {background: #ddd}
.operator {color: maroon}
.reserved {color: navy}
.exception {color: red}
.number {color: olive}
.definition, .operator, .reserved {font-weight: bold}
.definition, .stdlib {color: blue}
.string {color: purple}
.string span {color: purple}
.string span {font-style: normal}
.string span, .comment span {font-weight: normal}
.string .comment *, .comment .string * {font-weight: normal}

code {
    display: block;
    margin: 0 0 2em 0;
}

html, body {
    background: #eee;
    color: black;
}

.body {
    margin: 0;
    padding: 0;
}

.error, .notice {
    border: 1px solid #4d626f;
    margin: 0 0 2em 0;
    padding: 1em 2% 1em 2%;
}

.comment {
    color: green;
    font-style: italic;
}

.comment span {
    color: green;
    font-style: italic;
}

.string .comment * {
    color: purple;
    font-style: normal;
}

.comment .string * {
    color: green;
    font-style: italic;
}
';

if (isset($_GET["file"]))
{
    $result = mysql_query("SELECT name, filename, author, description FROM files WHERE id=" . $safe_get["file"])
              or MySQL_FatalError();
    $result = mysql_fetch_array($result);

    if (!stristr($result["filename"], ".py"))
    {
        GenerateMinimalHeader("ika", $style);
        FatalError("Invalid file type.");
    }

    $handle = fopen("files/$result[filename]", "r");
    $contents = fread($handle, filesize("files/$result[filename]"));
    fclose($handle);
    $contents = Colorize(NukeHTML($contents));

    GenerateMinimalHeader($result["name"], $style, $result["author"], $result["description"]);
    echo "<h1>Source Viewer -- " . $result["name"] . "</h1>";
    echo "<hr>";
    echo "<a href='files.php?download=" . $safe_get["file"] . "'>Download source code.</a>";
    echo "<hr><br />";
    echo "<code>$contents</code>";
    echo "<hr>";
    echo "<a href='files.php?download=" . $safe_get["file"] . "'>Download source code.</a>";
    echo "<hr><br />";
    die();
}
else
{
    GenerateMinimalHeader("ika", $style);
    FatalError("What are you doing?");
}
?>