# Tokenized file reader class
# coded by Andy Friesen
# copyright whenever.  All rights reserved.
#
# This source code may be used for any purpose, provided that
# the original author is never misrepresented in any way.
#
# There is no warranty, express or implied on the functionality, or
# suitability of this code for any purpose.

class TokenStream:
    def __init__(_, filename):
        f = open(filename, 'rt')
        _.text = f.read()
        _.curpos = 0
        _.delimiters = ' \t\n\0'
        f.close()

    def EOF(_):
        if _.curpos < len(_.text):
            return 0
        else:
            return 1

    def GetChar(_):
        if _.curpos >= len(_.text):
            return 0

        c = _.text[_.curpos]
        _.curpos += 1
        return c

    def WhiteSpace(_):
        while 1:
            c = _.text[_.curpos]

            # comment skipper
            if c == '#':
                while _.text[_.curpos] != '\n' and not _.EOF():
                    _.curpos += 1
                continue
                    
            if not c in _.delimiters:
                return

            _.curpos += 1

    def Next(_):
        _.WhiteSpace()

        s = ''
        while 1:
            c = _.GetChar()

            if c in _.delimiters:
                return s

            s = s + c

    def GetLine(_):
        _.WhiteSpace()

        s = ''
        while 1:
            c = _.GetChar()

            if c == '\n' or c == '':
                return s

            s = s + c


