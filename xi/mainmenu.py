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
        mm = self.mainmenu = Menu()
        mm.AddText('Item','Skills','Equip','Status','Order','Load','Save','Quit')
        mm.DockLeft().DockTop()

        self.statbar = StatusBar()
        self.statbar.Refresh()
        self.statbar.DockRight().DockTop()

        dummy = Dummy()
        self.submenu = [ itemmenu.ItemMenu(self.statbar),
                         skillmenu.SkillMenu(self.statbar),
                         equipmenu.EquipMenu(self.statbar),
                         statusmenu.StatusMenu(self.statbar),
                         dummy,
                         dummy,
                         dummy,
                         dummy ]


    def Draw(self):
        self.statbar.Draw()
        self.mainmenu.Draw()

    def Update(self):
        return self.mainmenu.Update()
        
    def Show(self):
        self.statbar.DockRight().DockTop()
        self.mainmenu.DockLeft().DockTop()

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
        r = self.mainmenu.Rect
        
        menu.StartShow()
        trans.AddWindow(self.mainmenu, (ika.Video.xres + 20, self.mainmenu.y) )
        trans.Execute()
        
        result = menu.Execute()
        
        menu.StartHide()
        self.mainmenu.x = -self.mainmenu.width  # put the menu at stage left
        trans.AddWindow(self.mainmenu, r)       # restore the menu's position
        trans.Execute()

        return result

    def Execute(self):
        self.Show()
        #ika.Input.cancel.Pressed() # flush
        
        self.statbar.Refresh()
        
        done = 0
        while not done:
            ika.Map.Render()
            self.Draw()
            ika.Video.ShowPage()
            
            result = self.Update()

            if result == -1:
                done = 1
            
            elif result != None:
                result = self.RunMenu(self.submenu[result])
                if not result:
                    break

        self.Hide()

#------------------------------------------------------------------------------

class Dummy(object):
    def StartShow(_):
        pass
    def StartHide(_):
        pass
    def Execute(_):
        return True
