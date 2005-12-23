# Skill menu for xi
# Coded by Andy Friesen
# Copyright whenever.  All rights reserved.
#
# This source code may be used for any purpose, provided that
# the original author is never misrepresented in any way.
#
# There is no warranty, express or implied on the functionality, or
# suitability of this code for any purpose.

import ika

import xi.party
import statusmenu
from xi.menuwindows import SkillWindow, PortraitWindow, StatusWindow

from xi import gui
from xi import controls
from xi.fps import FPSManager
from xi.menu import Menu, Cancel
from xi.cursor import NullCursor
from xi.misc import *

_nullCursor = NullCursor(1, 1, (1, 1))

class SkillMenu(object):
    def __init__(self):
        self.skillWindow = SkillWindow()
        self.menu = gui.FrameDecorator(Menu(textctrl=self.skillWindow))
        self.portraitWindow = PortraitWindow()
        self.statWindow = gui.FrameDecorator(StatusWindow())
        self.charidx = 0

        self.description = gui.FrameDecorator(gui.StaticText(text=['','']))

    curChar = property(lambda self: xi.party.activeRoster[self.charidx])

    def setDescription(self, desc):
        # wordwrap, take the first two lines (that's all we have room for) and join with a newline
        t = '\n'.join(wrapText(desc, self.description.client.width, self.description.font)[:2])
        self.description.text[0] = t

    def startShow(self, trans):
        self.refresh(self.curChar)

        trans.addChild(self.portraitWindow, startRect=(-self.portraitWindow.width, self.portraitWindow.y))
        trans.addChild(self.statWindow, startRect=(ika.Video.xres, self.statWindow.y))
        trans.addChild(self.description, startRect=(self.description.x, -self.description.height))
        trans.addChild(self.menu, startRect=(self.menu.x, ika.Video.yres))

    def startHide(self, trans):
        trans.addChild(self.portraitWindow, endRect=(ika.Video.xres, self.portraitWindow.y))
        trans.addChild(self.statWindow, endRect=(-self.statWindow.width, self.statWindow.y))
        trans.addChild(self.description, endRect=(self.description.x, -self.description.height))
        trans.addChild(self.menu, endRect=(self.menu.x, ika.Video.yres))

    def refresh(self, char):
        self.portraitWindow.refresh(char)
        self.statWindow.refresh(char)
        self.skillWindow.refresh(char,
            lambda skill: (skill.fieldEffect is not None) and (skill.mp <= char.mp)
            )

        if len(char.skills):
            self.menu.cursor = gui.default_cursor
        else:
            self.menu.cursor = _nullCursor

        self.menu.cursorPos = min(self.menu.cursorPos, len(char.skills))

        # Layout
        self.portraitWindow.dockTop().dockLeft()
        self.statWindow.dockTop(self.portraitWindow).dockLeft()
        self.statWindow.width = max(self.statWindow.width, self.portraitWindow.width)
        self.portraitWindow.width = self.statWindow.width
        self.description.autoSize()
        self.description.dockTop().dockLeft(self.portraitWindow)
        self.description.width = ika.Video.xres - self.description.x - self.description.border
        self.menu.dockTop(self.description).dockLeft(self.portraitWindow)
        self.menu.width = self.description.width

        try:
            s = char.skills[self.menu.cursorPos]
            self.setDescription(s.desc)
        except IndexError:
            self.setDescription('')

    def update(self):
        if controls.left() and self.charidx > 0:
            self.charidx -= 1
            self.refresh(xi.party.activeRoster[self.charidx])

        if controls.right() and self.charidx < len(xi.party.activeRoster) - 1:
            self.charidx += 1
            self.refresh(xi.party.activeRoster[self.charidx])

        char = self.curChar

        if self.menu.cursorPos < len(char.skills):
            s = char.skills[self.menu.cursorPos]
            self.setDescription(s.desc)
        else:
            self.setDescription('')

        result = self.menu.update()
        return result

    def castSpell(self, char, skill):
        try:
            # see if the effect wants us for context
            result = skill.fieldEffect(self.curChar, self)
        except TypeError:
            # if not, that's fine too
            result = skill.fieldEffect(self.curChar)

        # result will be None unless it was cancelled.
        if result is None:
            self.curChar.mp -= skill.mp
            self.refresh(self.curChar)

    def draw(self):
        ika.Map.Render()
        self.portraitWindow.draw()
        self.statWindow.draw()
        self.description.draw()
        self.menu.draw()

    def execute(self):
        fps = FPSManager()
        result = None

        self.charidx = 0

        self.refresh(self.curChar)

        while result is not Cancel:
            result = self.update()

            if result not in (None, Cancel) and 0 <= result < len(self.curChar.skills):
                skill = self.curChar.skills[result]

                if skill.fieldEffect is not None and self.curChar.mp >= skill.mp:
                    self.castSpell(self.curChar, self.curChar.skills[result])
                    fps.sync()
                else:
                    # Play a buzzing sound or somesuch.
                    pass

            fps.render(self.draw)

        return True
