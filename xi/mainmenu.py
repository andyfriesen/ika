#!/usr/bin/env python

"""Main menu."""

# Coded by Andy Friesen
# Copyright whenever.  All rights reserved.

# This source code may be used for any purpose, provided that the
# original author is never misrepresented in any way.

# There is no warranty, express or implied on the functionality, or
# suitability of this code for any purpose.

import ika

import xi.equipmenu
import xi.itemmenu
import xi.skillmenu
import xi.statusmenu

from xi.fps import FPSManager
from xi.menu import Menu
from xi.menuwindows import StatusBar
from xi.statelessproxy import StatelessProxy
from xi.transition import Transition


class MainMenu(StatelessProxy):

    def __init__(self):
        super(MainMenu, self).__init__()
        if self.__dict__:
            return
        # "Static constructor" logic follows.
        self.statbar = StatusBar()
        self.statbar.Refresh()
        self.statbar.DockRight().DockTop()
        self.mainmenu = Menu()
        self.SetMenuItems(self.CreateMenuItems())
        #self.mainmenu.AddText('Item', 'Skills', 'Equip', 'Status', 'Order',
        #                      'Load', 'Save', 'Quit')
        self.mainmenu.DockLeft().DockTop()

    def SetMenuItems(self, menuItems):
        self.mainmenu.Clear()
        self.submenu = [None] * len(menuItems)
        for i, (name, menu) in enumerate(menuItems):
            self.mainmenu.AddText(name)
            self.submenu[i] = menu

    def CreateMenuItems(self):
        return [('Item', xi.itemmenu.ItemMenu(self.statbar)),
                ('Skills', xi.skillmenu.SkillMenu(self.statbar)),
                ('Equip', xi.equipmenu.EquipMenu(self.statbar)),
                ('Status', xi.statusmenu.StatusMenu(self.statbar))]

    def Draw(self):
        self.statbar.Draw()
        self.mainmenu.Draw()

    def Update(self):
        return self.mainmenu.Update()

    def Layout(self):
        self.statbar.DockRight().DockTop()
        self.mainmenu.DockLeft().DockTop()

    def Show(self):
        self.Layout()
        trans.AddWindowReverse(self.statbar, (self.statbar.Left,
                                              ika.Video.yres))
        trans.AddWindowReverse(self.mainmenu, (-self.mainmenu.width,
                                               self.mainmenu.Top))
        trans.Execute()

    def Hide(self):
        trans.AddWindow(self.statbar,
                        (self.statbar.Left, -self.statbar.height), remove=True)
        trans.AddWindow(self.mainmenu, (ika.Video.xres, self.mainmenu.y),
                        remove=True)
        trans.Execute()
        trans.Reset()

    def RunMenu(self, menu):
        # Hold onto this so we can put the menu back later.
        mainMenuPos = self.mainmenu.Rect
        menu.StartShow()
        trans.AddWindow(self.mainmenu, (ika.Video.xres + 20, self.mainmenu.y) )
        trans.Execute()
        result = menu.Execute()
        menu.StartHide()
        # put the menu at stage left
        self.mainmenu.x = -self.mainmenu.width
        # restore the menu's position
        trans.AddWindow(self.mainmenu, mainMenuPos)
        trans.Execute()

        return result

    def Execute(self):
        def draw():
            ika.Map.Render()
            self.Draw()
        self.statbar.Refresh()
        self.Show()
        fps = FPSManager()
        while True:
            result = self.Update()
            if result is menu.Cancel:
                break
            elif result is not None:
                result = self.RunMenu(self.submenu[result])
                if not result:
                    break
            fps.Render(draw)
        self.Hide()


class Dummy(object):

    def StartShow(self):
        pass

    def StartHide(self):
        pass

    def Execute(self):
        return True
