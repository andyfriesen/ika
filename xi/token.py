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
    def __init__(self, filename):
        f = open(filename, 'rt')
        self.text = f.read()
        self.curpos = 0
        self.delimiters = ' \t\n\0'
        f.close()

    def EOF(self):
        if self.curpos < len(self.text):
            return 0
        else:
            return 1

    def GetChar(self):
        if self.curpos >= len(self.text):
            return 0

        c = self.text[self.curpos]
        self.curpos += 1
        return c

    def WhiteSpace(self):
        while 1:
            c = self.text[self.curpos]
            if not c in self.delimiters:
                return

            self.curpos += 1

    def Next(self):
        self.WhiteSpace()

        s = ''
        while 1:
            c = self.GetChar()

            if c in self.delimiters:
                return s

            s = s + c

    def GetLine(self):
        self.WhiteSpace()

        s = ''
        while 1:
            c = self.GetChar()

            if c == '\n' or c == '':
                return s

            s = s + c


