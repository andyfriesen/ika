# Tokenized file reader class
# Coded by Andy Friesen
# Copyright whenever.  All rights reserved.
#
# This source code may be used for any purpose, provided that
# the original author is never misrepresented in any way.
#
# There is no warranty, express or implied on the functionality, or
# suitability of this code for any purpose.

class TokenStream(object):
    def __init__(self, file):
        if isinstance(file, str):
            f = open(file, 'rt')
        else:
            f = file

        self.text = f.read()
        self.curpos = 0
        self.delimiters = ' \t\n\r\0'
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
        while not self.EOF():
            c = self.text[self.curpos]

            # comment skipper
            if c == '#':
                while not self.EOF() and self.text[self.curpos] != '\n':
                    self.curpos += 1
                continue

            if not c in self.delimiters:
                return

            self.curpos += 1

    def Next(self):
        self.WhiteSpace()

        s = ''
        while not self.EOF():
            c = self.GetChar()

            if c in self.delimiters:
                break

            s += c

        return s

    def GetLine(self):
        self.WhiteSpace()

        s = ''
        while 1:
            c = self.GetChar()

            if c == '\n' or c == '':
                return s

            s = s + c


