# Text Wrappers
# Coded by Kevin Gadd
# Copyright Teh Future(tm).  All rights reserved.
#
# This source code may be used for any purpose, provided that
# the original author is never misrepresented in any way.
#
# There is no warranty, express or implied on the functionality, or
# suitability of this code for any purpose.

def characterWrap(text, maxWidth, font):
    result = []
    buffer = []
    pos = 0
    max = len(text)
    current = ''
    runningWidth = 0
    runningStart = 0
    inColor = 0

    while pos < max:
        # scan through the string
        current = text[pos]
        buffer.append(current)

        if inColor:
            if current == ']':
                # end of a color
                inColor = 0
                # skip the ]
        else:
            if current == '\n':
                # line break
                # spit out everything except the line break
                result.append(''.join(buffer[:-1]))
                buffer = []
                pos += 1
                continue

            if current == '~':
                # subset
                # I think this is broken but I'm not sure...
                buffer.append(text[pos + 1])
                # skip the ~ and the digit
                pos += 2
                continue

            if current == '#':
                if text[pos + 1] == '[':
                    # beginning of a color
                    buffer.append('[')
                    inColor = 1
                    # skip the # and the [
                    pos += 2
                    continue

            # perform wrapping calculations here
            runningWidth = font.StringWidth(''.join(buffer))
            if (runningWidth >= maxWidth):
                # too large, so spit out everything except the last character of buffer
                result.append(''.join(buffer[:-1]))
                buffer = list(buffer[len(buffer)-1])

        pos += 1

    if len(buffer) > 0:
        # spit out the leftovers
        result.append(''.join(buffer))

    return result

def wordWrap(text, maxWidth, font):
    result = []
    pos = 0
    lastSpace = 0

    while len(text) > 0:
        # find a space, tab, or newline.  whichever comes first.
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

        # Python slices are [low, high), so we must compensate by adding 1.
        l = font.StringWidth(text[:pos + 1])

        if l >= maxWidth:        # too wide.  Go back to the last whitespace character, and chop
            if lastSpace > 0:
                result.append(text[:lastSpace])
                text = text[lastSpace + 1:]
                pos = 0
                lastSpace = 0
            else:                       # no last space!  Hack right here, since the word is obviously too goddamn long.
                result.append(text[:pos + 1])
                text = text[pos + 1:].lstrip()
                pos = 0
                lastSpace = 0

            continue

        pos += 1

    return result
