<?php

function pattern($text, $mod='') { return "/$text/$mod"; }
function tag($text) { return "&lt;$text&gt;"; }
function endtag($text) { return "&lt;\\/$text&gt;"; }
function newlines() { return "[\n\r]*"; }
function attr($name, $i=1) { return "$name=(&quot;|&\#039;)(.+)\\$i"; }


define("LVL_NO_HTML", 0);
define("LVL_BASIC_HTML", 1);
define("LVL_COMPLEX_HTML", 2);


function MakeToC($text)
{
    preg_match_all("|<h(.)>(.+)</h.>|US", $text, $matches, PREG_PATTERN_ORDER);
    $list = "<h3>Table of Contents</h3><ul>";
    $level = 1;
    foreach ($matches as $match)
    {
        $newlevel = $match[1] - 2;
        if ($newlevel > $level)
            for ($i = 0; $i < $newlevel - $level; $i++)
                $list .= "<ul>";
        $list .= "<li>$match[2]</li>";
        if ($newlevel < $level)
            for ($i = 0; $i < $level - $newlevel; $i++)
                $list .= "</ul>";
        $level = $newlevel;
    }
    $list .= "</ul>";
    return $list.$text;
}


function Colorize($code, $nl2br=True)
{
    $del = "[^\w\d_]";  # Delimiter regexp to use for keyword matching.

    $patterns = array(
        pattern('\t', 'S'),
        pattern(' ', 'S'),
        pattern('((&nbsp;)*)&nbsp;', 'S'),
        pattern('&mdash;', 'S'), # hack
        pattern('((?: [~`%^*()-+={}|\[\]:,.\/] | &lt; | &gt; | &amp;)+)', 'x'),
        pattern('([^\w#])(\d+)'),
        pattern('(^|[^&])(#.*$)', 'Um'),
        pattern('((((&\#039;){3}|(&quot;){3})(.|\n)*\3)|'. # multi-line strings
                '((&\#039;|&quot;).*(\8|\n)))', 'US'), # single line strings
        pattern("(^|$del)(".
            'and | as | assert | break | continue | del | elif | Ellipsis |'.
            'else | except | exec | finally | False | for | from | global |'.
            'if | import | in| is| lambda | not | None | NotImplemented |'.
            'or | pass| print | raise | return | True | try | while | yield'.
            ")(\b|$del)", 'USx'
        ),
        pattern("(^|$del)(class|def)(\s+)(.+)(\s*[<])", 'US'),
        pattern("(^|$del)(".
            'abs | apply | basestring | bool | buffer | callable | chr |'.
            'classmethod | cmp | coerce | compile | complex | copyright |'.
            'credits | delattr | dict | dir | divmod | enumerate | eval |'.
            'execfile | exit | file | filter | float | getattr | globals |'.
            'hasattr | hash | help | hex | id | input | int | intern |'.
            'isinstance | issubclass | iter | len | license | list | locals |'.
            'long | map | max | min | object | oct | open | ord | pow |'.
            'property | quit | range | raw_input | reduce | reload | repr |'.
            'round | setattr | slice | staticmethod | str | sum | super |'.
            'tuple | type | unichr | unicode | vars | xrange | zip | self'.
            ")($del)", 'USx'
        ),
        pattern("(^|$del)(".
            'ArithmeticError | AssertionError | AttributeError |'.
            'DeprecationWarning | EOFError | EnvironmentError |'.
            'Exception | FloatingPointError | FutureWarning | IOError |'.
            'ImportError | IndentationError | IndexError | KeyError |'.
            'KeyboardInterrupt | LookupError | MemoryError | NameError |'.
            'NotImplementedError | OSError | OverflowError |'.
            'OverflowWarning | PendingDeprecationWarning | ReferenceError |'.
            'RuntimeError | RuntimeWarning | StandardError | StopIteration |'.
            'SyntaxError | SyntaxWarning | SystemError | SystemExit |'.
            'TabError | TypeError | UnboundLocalError|UnicodeDecodeError |'.
            'UnicodeEncodeError | UnicodeError | UnicodeTranslateError |'.
            'UserWarning | ValueError | Warning | WindowsError |'.
            'ZeroDivisionError'.
            ")($del)", 'USx'
        ),
    );

    $replace = array(
        '    ',
        '&nbsp;',
        '$1 ',
        '--', #ugh hack
        '<span class="operator">$1</span>',
        '$1<span class="number">$2</span>',
        '$1<span class="comment">$2</span>',
        '<span class="string">$1</span>',
        '$1<span class="reserved">$2</span>$3',
        '$1<span class="reserved">$2</span>$3<span class="definition">$4</span>$5',
        '$1<span class="stdlib">$2</span>$3',
        '$1<span class="exception">$2</span>$3'
    );

    $code = preg_replace($patterns, $replace, $code);
    
    if ($nl2br)
        $code = nl2br($code);
    
    return $code;
}


