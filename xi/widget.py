# Base UI layer
# coded by Andy Friesen
# copyright whenever.  All rights reserved.
#
# This source code may be used for any purpose, provided that
# the original author is never misrepresented in any way.
#
# There is no warranty, express or implied on the functionality, or
# suitability of this code for any purpose.

# Frames hold an arbitrary number of widgets. (and are widgets themselves)
# Widgets are, currently, either text labels, or images.  Maybe more later.

import ika
import window

# globals (hurk)

defaultwindow = window.Window()

#defaultwindow.Load('window.png', 8)
defaultwindow.Load('window10.png', 4)
defaultfont = ika.Font('arial.fnt')
#defaultfont = Font('eb.fnt')
defaultfont.tabsize = 64

class Widget(object):
    "Basic widget interface."
    def __init__(self, x = 0, y = 0, width = 0, height = 0):
        self.x = x
        self.y = y
        self.width = width
        self.height = height
        self.border = 0

    def Draw(self, xoffset = 0, yoffset = 0):
        pass

    def DockLeft(self, wnd = None):
        self.x = self.border
        if wnd is not None:
            self.x += wnd.Right
        return self
    
    def DockRight(self, wnd = None):
        y = wnd and wnd.Right or ika.GetScreenImage().width
        self.x = y - self.width - self.border
        return self
    
    def DockTop(self, wnd = None):
        self.y = self.border
        if wnd is not None:
            self.y += wnd.Bottom
        return self
    
    def DockBottom(self, wnd = None):
        y = wnd and wnd.Bottom or ika.GetScreenImage().height
        self.y = y - self.height - self.border
        return self

    def get_Position(self):        return self.x, self.y
    def set_Position(self, pos):   (self.x, self.y) = pos

    def get_Size(self):            return self.width, self.height
    def set_Size(self, value):     (self.width, self.height) = value

    Position = property( lambda self: (self.width, self.height), set_Position, doc = 'Gets or sets the position')
    Size     = property(get_Size, set_Size, doc = 'Gets or sets the size (in pixels)')

    Left     = property(lambda self: self.x)
    Top      = property(lambda self: self.y)
    Right    = property(lambda self: self.x + self.width)
    Bottom   = property(lambda self: self.y + self.height)

class Frame(Widget):
    "Base frame class.  A window, with things in it."
    def __init__(self, wnd = defaultwindow, x = 0, y = 0, width = 0, height = 0):
        Widget.__init__(self, x, y, width, height)
        
        self.wnd = wnd
        self.widgets = []
        self.border = int(1.5 * self.wnd.Left)     # or right or whatever

    def Draw(self, xoffset = 0, yoffset = 0):
        self.wnd.Draw(self.x, self.y, self.x+self.width, self.y+self.height)
        for x in self.widgets:
            x.Draw(self.x, self.y)

    def AutoSize(self):                 # makes the frame big enough to hold its contents
        self.width = 0
        self.height = 0

        for w in self.widgets:
            self.width = max(self.width, w.x+w.width)
            self.height = max(self.height, w.y+w.height)

class TextFrame(Frame):
    "A frame with a simple text widget.  Nothing else."
    def __init__(self, wnd = defaultwindow, x = 0, y = 0, width = 0, height = 0):
        Frame.__init__(self, wnd, x, y, width, height)
        self.__text = TextLabel()
        self.widgets.append(self.__text)
        self.text = self.__text.text

    def Clear(self):
        self.__text.Clear()
        self.__text.width, self.__text.height = 0, 0

    def AddText(self, args):
        self.__text.AddText(args)
        self.Size = self.__text.Size
        
    def AutoSize(self):
        self.__text.AutoSize()
        self.Size = self.__text.Size
    

class TextLabel(Widget):
    'Textlabels hold one or more lines of text.'

    def __init__(self, font = defaultfont, *text):
        Widget.__init__(self)

        self.font = font
        self.ypos = 0
        self.maxy = 0

        self.text = list(text)

        self.LeftJustify()

    def LeftJustify(self):
        self.PrintString = self.font.Print

    def RightJustify(self):
        self.PrintString = self.font.RightPrint

    def Center(self):
        self.PrintString = self.font.CenterPrint

    def Clear(self):
        self.text = []
        self.width = 0
        self.height = 0

    def SetText(self, *text):
        self.Clear()
        self.AddText(text)

    def AddText(self, text):
        for x in text.split('\n'):
            self.text.append(x)
            self.width = max(self.width, self.font.StringWidth(x))
        self.AutoSize()

    def Draw(self, xoffset = 0, yoffset = 0):
        curx = 0
        cury = 0

        for t in self.text:
            if cury >= self.height:
                break
            self.PrintString(self.x + xoffset + curx, self.y + yoffset + cury, t)
            cury+=self.font.height

    def AutoSize(self):
        self.Size = (0, 0)
        
        for t in self.text:
            self.width = max(self.width, self.font.StringWidth(t))

        self.height = len(self.text) * self.font.height
        
        if self.maxy != 0:
            maxy = self.maxy
        else:
            maxy = ika.GetScreenImage().height - self.y

        self.height = min(self.height, maxy)

    def set_YPos(self, val):
        self.ypos = min(val, maxy - self.height / self.font.height)

    YPos = property( lambda self: self.ypos, set_YPos )

class ColumnedTextLabel(Widget):
    'A text label that holds one or more columns of text.  Columns stay lined up.'

    def __init__(self, x = 0, y = 0, columns = 1, font = defaultfont):
        Widget.__init__(self, x, y)

        self.font = font

        self.columns = []
        for i in range(columns):
            self.columns.append(TextLabel(font))

    def LeftJustify(self):
        for c in self.columns:
            c.LeftJustify()

    def RightJustify(self):
        for c in self.columns:
            c.RightJustify()

    def Center(self):
        for c in self.columns:
            c.Center()

    def Clear(self):
        for c in self.columns:
            c.Clear()

    def AddText(self,*args):
        for i in range(min(len(args), len(self.columns))):
            self.columns[i].AddText(args[i])

    def Draw(self, xoffset = 0, yoffset = 0):
        for c in self.columns:
            c.Draw(self.x + xoffset, self.y + yoffset)

    def AutoSize(self):
        self.columns[0].AutoSize()
        self.columns[0].Position = (0, 0)
        for i in range(1, len(self.columns)):
            self.columns[i].AutoSize()
            self.columns[i].DockLeft(self.columns[i - 1])

        self.Size = (0, 0)
        for c in self.columns:
            self.width = max(self.width, c.Right)
            self.height = max(self.height, c.Bottom)

    def set_YPos(self,val):
        for c in self.columns:
            c.YPos = value

    YPos = property( lambda self: self.columns[0].YPos, set_YPos )
    

class Bitmap(Widget):
    "Bitmap widgets are images."
    def __init__(self, img = None):
        Widget.__init__(self)

        self.img = img
        self.x = 0
        self.y = 0
        if img is not None:
            self.Size = img.width, img.height

    def set_Image(self, val):
        self.img = val
        if val is not None:
            self.Size = val.width, val.height

    Image = property(lambda self: self.img, set_Image)

    def Draw(self, xoffset = 0, yoffset = 0):
        # TODO: allow the widget to be resized, thus scaling the image?
        if self.img is not None:
            self.img.Blit(self.x+xoffset, self.y+yoffset)
