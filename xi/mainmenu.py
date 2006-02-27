# Main menu
# coded by Andy Friesen
# copyright whenever.  All rights reserved.
#
# This source code may be used for any purpose, provided that
# the original author is never misrepresented in any way.
#
# There is no warranty, express or implied on the functionality, or
# suitability of this code for any purpose.

import ika
import party
import widget
import menu
from fps import FPSManager
from menu import Menu
from statelessproxy import StatelessProxy

from menuwindows import StatusBar
from transition import *
from misc import *
import statusmenu
import itemmenu
import equipmenu
import skillmenu

class MainMenu(StatelessProxy):
    def __init__(self):
        StatelessProxy.__init__(self)
        if len(self.__dict__) != 0:
            return
        # "static constructor" logic follows

        self.statbar = StatusBar()
        self.statbar.Refresh()
        self.statbar.DockRight().DockTop()

        mm = self.mainmenu = Menu()        

        self.SetMenuItems(self.CreateMenuItems())

        #mm.AddText('Item','Skills','Equip','Status','Order','Load','Save','Quit')
        mm.DockLeft().DockTop()

    def SetMenuItems(self, menuItems):
        self.mainmenu.Clear()
        self.submenu = [None] * len(menuItems)
        for i, (name, menu) in enumerate(menuItems):
            self.mainmenu.AddText(name)
            self.submenu[i] = menu

    def CreateMenuItems(self):
        return [
            ('Item', itemmenu.ItemMenu(self.statbar)),
            ('Skills', skillmenu.SkillMenu(self.statbar)),
            ('Equip', equipmenu.EquipMenu(self.statbar)),
            ('Status', statusmenu.StatusMenu(self.statbar))
            ]

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

        trans.AddWindowReverse(self.statbar, (self.statbar.Left, ika.Video.yres))
        trans.AddWindowReverse(self.mainmenu, (-self.mainmenu.width, self.mainmenu.Top))
        trans.Execute()

    def Hide(self):
        trans.AddWindow(self.statbar, (self.statbar.Left, -self.statbar.height), remove = True)
        trans.AddWindow(self.mainmenu, (ika.Video.xres, self.mainmenu.y), remove = True)
        trans.Execute()
        trans.Reset()

    def RunMenu(self, menu):
        # hold onto this so we can put the menu back later
        mainMenuPos = self.mainmenu.Rect

        menu.StartShow()
        trans.AddWindow(self.mainmenu, (ika.Video.xres + 20, self.mainmenu.y) )
        trans.Execute()

        result = menu.Execute()

        menu.StartHide()
        self.mainmenu.x = -self.mainmenu.width  # put the menu at stage left
        trans.AddWindow(self.mainmenu, mainMenuPos)       # restore the menu's position
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

#------------------------------------------------------------------------------

class Dummy(object):
    def StartShow(self):
        pass
    def StartHide(self):
        pass
    def Execute(self):
        return True
