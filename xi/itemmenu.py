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

from party import itemdb,inv

# TODO: class wrap this junk

def UpdateItemMenu():
    self.Clear()
    for itm in inv:
            itemmenu.AddText( itm.item.name + '\t' + str(itm.qty) )

#------------------------------------------------------------------------------

def Execute():
    statbar = StatusBar()
    statbar.Update()
    statbar.DockTop()
    statbar.DockRight()

    itemmenu = menu.Menu()
    
    description = widget.TextFrame()
    description.Position = 10, 10
    description.AddText('')
    description.AutoSize()
    description.width = statbar.x - 20
    
    itemmenu.x = 10
    itemmenu.y = 10 + description.Bottom
    
    UpdateItemMenu(itemmenu)    

    while 1:
        ika.map.Render()
        statbar.Draw()
        result = itemmenu.Update()
        
        # update the description box
        description.text.text[0] = inv.items[itemmenu.cursory].item.desc

        itemmenu.Draw()
        description.Draw()
        
        ika.ShowPage()

        if result == None:
            continue
        elif result == -1:
            break
            
        # TODO: implement using items

    return 1

#------------------------------------------------------------------------------
