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
from misc import *
from transition import *

class ItemMenu(object):
    def __init__(_, statbar):
        _.menu = InventoryWindow()
        
        _.statbar = statbar

        _.description = widget.TextFrame()
        _.description.DockTop().DockLeft()
        _.description.AddText( '' )
        _.description.AutoSize()

        _.menu.DockLeft().DockTop(_.description)
        _.description.Right = _.statbar.x - _.statbar.border * 2
        _.menu.YMax = (ika.GetScreenImage().height - _.menu.y - 20) / _.menu.Font.height

    #--------------------------------------------
    
    def StartShow(_):
        _.description.DockTop().DockLeft()
        _.menu.DockLeft().DockTop(_.description)
        _.description.Right = _.statbar.x - _.statbar.border * 2
        _.Refresh()

        trans.AddWindowReverse(_.description, (_.description.x, -_.description.height) )
        trans.AddWindowReverse(_.menu, (_.menu.x, YRes()) )

    #--------------------------------------------
    
    def StartHide(_):
        trans.AddWindow(_.description, (_.description.x, -_.description.height), remove = True )
        trans.AddWindow(_.menu, (_.menu.x, YRes()), remove = True )

    #--------------------------------------------

    def Refresh(_):
        _.menu.Refresh(lambda i: i.fieldeffect is not None)
        _.menu.AutoSize()
        _.statbar.Refresh()
        trans.Reset()

    #--------------------------------------------

    def Execute(_):
        
        while True:
            _.description.text[0] = party.inv[_.menu.CursorPos].Description

            ika.map.Render()
            for x in (_.menu, _.statbar, _.description):
                x.Draw()
            ika.ShowPage()

            result = _.menu.Update()
            if result == -1:
                break
            
            if result is not None:
                item = party.inv[_.menu.CursorPos].item
                if item.fieldeffect is not None:
                    result = item.fieldeffect()
                    if result is None and item.consumable:
                        party.inv.Take(item.name)
                    _.Refresh()

        return True

#--------------------------------------------