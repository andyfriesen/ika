# Generic menu interface
# coded by Andy Friesen
# copyright whenever.  All rights reserved.
#
# This source code may be used for any purpose, provided that
# the original author is never misrepresented in any way.
#
# There is no warranty, express or implied on the functionality, or
# suitability of this code for any purpose.

import ika
import widget
import cursor

from misc import *

#------------------------------------------------------------------------------

CURSOR_WIDTH=20
defaultcursor = cursor.Cursor(widget.defaultfont)

def SetDefaultCursor(csr):
    global defaultcursor
    
    defaultcursor = csr

class Menu(widget.Frame):
    "A menu window.  Has a list of items that the user can select."
    def __init__(self, x = 0 , y = 0, cursor = None, textcontrol = None):
        global defaultcursor

        widget.Frame.__init__(self)
        self.menuitems = textcontrol or widget.TextLabel()

        self.menuitems.x = CURSOR_WIDTH
        self.widgets.append(self.menuitems)
        self.cursor = cursor or defaultcursor

        self.Position = (x, y)
        self.Size = self.menuitems.Size
        self.width += CURSOR_WIDTH

	self.pagesize = self.height / self.menuitems.font.height        	# The number of menu items that fit in the window at one time

        self.ypos = 0								# The position of the cursor, on the menu
        self.active = True

    def set_YPage(self, value):
        self.menuitems.YPage = value

    def set_YMax(self, value):
        self.menuitems.YMax = value

    def set_CursorPos(self, value):
        if value - self.YPage < self.pagesize:
            self.ypos = value
        else:
            self.YPage = value
            self.ypos = 0

    CursorPos = property(lambda self: self.ypos + self.YPage, set_CursorPos)
    YPage = property(lambda self: self.menuitems.YPage, set_YPage)
    YMax = property(lambda self: self.menuitems.YMax, set_YMax)
    Font = property(lambda self: self.menuitems.font)
    Text = property(lambda self: self.menuitems)

    def Draw(self):
        widget.Frame.Draw(self)

        if self.active:
            self.cursor.Draw(self.x + CURSOR_WIDTH, self.y + (self.ypos + 0.5) * self.menuitems.font.height)

    def Clear(self):
        self.menuitems.Clear()

    def AddText(self,*args):
        self.menuitems.AddText(*args)
        self.AutoSize()

    def AutoSize(self):
        self.menuitems.AutoSize()
        widget.Frame.AutoSize(self)
        self.pagesize = self.height / self.menuitems.font.height

    def Update(self):
        ika.Input.Update()
        if up():
            if self.ypos > 0:
                self.ypos -= 1
            elif self.YPage > 0:
                self.YPage -= 1
            
        if down():
            if self.ypos < self.pagesize - 1:
                self.ypos += 1
            elif self.YPage < len(self.menuitems) - self.pagesize:
                self.YPage += 1

        if enter():
            return self.CursorPos

        if cancel():
            return -1

        return None

    def Execute(self):
        self.ypos = self.YPage = 0

        while 1:
            map.Render()

            result = self.Update()
            if result != None:
                return result
                
            self.Draw()
            ika.ShowPage()