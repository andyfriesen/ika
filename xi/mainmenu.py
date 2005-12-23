# Main menu for pi
# Coded by Andy Friesen
# Copyright whenever.  All rights reserved.
#
# This source code may be used for any purpose, provided that
# the original author is never misrepresented in any way.
#
# There is no warranty, express or implied on the functionality, or
# suitability of this code for any purpose.

import ika

import xi
import xi.party
from xi import gui
from xi import menu
from xi.fps import FPSManager
from xi.menu import Menu
from xi.widgetmanager import *
from xi.transition import Transition

from menuwindows import StatusBar, MiscWindow
from itemmenu import ItemMenu
from equipmenu import EquipMenu
from skillmenu import SkillMenu
from statusmenu import StatusMenu

# main WM used for most (if not all) field menu stuff
wm = WidgetManager()

class MainMenu(xi.StatelessProxy):
    def __init__(self):
        super(MainMenu, self).__init__()
        if len(self.__dict__) != 0:
            return
        # "static constructor" logic follows

        mm = self.mainMenu = gui.FrameDecorator(Menu())

        self.statbar = gui.FrameDecorator(StatusBar())
        self.statbar.refresh()
        self.statbar.dockRight().dockTop()
        self.miscwindow = gui.FrameDecorator(MiscWindow())
        self.miscwindow.refresh()
        self.miscwindow.dockRight().dockBottom()

        self.setMenuItems(self.createMenuItems())

        mm.autoSize()
        mm.dockTop().dockLeft()

    def setMenuItems(self, menuItems):
        self.mainMenu.clear()
        self.mainMenu.addText(*[ name for (name, menu) in menuItems ])
        self.submenu = [ menu for (name, menu) in menuItems ]

    def createMenuItems(self):
        return [
            ('Item', ItemMenu(self.statbar)),
            ('Skills', SkillMenu()),
            ('Equip', EquipMenu()),
            ('Status', StatusMenu()),
            ]

    def draw(self):
        ika.Map.Render()
        self.statbar.draw()
        self.miscwindow.draw()
        self.mainMenu.draw()

    def update(self):
        return self.mainMenu.update()

    def layout(self):
        self.mainMenu.dockLeft().dockTop()
        self.statbar.dockRight().dockTop()
        self.miscwindow.dockRight().dockBottom()

    def show(self):
        self.layout()

        b = self.statbar.border

        t = Transition()
        t.addChild(self.statbar, startRect=(self.statbar.left, ika.Video.yres + b))
        t.addChild(self.miscwindow, startRect=(self.miscwindow.left, ika.Video.yres + b))
        t.addChild(self.mainMenu, startRect=(-self.mainMenu.width - b, self.mainMenu.top))

        t.execute()

    def hide(self):
        t = Transition()
        t.addChild(self.statbar, endRect=(self.statbar.left, -self.statbar.height))
        t.addChild(self.miscwindow, endRect=(self.miscwindow.left, -self.miscwindow.height))
        t.addChild(self.mainMenu, endRect=(ika.Video.xres, self.mainMenu.top))

        t.execute()

    def runMenu(self, menu):
        # Save window positions, so we can put them back later.
        mainMenuPos = self.mainMenu.rect
        statBarPos = self.statbar.rect
        miscWndPos = self.miscwindow.rect

        t = Transition()
        menu.startShow(t)
        t.addChild(self.mainMenu, endRect=(ika.Video.xres + 20, self.mainMenu.y) )
        t.addChild(self.statbar, endRect=(self.statbar.x, -(self.statbar.height + self.statbar.border * 2)))
        t.addChild(self.miscwindow, endRect=(self.miscwindow.x, ika.Video.yres + self.miscwindow.border * 2))
        t.execute()

        result = menu.execute()

        menu.startHide(t)
        self.mainMenu.x = -self.mainMenu.width  # put the menu at stage left
        t.addChild(self.mainMenu, endRect=mainMenuPos)       # restore the menu's position
        t.addChild(self.statbar, endRect=statBarPos)
        t.addChild(self.miscwindow, endRect=miscWndPos)
        t.execute()

        return result


    def execute(self):

        self.statbar.refresh()
        self.miscwindow.refresh()
        self.show()

        fps = FPSManager(100)

        while True:
            result = self.update()
            self.miscwindow.refresh()

            if result is menu.Cancel:
                break

            elif result is not None:
                result = self.runMenu(self.submenu[result])
                fps.sync()
                if not result:
                    break

            fps.render(self.draw)

        self.hide()

#------------------------------------------------------------------------------

class Dummy(object):
    def startShow(*args):
        pass
    def startHide(*args):
        pass
    def execute(*args):
        return True
