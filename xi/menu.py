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

def GetDefaultCursor():
    return cursor.Cursor(widget.defaultfont)

# Unique object returned when a menu was cancelled.
Cancel = object()
# Key repeat delay
DELAY = 50
MOVE_DELAY = 5

def SetDefaultCursor(csr):
    global defaultcursor
    
    defaultcursor = csr

class Menu(widget.Frame):
    "A menu window.  Has a list of items that the user can select."

    __slots__ = widget.Frame.__slots__ + [
        'menuitems',    # The widget that holds the individual menu elements
        'widgets',      # Inherited from widget.Frame.  List of child widgets.
        'cursor',       # Image to draw as a cursor
        'cursorwidth',  # The amount of space to make for the cursor at the left.  Is just a bit smaller than the cursor itself for asthetic reasons.
        'width',        # The width of the cursor?
        'pagesize',     # The number of menu items that fit on a single page.
        'ypos',         # Current Y position of the cursor relative to the window.
        'active',       # If true, the cursor is drawn.
        'cursorcount',  # lil count variable for cursor repeating.
        #'Update',       # Update coroutine
        ]
    
    def __init__(self, x = 0 , y = 0, cursor = None, textcontrol = None):
        
        widget.Frame.__init__(self)
        self.menuitems = textcontrol or widget.TextLabel()

        self.widgets.append(self.menuitems)
        self.cursor = cursor or GetDefaultCursor()
        self.cursorwidth = self.cursor.Width * 2 / 3
        self.menuitems.x = self.cursorwidth

        self.Position = (x, y)
        self.Size = self.menuitems.Size
        self.width += self.cursorwidth
        self.cursorcount = DELAY

        self.pagesize = self.height / self.menuitems.font.height    # The number of menu items that fit in the window at one time

        self.ypos = 0                                         # The position of the cursor, on the menu
        self.active = True

        #self.Update = self.__Update().next

    def set_YPage(self, value):
        self.menuitems.YPage = value

    def set_YMax(self, value):
        self.menuitems.YMax = value

    def set_CursorPos(self, value):
        if value - self.YPage < self.pagesize:
            self.ypos = value - self.YPage
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
            self.cursor.Draw(self.x + self.cursorwidth, self.y + (self.ypos + 0.5) * self.menuitems.font.height)

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
        def MoveUp():
            if self.ypos > 0:      self.ypos -= 1
            elif self.YPage > 0:   self.YPage -= 1

        def MoveDown():
            if self.CursorPos == self.menuitems.Length - 1:
                return
            
            if self.ypos < self.pagesize - 1:
                self.ypos += 1
            elif self.YPage < self.menuitems.Length - self.pagesize:
                self.YPage += 1

        #--
                
        ika.Input.Update()

        if ika.Input.up.Position():
            if self.cursorcount == 0 or self.cursorcount == DELAY:
                MoveUp()
            self.cursorcount -= 1
            if self.cursorcount < 0:
                self.cursorcount = MOVE_DELAY


        elif ika.Input.down.Position():
            if self.cursorcount == 0 or self.cursorcount == DELAY:
                MoveDown()
            self.cursorcount -= 1
            if self.cursorcount < 0:
                self.cursorcount = MOVE_DELAY

        else:
            self.cursorcount = DELAY

        if enter():
            return self.CursorPos

        elif cancel():
            return Cancel

        return None            

    def Execute(self):
        self.ypos = self.YPage = 0
        t = ika.GetTime()

        while 1:
            time = ika.GetTime()
            delta = time - t
            t = time

            while delta > 0:            
                result = self.Update()
                if result is not None:
                    return result

            map.Render()
                
            self.Draw()
            ika.Video.ShowPage()
