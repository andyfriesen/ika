# Generic menu interface
# coded by Andy Friesen
# copyright whenever.  All rights reserved.
#
# This source code may be used for any purpose, provided that
# the original author is never misrepresented in any way.
#
# There is no warranty, express or implied on the functionality, or
# suitability of this code for any purpose.

from __future__ import generators

import ika
import widget
import cursor

from misc import *

#------------------------------------------------------------------------------

defaultcursor = cursor.Cursor(widget.defaultfont)

# Unique object returned when a menu was cancelled.
Cancel = object()
# Key repeat delay
DELAY = 50
MOVE_DELAY = 10

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
    
    def __init__(_, x = 0 , y = 0, cursor = None, textcontrol = None):
        global defaultcursor

        widget.Frame.__init__(_)
        _.menuitems = textcontrol or widget.TextLabel()

        _.widgets.append(_.menuitems)
        _.cursor = cursor or defaultcursor
        _.cursorwidth = _.cursor.Width * 2 / 3
        _.menuitems.x = _.cursorwidth

        _.Position = (x, y)
        _.Size = _.menuitems.Size
        _.width += _.cursorwidth
        _.cursorcount = DELAY

        _.pagesize = _.height / _.menuitems.font.height    # The number of menu items that fit in the window at one time

        _.ypos = 0                                         # The position of the cursor, on the menu
        _.active = True

        #_.Update = _.__Update().next

    def set_YPage(_, value):
        _.menuitems.YPage = value

    def set_YMax(_, value):
        _.menuitems.YMax = value

    def set_CursorPos(_, value):
        if value - _.YPage < _.pagesize:
            _.ypos = value - _.YPage
        else:
            _.YPage = value
            _.ypos = 0

    CursorPos = property(lambda _: _.ypos + _.YPage, set_CursorPos)
    YPage = property(lambda _: _.menuitems.YPage, set_YPage)
    YMax = property(lambda _: _.menuitems.YMax, set_YMax)
    Font = property(lambda _: _.menuitems.font)
    Text = property(lambda _: _.menuitems)

    def Draw(_):
        widget.Frame.Draw(_)

        if _.active:
            _.cursor.Draw(_.x + _.cursorwidth, _.y + (_.ypos + 0.5) * _.menuitems.font.height)

    def Clear(_):
        _.menuitems.Clear()

    def AddText(_,*args):
        _.menuitems.AddText(*args)
        _.AutoSize()

    def AutoSize(_):
        _.menuitems.AutoSize()
        widget.Frame.AutoSize(_)
        _.pagesize = _.height / _.menuitems.font.height

    def Update(_):
        def MoveUp():
            if _.ypos > 0:      _.ypos -= 1
            elif _.YPage > 0:   _.YPage -= 1

        def MoveDown():
            if _.ypos < _.pagesize - 1:
                _.ypos += 1
            elif _.YPage < _.menuitems.Length - _.pagesize:
                _.YPage += 1

        #--
                
        ika.Input.Update()

        if ika.Input.up.Position():
            if _.cursorcount == 0 or _.cursorcount == DELAY:
                MoveUp()

        elif ika.Input.down.Position():
            if _.cursorcount == 0 or _.cursorcount == DELAY:
                MoveDown()

        elif enter():
            return _.CursorPos

        elif cancel():
            return Cancel

        else:
            _.cursorcount = DELAY
            return None

        # if not ika.Input.up.Position() and not ika.Input.down.Position():
        # not needed because the else clause returns.
        _.cursorcount -= 1
        if _.cursorcount < 0:
            _.cursorcount = MOVE_DELAY

        return None            

    def Execute(_):
        _.ypos = _.YPage = 0
        t = ika.GetTime()

        while 1:
            time = ika.GetTime()
            delta = time - t
            t = time

            while delta > 0:            
                result = _.Update()
                if result is not None:
                    return result

            map.Render()
                
            _.Draw()
            ika.Video.ShowPage()