# equip menu for xi
# coded by Andy Friesen
# copyright whenever.  All rights reserved.
#
# This source code may be used for any purpose, provided that
# the original author is never misrepresented in any way.
#
# There is no warranty, express or implied on the functionality, or
# suitability of this code for any purpose.

import ika
from ika import input

import party
import menu
import itemmenu
import statusmenu
import widget

from menu import Menu

from menuwindows import *

class EquipItemList(Menu):
    def __init__(self):
        Menu.__init__(self)
        self.menuitems = widget.ColumnedTextLabel(columns = 2)
        self.menuitems.Position = (20, 0)
        self.widgets = [self.menuitems]
        
    def Update(self,char):
        self.Clear()
        for i in party.inv:
            if char.CanEquip(i.Name):
                self.menuitems.AddText(i.Name, str(i.qty))
            else:
                self.menuitems.AddText('~3' + i.Name, str(i.qty))
        self.menuitems.AutoSize()
        self.AutoSize()

class EquipMenu(object):
    def __init__(self):
        self.equipwindow = EquipWindow()
        self.portraitwindow = PortraitWindow()
        self.statwindow = StatusWindow()
        self.itemlist = EquipItemList()
        self.charidx = 0


    def Refresh(self, char):
        for x in (self.equipwindow, self.portraitwindow, self.statwindow, self.itemlist):
            x.Update(char)

        # Layout
        self.portraitwindow.DockLeft().DockTop()
        self.statwindow.DockLeft(self.portraitwindow).DockTop()
        self.equipwindow.DockLeft(self.portraitwindow).DockTop(self.statwindow)
        self.itemlist.DockLeft()
        self.itemlist.y = max(self.equipwindow.Bottom, self.portraitwindow.Bottom) + self.itemlist.border

    def UpdateEquipWindow(self):
        if input.left and self.charidx > 0:
            self.charidx -= 1
            self.Refresh(party.party[charidx])
            
        
        return None

    def UpdateItemWindow(self):
        result = self.itemlist.Update()
        return None

    def Execute(self):
        self.charidx = 0
        self.Refresh(party.party[self.charidx])

        states = ( self.UpdateEquipWindow, self.UpdateItemWindow )
        curstate = 0

        while True:
            input.Update()
            
            ika.map.Render()

            for x in (self.equipwindow, self.portraitwindow, self.statwindow, self.itemlist):
                x.Draw()

            ika.ShowPage()

            result = states[curstate]()
            if result is not None:
                break
            
        return True
"""
def GenerateEquipMenu(curchar):
    window=Menu()
    
    for slotname in curchar.equip:
        eq=curchar.equip[slotname]
        if eq:
            name = eq.name
        else:
            name = '';
        window.AddText(slotname.capitalize() + ':\t' + name)
        
    window.AutoSize()
    
    return window

def Execute():
    charidx=0
    char = party.party[charidx]

    portraitwindow = PortraitWindow()
    statwindow = StatusWindow()
    equipwindow = EquipWindow()

    for wnd in (portraitwindow, statwindow, equipwindow):
        wnd.Update(party.party[charidx])

    itemmenu=Menu(equipmenu.x+equipmenu.width+16,20)
    UpdateItemMenu(itemmenu)
    itemmenu.AutoSize()

    active=equipmenu

    while 1:
        result = active.Update()

        if result is not None:
            break

        map.Render()

        for x in (statwindow, portraitwindow, equipwindow, itemmenu):
            hx.Draw()
        
        ShowPage()
        input.Update()

    input.Update()
    input.enter = 0
    return 1
"""