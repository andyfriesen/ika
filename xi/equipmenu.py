# equip menu for xi
# coded by Andy Friesen
# copyright whenever.  All rights reserved.
#
# This source code may be used for any purpose, provided that
# the original author is never misrepresented in any way.
#
# There is no warranty, express or implied on the functionality, or
# suitability of this code for any purpose.

import party
import menu
import itemmenu
import statusmenu

from party import *
from menu import Menu
from itemmenu import UpdateItemMenu

from menuwindows import *

def GenerateEquipMenu(curchar):
    window=Menu()
    
    for slotname in curchar.equip:
        eq=curchar.equip[slotname]
        if eq:
            name=eq.name
        else:
            name='';
        window.AddText(slotname.capitalize()+':\t'+name)
        
    window.AutoSize()
    
    return window

def Execute():
    charidx=0
    char = party[charidx]

    portraitwindow = PortraitWindow()
    statwindow = StatusWindow()
    equipwindow = EquipWindow()

    for wnd in (portraitwindow, statwindow, equipwindow):
        wnd.Update(party[charidx])

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
    input.enter=0
    return 1
