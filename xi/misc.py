#  odds and ends
# coded by Andy Friesen
# copyright whenever.  All rights reserved.
#
# This source code may be used for any purpose, provided that
# the original author is never misrepresented in any way.
#
# There is no warranty, express or implied on the functionality, or
# suitability of this code for any purpose.

# wraps the text to the given pixel width, using the font specified.
# returns a list of strings

def WrapText(text, maxWidth, font):
    result = []
    pos = 0
    lastSpace = 0

    while len(text) > 0:
        # find a space, tab, or return.  whichever comes first.
        # if the word can be appended to the current line, append it.
        # if not, and the current line is not empty, put it on a new line.
        # if the word is longer than a single line, hack it wherever, and make the hunk its own line.

        # find the next space, tab, or newline.
        if pos >= len(text):    # hit the end of the string?
            result.append(text) # we're done.  add the last of it to the list
            break               # and break out

        if text[pos].isspace():
            lastSpace = pos

        if text[pos] == '\n':      # newline.  Chop.
            result.append(text[:pos])
            text = text[pos + 1:]
            pos = 0
            lastSpace = 0
            continue

        l = font.StringWidth(text[:pos])

        if l >= maxWidth:        # too wide.  Go back to the last whitespace character, and chop
            if lastSpace > 0:
                result.append(text[:lastSpace])
                text = text[lastSpace + 1:]
                pos = 0
                lastSpace = 0
            else:                       # no last space!  Hack right here, since the word is obviously too goddamn long.
                result.append(text[:pos])
                text = text[pos + 1:]

            continue

        pos += 1

    return result

def clamp(value, lower, upper):
    return max(min(value, upper), lower)

import ika

a = lambda n: ika.Input[n].Pressed
up = a('UP')
down = a('DOWN')
left = a('LEFT')
right = a('RIGHT')
enter = a('RETURN')
cancel = a('ESCAPE')
del a