function HandleCodeTag($text, $nl2br)
{
    $tagsize = strlen("<code>");

    $startpos = strpos($text, "<code>");
    if ($startpos === False)
        return $text;

    $endpos = strpos($text, "</code>");
    if ($endpos === False)
        $endpos = strlen($text);

    if ($endpos < $startpos)
        FatalError("Mismatched code tags make ikagames.com explode.");

    $before = substr($text, 0, $startpos);
    $code = substr($text, $startpos + $tagsize, $endpos - $startpos - $tagsize);
    $after = substr($text, $endpos + $tagsize + 1);


    $code = Colorize($code, $nl2br);

    return "$before<code>$code</code>".HandleCodeTag($after, $nl2br);
}


function NukeHTML($text, $level=LVL_NO_HTML)
{
    # If the string starts with <verbatim>, then we don't add <br /> tags in place of newlines.

    if (strtolower(substr($text, 0, 10)) == "<verbatim>")
    {
        $text = substr($text, 10);
        $convertNewlines = False;
        $toc = True;
    }
    else
        $convertNewlines = True;

    $text = str_replace("\\", "", $text);
    $text = htmlspecialchars($text, ENT_QUOTES);

    if ($level < LVL_BASIC_HTML)
        return $text;

    $patterns = array(
        pattern('--', UxS),
        pattern(tag('(em | i)') .'(.+)'.endtag('\1'), 'USix'),
        pattern(tag('(strong | b)').'(.+)'.endtag('\1'), 'USix'),
        pattern(tag('(u)').'(.+)'.endtag('\1'), 'USix'),
        pattern(tag('(abbr | acronym) \s+'.attr('title', 2)).'(.+)'.endtag('\1'), 'USix'),
        pattern(tag('(abbr | acronym)').'(.+)'.endtag('\1'), 'USix'),
        pattern(tag('(\/?) (blockcode | code)'), 'USix'),
        pattern(tag('(hr | br) (\s*\/)?'), 'USix'),
        pattern(tag('(address | cite | dfn | kbd | samp | sub | sup | var)').'((.|\n)+)'.endtag('\1'), 'USix'),
        pattern(tag('(blockquote | p | pre)').'((.|\n)+)'.endtag('\1'), 'USix'),
        pattern(tag('img \s+'.attr('src')  .'\s+'. attr('alt').'(\s*\/)?'), 'Six'),
        pattern(tag('img \s+'.attr('src')  .'(\s*\/)?'), 'USix'),
        pattern(tag('a   \s+'.attr('name')).'(.+)'.endtag('a'), 'USix'),
        pattern(tag('a   \s+'.attr('href')).'(.+)'.endtag('a'), 'USix'),
        pattern(tag('a   \s+'.attr('id'))  .'(.+)'.endtag('a'), 'USix')
    );

    $replace = array(
        '&mdash;',
        '<em>$2</em>',
        '<strong>\2</strong>',
        '<u>$2</u>',
        '<abbr title="$3">$4</abbr>',
        '<abbr>$2</abbr>',
        '<$1code>',
        '<$1/>',
        '<$1>$2</$1>',
        '<$1>$2</$1>',
        '<img src="$2" alt="$3"/>',
        '<img src="$2"/>',
        '<a name="$2">$3</a>',
        '<a href="$2">$3</a>',
        '<a id="$2">$3</a>'
    );

    $text = preg_replace($patterns, $replace, $text);

    # HandleCodeTag has to do the newline conversion if we're not going to.
    # (code should always be <br />'d)
    $text = HandleCodeTag($text, !$convertNewlines);

    if ($level < LVL_COMPLEX_HTML)
    {
        if ($convertNewlines)
            $text = nl2br($text);
        return $text;
    }

    $patterns = array(
        pattern(tag('(\/? (caption | col | colgroup | dd | dl | dt | li | ol'.
                     '| table | tbody | td | th | thead | tfoot | tr | ul))')
                .newlines(), 'Six'),
        pattern(tag('h1(.*?)').'(.+)'.endtag('h1').newlines(), 'Six'),
        pattern(tag('h2(.*?)').'(.+)'.endtag('h2').newlines(), 'Six'),
        pattern(tag('h3(.*?)').'(.+)'.endtag('h3').newlines(), 'Six'),
        pattern(tag('h4(.*?)').'(.+)'.endtag('h4').newlines(), 'Six')
    );

    $replace = array(
        '<$1>',
        '<h3$1>$2</h3>',
        '<h4$1>$2</h4>',
        '<h5$1>$2</h5>',
        '<h6$1>$2</h6>'
    );

    $text = preg_replace($patterns, $replace, $text);

    $patterns = array(
        pattern('<h(\d)\s(.+)=&quot;(.+)&quot;>(.+)<\\/h\1>', 'Six')
    );

    $replace = array(
        '<h$1 $2="$3">$4</h$1>'
    );
    
    $text = preg_replace($patterns, $replace, $text);

    //if ($toc)
        //$text = MakeToC($text);

    if ($convertNewlines)
        $text = nl2br($text);

    return $text;
}

?>