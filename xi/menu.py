# Base menu class
# Coded by Andy Friesen
# Copyright whenever.  All rights reserved.
#
# This source code may be used for any purpose, provided that
# the original author is never misrepresented in any way.
#
# There is no warranty, express or implied on the functionality, or
# suitability of this code for any purpose.

import ika

import gui
import cursor

import sound
import controls

from proxy import Proxy

# unique object returned when the user cancels a menu.
# We use this pretty much the same way as None.  It has
# no attributes.
Cancel = object()

_PAUSE_DELAY= 50
_REPEAT_DELAY = 0

class Menu(gui.Widget):
    '''
    A menu.  A list of textual options displayed in some sort of text
    container, with a cursor that responds to user input, allowing the user to
    select an option.

    I'll readily admit that this is somewhat limiting.  Doing a SoM style ring
    menu with this class is not very realistic, but it could be implemented as
    its own class. (and probably should, considering how different it is from
    this.
    '''
    def __init__(self, *args, **kw):
        gui.Widget.__init__(self, *args)
        self._textCtrl = kw.get('textctrl') or gui.ScrollableTextLabel()
        self._cursor = kw.get('cursor') or gui.default_cursor
        self._cursorWidth = self.cursor.width / 2
        self._textCtrl.position = (self._cursorWidth, 0)
        self._cursorY = 0
        self._cursorPos = 0
        self._cursorSpeed = 2 # speed at which the cursor moves (in pixels per update)

        # cursor repeating stuff.
        self._pauseDelay = kw.get('pausedelay', _PAUSE_DELAY)
        self._repeatDelay = kw.get('repeatdelay', _REPEAT_DELAY)
        self._cursorCount = self._pauseDelay

    cursorY = property(lambda self: self._cursorY)

    def getCursorPos(self):
        return self._cursorPos
    def setCursorPos(self, value):
        value = max(0, value)
        self._cursorPos = min(len(self.text), value)
    cursorPos = property(getCursorPos, setCursorPos)

    def getWidth(self):
        return self._textCtrl.width + self._cursorWidth
    def setWidth(self, value):
        self._textCtrl.width = value - self._cursorWidth
    width = property(getWidth, setWidth)

    def getHeight(self):
        return self._textCtrl.height
    def setHeight(self, value):
        self._textCtrl.height = value
    height = property(getHeight, setHeight)

    def getFont(self):
        return self._textCtrl.font
    font = property(getFont)

    def getCursor(self):
        return self._cursor
    def setCursor(self, value):
        self._cursor = value
    cursor = property(getCursor, setCursor)

    def getText(self):
        return self._textCtrl.text
    text = property(getText)

    def addText(self, *args):
        self._textCtrl.addText(*args)

    def clear(self):
        self._textCtrl.clear()

    def autoSize(self):
        self._textCtrl.autoSize()

    def update(self):
        '''
        Performs one tick of menu input.  This includes scrolling things around,
        and updating the position of the cursor based on user interaction.

        If the user has selected an option, then the return value is the index
        of that option.  If the user hit the cancel (ESC) key, Cancel is
        returned.  Else, None is returned, to signify that nothing has happened
        yet.
        '''
        ika.Input.Update()
        cy = self.cursorY

        # update the cursor
        ymax = max(0, len(self.text) * self.font.height - self._textCtrl.height)


        # Goofy, but kinda cool.
        # We figure out where the cursor should be given its logical position.
        # (ie the item it's supposed to point at)  If it's different from its
        # actual position, we move it.  This way, we get nice smooth cursor
        # movement.

        delta = self.cursorPos * self.font.height - self._textCtrl.ywin - cy
        if delta > 0:
            if cy < self._textCtrl.height - self.font.height:
                self._cursorY += self._cursorSpeed
            else:
                self._textCtrl.ywin += min(delta, self._cursorSpeed)
        elif delta < 0:
            if cy > 0:
                self._cursorY -= self._cursorSpeed
            elif self._textCtrl.ywin > 0:
                self._textCtrl.ywin -= min(-delta, self._cursorSpeed)
        else:

            # Only move the cursor if delta is zero.  That way movement doesn't
            # get bogged down by a cursor that moves too slowly.
            #
            # The cursor delaying stuff mucks this up considerably.  Basically, there's
            # a counter variable (_cursorCount) that is reset when no key is pressed.
            # When a key is pressed, the cursor is moved iff the count is maxed, or at 0.
            # when the counter goes below 0, it is set to the repeat count.
            if controls.up() and self.cursorPos > 0:
                if self._cursorCount == 0 or self._cursorCount == self._pauseDelay:
                    self.cursorPos -= 1
                    sound.cursormove.Play()

                self._cursorCount -= 1

                if self._cursorCount < 0:
                    self._cursorCount = self._repeatDelay

            elif controls.down() and self.cursorPos < len(self.text) - 1:
                if self._cursorCount == 0 or self._cursorCount == self._pauseDelay:
                    self.cursorPos += 1
                    sound.cursormove.Play()

                self._cursorCount -= 1

                if self._cursorCount < 0:
                    self._cursorCount = self._repeatDelay

            elif controls.enter():
                return self.cursorPos
            elif controls.cancel():
                return Cancel
            else:
                self._cursorCount = self._pauseDelay

            # execution reaches this point if enter or cancel is not pressed
            return None

    def draw(self, xoffset = 0, yoffset = 0):
        self._textCtrl.draw(self.x + xoffset, self.y + yoffset)
        self.cursor.draw(
            self.x + self._textCtrl.x + xoffset,
            self.y + self._textCtrl.y + yoffset + self.cursorY +
                (self.font.height / 2)
            )
