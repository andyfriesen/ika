# Status menu for pi
# Coded by Andy Friesen
# Copyright whenever.  All rights reserved.
#
# This source code may be used for any purpose, provided that
# the original author is never misrepresented in any way.
#
# There is no warranty, express or implied on the functionality, or
# suitability of this code for any purpose.

import ika

from xi import gui
from xi import menu
from xi import layout
from xi import controls
from xi.misc import *
from xi.fps import FPSManager

import stats

from common import PortraitWindow, StatusWindow, EquipWindow, SkillWindow

class StatusMenu(object):

    def __init__(self):
        self.portraitWindow = PortraitWindow()
        self.statWindow = gui.FrameDecorator(StatusWindow())
        self.equipWindow = gui.FrameDecorator(EquipWindow())
        self.skillWindow = gui.FrameDecorator(SkillWindow())
        self._curChar = 0

    def startShow(self, trans):
        self.refresh(stats.activeRoster[0])

        trans.addChild(self.portraitWindow, startRect=(-self.portraitWindow.width, self.portraitWindow.y))
        trans.addChild(self.statWindow, startRect=(ika.Video.xres, self.statWindow.y))
        trans.addChild(self.equipWindow, startRect=(self.equipWindow.x, -self.equipWindow.height))
        trans.addChild(self.skillWindow, startRect=(self.skillWindow.x, ika.Video.yres))

    def startHide(self, trans):
        trans.addChild(self.portraitWindow, endRect=(-self.portraitWindow.width, self.portraitWindow.y))
        trans.addChild(self.statWindow, endRect=(ika.Video.xres, self.statWindow.y))
        trans.addChild(self.equipWindow, endRect=(self.equipWindow.x, -self.equipWindow.height))
        trans.addChild(self.skillWindow, endRect=(self.skillWindow.x, ika.Video.yres))

    def refresh(self, curchar):
        self.portraitWindow.refresh(curchar)
        self.statWindow.refresh(curchar)
        self.equipWindow.refresh(curchar)

        self.equipWindow.y = self.portraitWindow.y

        self.portraitWindow.dockTop().dockLeft()
        self.statWindow.dockTop(self.portraitWindow).dockLeft()

        self.statWindow.width = max(self.statWindow.width, self.portraitWindow.width)
        self.portraitWindow.width = self.statWindow.width

        self.equipWindow.dockTop().dockLeft(self.portraitWindow)
        self.skillWindow.dockTop(self.equipWindow).dockLeft(self.statWindow)
        self.equipWindow.width = ika.Video.xres - self.equipWindow.border - self.equipWindow.x
        self.skillWindow.width = self.equipWindow.width
        self.skillWindow.refresh(curchar)
        self.skillWindow.height = min(self.skillWindow.height, ika.Video.yres - self.skillWindow.y - self.skillWindow.border)

    #--------------------------------------------

    def update(self):
        ika.Input.Update()

        if controls.left() and self._curChar > 0:
            self._curChar -= 1
            self.refresh(stats.activeRoster[self._curChar])
        if controls.right() and self._curChar < len(stats.activeRoster) - 1:
            self._curChar += 1
            self.refresh(stats.activeRoster[self._curChar])

        if controls.up():
            self.skillWindow.ywin -= 1

        if controls.down():
            self.skillWindow.ywin += 1

        if controls.cancel() or controls.enter():
            return menu.Cancel

    #--------------------------------------------

    def draw(self):
        ika.Map.Render()
        self.portraitWindow.draw()
        self.statWindow.draw()
        self.equipWindow.draw()
        self.skillWindow.draw()

    def execute(self):
        fps = FPSManager()
        result = None

        while result is None:
            result = self.update()
            fps.render(self.draw)

        return True
