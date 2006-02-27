# Item menu for pi
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
from xi import layout
from xi.fps import FPSManager
from xi.menu import Menu, Cancel
from xi.transition import Transition
from xi.misc import *

import menu.mainmenu
import menu.common

import stats
import stats.item

from stats import inventory

class ItemMenu(object):
    def __init__(self, statbar):
        self.statbar = statbar
        self.itemList = self.createInventoryWindow()
        self.menu = gui.FrameDecorator(Menu(textctrl=self.itemList))
        self.description = self.createDescriptionBar()

    def createInventoryWindow(self):
        return menu.common.InventoryWindow(stats.inventory)

    #--------------------------------------------

    # Maybe someone wants to make a multiline description field.
    # Like Star Ocean.  You could even put an image in here somewhere, or something.
    def createDescriptionBar(self):
        return gui.FrameDecorator(gui.StaticText(text=['','']))

    #--------------------------------------------

    def setDescriptionBarText(self, desc):
        # there's a second line, but we'll play ika's willingness to print
        # newlines and just let it go.

        # wordwrap, take the first two lines (that's all we have room for) and join with a newline
        t = '\n'.join(wrapText(desc, self.description.client.width, self.description.font)[:2])
        self.description.text[0] = t

    #--------------------------------------------

    def updateDescriptionBar(self):
        if self.menu.cursorPos < len(inventory):
            self.setDescriptionBarText(inventory[self.menu.cursorPos].description)
        else:
            self.setDescriptionBarText('')

    #--------------------------------------------

    def layout(self):
        self.description.autoSize()
        l = layout.VerticalBoxLayout(
            0, 0, ika.Video.xres - 16, ika.Video.yres,
            children=[self.description, self.menu],
            pad=8,
            stretch=True)
        l.layout()

        # hack: Nudge each control down and to the right a bit.
        self.description.x += 8
        self.description.y += 8
        self.menu.x += 8
        self.menu.y += 8

        # Keep the menu from going past the bottom of the screen.
        self.menu.height = ika.Video.yres - self.menu.y - self.menu.border * 4

    #--------------------------------------------

    def startShow(self, transition):
        self.layout()
        self.refresh()

        transition.addChild(self.description, startRect=(self.description.x, -self.description.height * 2) )
        transition.addChild(self.menu, startRect=(self.menu.x, ika.Video.yres) )
        self.updateDescriptionBar()

    #--------------------------------------------

    def startHide(self, transition):
        transition.addChild(self.description, endRect=(self.description.x, -self.description.height * 2))
        transition.addChild(self.menu, endRect=(self.menu.x, ika.Video.yres))

    #--------------------------------------------

    def refresh(self):
        self.itemList.refresh(lambda i: i.fieldEffect is not None)

        self.statbar.refresh()

        if len(inventory) == 0:
            self.menu.addText('No Items', '', '')

        self.layout()

    #--------------------------------------------

    def useItem(self, item):
        try:
            # first see if the effect wants us as an argument
            result = item.fieldEffect(self)
        except TypeError:
            # if it doesn't, that's fine too.
            result = item.fieldEffect()

        if result is None and item.consumable:
            inventory.take(item.name)
        self.refresh()

    def draw(self):
        ika.Map.Render()
        self.description.draw()
        self.menu.draw()

    def execute(self):
        fps = FPSManager()

        result = None
        while result is not Cancel:
            oldPos = self.menu.cursorPos

            result = self.menu.update()

            if result not in (None, Cancel):
                # use an item
                item = inventory[self.menu.cursorPos].item
                if item.fieldEffect is not None:
                    self.useItem(item)
                    fps.sync()

            if oldPos != self.menu.cursorPos:
                self.updateDescriptionBar()

            fps.render(self.draw)

        return True

#--------------------------------------------
