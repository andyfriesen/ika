# item menu for xi
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
import menu
import widget
import item
import mainmenu
from menuwindows import *
import party

class ItemMenu(object):
    def __init__(self):
        self.menu = menu.Menu(textcontrol = widget.ColumnedTextLabel(columns = 2))

        self.statbar = StatusBar()
        self.statbar.Update()
        self.statbar.AutoSize()
        
        self.description = widget.TextFrame()
        self.description.DockTop().DockLeft()
        self.description.AddText( '' )
        self.description.AutoSize()

        self.statbar.DockTop().DockRight()
        self.menu.DockLeft().DockTop(self.description)
        self.description.width = self.statbar.x - self.description.x
        self.menu.menuitems.YMax = 20

    #--------------------------------------------

    def Refresh(self):
        self.statbar.Update()
        self.menu.Clear()
        for itm in party.inv:
            self.menu.AddText(itm.Name, str(itm.qty) )
        self.menu.AutoSize()

    #--------------------------------------------

    def Execute(self):
        self.Refresh()
        
        while True:
            self.description.text[0] = party.inv[self.menu.CursorPos].Description

            ika.map.Render()
            for x in (self.menu, self.statbar, self.description):
                x.Draw()
            ika.ShowPage()

            result = self.menu.Update()
            if result == -1:
                break
            
            if result is not None:
                pass # Handle item use here

        return True

#--------------------------------------------