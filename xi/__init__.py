#!/usr/bin/env python

__all__ = ['char', 'color', 'controls', 'cursor', 'datfile', 'decorator',
           'effects', 'equipmenu', 'exception', 'field', 'fps', 'gui', 'item',
           'itemdatabase', 'itemmenu', 'layout', 'mainmenu', 'menu',
           'menuwindows', 'movescript', 'music', 'parser', 'party',
           'proxy', 'skill', 'skilldatabase', 'skillmenu', 'sound',
           'statelessproxy', 'statset', 'statusmenu', 'test', 'textbox',
           'token', 'transition', 'utilmenu', 'widget', 'widgetmanager',
           'window']


def readonly(fget, doc=""):
    return property(fget, doc=doc)


def wrapText(text, maxWidth, font):
    """Wraps the text to the given pixel width using the font specified.

    Returns a list of strings.
    """
    result = []
    pos = 0
    lastSpace = 0
    while len(text) > 0:
        # find a space, tab, or newline.  whichever comes first.
        # if the word can be appended to the current line, append it.
        # if not, and the current line is not empty, put it on a new line.
        # if the word is longer than a single line, hack it wherever, and make the hunk its own line.
        # find the next space, tab, or newline.
        if pos >= len(text):
            # hit the end of the string?
            result.append(text)
            # we're done.  add the last of it to the list
            break
        if text[pos].isspace():
            lastSpace = pos
        if text[pos] == '\n':
            # newline.  Chop.
            result.append(text[:pos])
            text = text[pos + 1:]
            pos = 0
            lastSpace = 0
            continue
        # Python slices are [low, high), so we must compensate by adding 1.
        l = font.StringWidth(text[:pos + 1])
        if l >= maxWidth:
            # too wide.  Go back to the last whitespace character, and chop
            if lastSpace > 0:
                result.append(text[:lastSpace])
                text = text[lastSpace + 1:]
                pos = 0
                lastSpace = 0
            else:
                # no last space!  Hack right here, since the word is obviously too goddamn long.
                result.append(text[:pos + 1])
                text = text[pos + 1:].lstrip()
                pos = 0
                lastSpace = 0
            continue
        pos += 1

    return result


def clamp(value, lower, upper):
    """Returns the value, unless it is outside the min and max allowed
    values, in which case it returns whichever boundary the value
    crossed.
    
    (min if value is to low, max if value is too high)
    """
    return max(min(value, upper), lower)


def formatTime(time, separator = ':'):
    hours = time / 360000
    minutes = time / 6000 % 60
    seconds = time / 100 % 60
    return separator.join(['%02i' % i for i in (hours, minutes, seconds)])
