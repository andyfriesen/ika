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
import item

from menu import Menu

from menuwindows import *
from misc import *

class EquipMenu(object):
    def __init__(_):
        _.equipwindow = EquipWindow()
        _.portraitwindow = PortraitWindow()
        _.statwindow = StatusWindow()
        _.itemlist = InventoryWindow()
        _.statbar = StatusBar()
        _.statbar.Refresh()
        _.statbar.AutoSize()
        _.statbar.DockTop().DockRight()
        _.charidx = 0
        _.slotidx = 0
        _.itemlist.active = False
        _.equipwindow.active = True
        _.state = _.UpdateEquipWindow
        _.description = widget.TextFrame()
        _.description.AddText('')

    CurChar = property(lambda _: party.party[_.charidx])        

    def StartShow(_, trans):
        _.Refresh(_.CurChar)
        trans.AddWindowReverse(_.portraitwindow, (-_.portraitwindow.width, _.portraitwindow.y))
        trans.AddWindowReverse(_.statwindow, (XRes(), _.statwindow.y))
        trans.AddWindowReverse(_.description, (_.description.x, -_.description.height))
        trans.AddWindowReverse(_.equipwindow, (_.equipwindow.x, -_.equipwindow.height))
        trans.AddWindowReverse(_.itemlist, (_.itemlist.x, YRes()))
        
    def StartHide(_, trans):
        trans.AddWindow(_.portraitwindow, (XRes(), _.portraitwindow.y), remove = True)
        trans.AddWindow(_.statwindow, (-_.statwindow.width, _.statwindow.y), remove = True)
        trans.AddWindow(_.description, (_.description.x, -_.description.height), remove = True)
        trans.AddWindow(_.equipwindow, (_.equipwindow.x, -_.equipwindow.height), remove = True)
        trans.AddWindow(_.itemlist, (_.itemlist.x, YRes()), remove = True)

    def Refresh(_, char):
        for x in (_.equipwindow, _.portraitwindow, _.statwindow):
            x.Refresh(char)

        _.itemlist.Refresh(lambda i: char.CanEquip(i.name))

        # Layout
        _.portraitwindow.DockTop().DockLeft()
        
        _.description.DockTop().DockLeft(_.portraitwindow)
        _.description.Right = _.statbar.x - _.statbar.border * 2
        
        _.statwindow.DockTop(_.portraitwindow).DockLeft()
        _.statwindow.width = _.portraitwindow.width
        _.equipwindow.DockTop(_.description).DockLeft(_.portraitwindow)
        _.itemlist.DockTop(_.equipwindow).DockLeft(_.statwindow)
        _.statbar.Refresh()
        _.equipwindow.Right = _.statbar.x - _.statbar.border * 2
        _.itemlist.width = _.equipwindow.width

    def UpdateEquipWindow(_):
        char = _.CurChar
        
        if input.left and _.charidx > 0:
            input.left = False
            _.charidx -= 1
            _.Refresh(_.CurChar)
            
        if input.right and _.charidx < len(party.party) - 1:
            input.right = False
            _.charidx += 1
            _.Refresh(_.CurChar)

        result = _.equipwindow.Update()

        k = item.equiptypes[_.equipwindow.CursorPos]
        i = char.equip[k]
        _.description.text[0] = i and i.desc or ''

        if result == -1 or result == None:
            return result

        _.slotidx = result
        _.state = _.UpdateItemWindow
        _.equipwindow.active = False
        _.itemlist.active = True
        return None

    def UpdateItemWindow(_):
        result = _.itemlist.Update()

        i = party.inv[_.itemlist.CursorPos].item
        _.description.text[0] = i and i.desc or ''

        if result == None:
            return None
        elif result == -1:
            _.state = _.UpdateEquipWindow
            _.equipwindow.active = True
            _.itemlist.active = False
            return None

        # actually change equipment here
        char = party.party[_.charidx]
        selecteditem = party.inv[_.itemlist.CursorPos].item
        slot = item.equiptypes[_.equipwindow.CursorPos]

        if char.CanEquip(selecteditem.name) and slot == selecteditem.equiptype:
            char.Equip(selecteditem.name)

            _.Refresh(party.party[_.charidx])
        
        return None

    def Execute(_):
        _.charidx = 0
        _.Refresh(party.party[_.charidx])

        _.state = _.UpdateEquipWindow

        while True:
            input.Update()
            
            ika.map.Render()

            for x in (_.equipwindow, _.portraitwindow, _.statwindow, _.itemlist, _.statbar, _.description):
                x.Draw()

            ika.ShowPage()

            result = _.state()

            if input.cancel:
                break
            if result is not None:
                break

        return True