#!/usr/bin/env python

"""Item menu for xi."""

# Coded by Andy Friesen.
# Copyright whenever.  All rights reserved.

# This source code may be used for any purpose, provided that the
# original author is never misrepresented in any way.

# There is no warranty, express or implied on the functionality, or
# suitability of this code for any purpose.

import ika

import xi.party
import xi.menu
import xi.menuwindows
import xi.widget
from xi.fps import FPSManager
from xi.transition import Transition


class ItemMenu(object):

    def __init__(self, statusbar):
        self.statusbar = statusbar
        self.menu = self.CreateInventoryWindow()
        self.description = self.CreateDescriptionBar()
        self.description.AddText('')

    def CreateInventoryWindow(self):
        return xi.menuwindows.InventoryWindow()

    def CreateDescriptionBar(self):
        # Maybe someone wants to make a multiline description field like
        # Star Ocean.  You could even put an image in here somewhere, or
        # something.
        return xi.widget.TextFrame()

    def UpdateDescriptionBar(self, item):
        self.description.Text[0] = item.Description

    def Layout(self):
        self.description.DockTop().DockLeft()
        self.menu.DockLeft().DockTop(self.description)
        self.menu.YMax = ((ika.Video.yres - self.menu.y - 20) /
                          self.menu.Font.height)
        self.description.Right = self.statbar.x - self.statbar.border * 2

    def StartShow(self):
        self.Layout()
        self.Refresh()
        trans.AddWindowReverse(self.description,
                               (self.description.x,
                                -self.description.height * 2))
        trans.AddWindowReverse(self.menu, (self.menu.x, ika.Video.yres))

    def StartHide(self):
        trans.AddWindow(self.description,
                        (self.description.x, -self.description.height * 2),
                        remove=True)
        trans.AddWindow(self.menu, (self.menu.x, ika.Video.yres), remove=True)

    def Refresh(self):
        self.menu.Refresh(lambda i: i.fieldeffect is not None)
        self.menu.AutoSize()
        self.statbar.Refresh()
        self.menu.Right = self.statbar.Left - self.statbar.border * 2
        if len(xi.party.inv) > 0:
            self.menu.active = True
        else:
            self.menu.AddText('No items.')
            self.menu.CursorPos = 0
            self.menu.active = False
        self.menu.Layout()
        trans.Reset()

    def Update(self):
        result = self.menu.Update()
        if result == xi.menu.Cancel:
            return xi.menu.Cancel
        if result is not None:
            item = xi.party.inv[self.menu.CursorPos].item
            if item.fieldeffect is not None:
                result = item.fieldeffect()
                if result is None and item.consumable:
                    xi.party.inv.Take(item.name)
                self.Refresh()

    def Execute(self):
        def Draw():
            ika.Render()
            trans.Draw()
        fps = FPSManager()
        while True:
            oldposition = self.menu.CursorPos
            result = self.Update()
            if result is not None:
                break
            if oldposition != self.menu.CursorPos:
                self.UpdateDescriptionBar(xi.party.inv[self.menu.CursorPos])
            fps.Render(Draw)
        return True
