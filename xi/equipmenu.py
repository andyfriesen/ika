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

import party
import menu
import itemmenu
import statusmenu
import widget
import item

from menu import Menu
from transition import *

from menuwindows import *
from misc import *

class EquipMenu(object):
    def __init__(_, statbar):
        _.equipwindow = EquipWindow()
        _.portraitwindow = PortraitWindow()
        _.statwindow = StatusWindow()
        _.itemlist = InventoryWindow()
        _.statbar = statbar
        _.charidx = 0
        _.slotidx = 0
        _.itemlist.active = False
        _.equipwindow.active = True
        _.state = _.UpdateEquipWindow
        _.description = widget.TextFrame()
        _.description.AddText('')

    CurChar = property(lambda _: party.party[_.charidx])

    def CurEquipType(_):
        return _.CurChar.equip[_.equipwindow.CursorPos].type

    def StartShow(_):
        _.charidx = 0
        _.Refresh(_.CurChar)

        trans.AddWindowReverse(_.portraitwindow, (-_.portraitwindow.width, _.portraitwindow.y))
        trans.AddWindowReverse(_.statwindow, (ika.Video.xres, _.statwindow.y))
        trans.AddWindowReverse(_.description, (_.description.x, -_.description.height))
        trans.AddWindowReverse(_.itemlist, (_.itemlist.x, ika.Video.yres))
        trans.AddWindowReverse(_.equipwindow, (_.equipwindow.x, -_.equipwindow.height))

    def StartHide(_):
        trans.AddWindow(_.portraitwindow, (ika.Video.xres, _.portraitwindow.y), remove = True)
        trans.AddWindow(_.statwindow, (-_.statwindow.width, _.statwindow.y), remove = True)
        trans.AddWindow(_.description, (_.description.x, -_.description.height), remove = True)
        trans.AddWindow(_.itemlist, (_.itemlist.x, ika.Video.yres), remove = True)
        trans.AddWindow(_.equipwindow, (_.equipwindow.x, -_.equipwindow.height), remove = True)

    def Refresh(_, char):
        for x in (_.equipwindow, _.portraitwindow, _.statwindow):
            x.Refresh(char)

        _.itemlist.Refresh(lambda i: char.CanEquip(i.name) and i.equiptype == _.CurEquipType())

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
        
        if left() and _.charidx > 0:
            _.charidx -= 1
            _.Refresh(_.CurChar)
            
        if right() and _.charidx < len(party.party) - 1:
            _.charidx += 1
            _.Refresh(_.CurChar)

        eqtype = _.CurEquipType()
        result = _.equipwindow.Update()
        eqtype2 = _.CurEquipType()
        # rehighlight the item list so that items that this character can equip in this slot are highlighted.
        if eqtype != eqtype2:
            _.itemlist.Rehighlight(lambda i: char.CanEquip(i.name) and i.equiptype == eqtype2)

        i = char.equip[_.equipwindow.CursorPos].item
        _.description.Text[0] = i and i.desc or ''

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
        _.description.Text[0] = i and i.desc or ''

        if result == None:
            return None
        elif result == -1:
            _.state = _.UpdateEquipWindow
            _.equipwindow.active = True
            _.itemlist.active = False
            return None

        # actually change equipment here
        char = _.CurChar
        selecteditem = party.inv[_.itemlist.CursorPos].item
        slot = char.equip[_.equipwindow.CursorPos].type

        if char.CanEquip(selecteditem.name) and slot == selecteditem.equiptype:
            char.Equip(selecteditem.name, _.equipwindow.CursorPos)

            _.Refresh(party.party[_.charidx])
        
        return None

    def Execute(_):
        _.state = _.UpdateEquipWindow

        while True:
            ika.Input.Update()
            
            ika.Map.Render()

            for x in (_.equipwindow, _.portraitwindow, _.statwindow, _.itemlist, _.statbar, _.description):
                x.Draw()

            ika.Video.ShowPage()

            result = _.state()

            if cancel():
                break
            if result is not None:
                break

        return True
