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
from fps import FPSManager
from statset import StatSet

from menu import Menu
from transition import *

from menuwindows import *
from misc import *

class StatPreviewWindow(StatusWindow):
    def __init__(self):
        widget.Frame.__init__(self)
        self.text = widget.ColumnedTextLabel(columns = 3)
        self.AddChild(self.text)

    def RefreshItemColumn(self, oldStats, newStats):
        def PlusMinus(old, new):
            if old > new:   s = '~4'  # Worse
            elif old < new: s = '~6'  # better
            else:           s = '~0'  # equal

            return s + str(new) + '~0'

        self.text.columns[1].SetText(
            PlusMinus(oldStats.atk, newStats.atk),
            PlusMinus(oldStats.grd, newStats.grd),
            PlusMinus(oldStats.hit, newStats.hit),
            PlusMinus(oldStats.eva, newStats.eva),
            '',
            PlusMinus(oldStats.str, newStats.str),
            PlusMinus(oldStats.vit, newStats.vit),
            PlusMinus(oldStats.mag, newStats.mag),
            PlusMinus(oldStats.wil, newStats.wil),
            PlusMinus(oldStats.spd, newStats.spd),
            PlusMinus(oldStats.luk, newStats.luk),
            )

class EquipMenu(object):
    def __init__(self, statbar):
        self.equipwindow = EquipWindow()
        self.portraitwindow = PortraitWindow()
        self.statwindow = StatPreviewWindow() #StatusWindow()
        self.itemlist = InventoryWindow()
        self.statbar = statbar
        self.charidx = 0
        self.slotidx = 0
        self.itemlist.active = False
        self.equipwindow.active = True
        self.state = self.UpdateEquipWindow
        self.description = widget.TextFrame()
        self.description.AddText('')

    CurChar = property(lambda self: party.party[self.charidx])

    def CurEquipType(self):
        return self.CurChar.equip[self.equipwindow.CursorPos].type

    def RefreshStatPreview(self):
        char = self.CurChar                                    # current character
        selecteditem = party.inv[self.itemlist.CursorPos].item # item the cursor is pointing to
        slot = char.equip[self.equipwindow.CursorPos].type     # name of the slot the equip pointer is pointing to

        if char.CanEquip(selecteditem.name) and slot == selecteditem.equiptype:
            oldItem = char.equip[self.equipwindow.CursorPos].item
            oldStats = oldItem and oldItem.stats or StatSet()

            newStats = char.stats.Clone()
            newStats -= oldStats
            newStats += selecteditem.stats
            # blech. -_-
            newStats.atk -= oldStats.str
            newStats.atk += selecteditem.stats.str
            newStats.grd += selecteditem.stats.vit
            self.statwindow.RefreshItemColumn(char.stats, newStats)
        else:
            self.statwindow.RefreshItemColumn(char.stats, char.stats)

    def StartShow(self):
        self.charidx = 0
        self.Refresh(self.CurChar)

        trans.AddWindow(self.statbar, (self.statbar.x, -(self.statbar.height + self.statbar.border * 2)))
        trans.AddWindowReverse(self.portraitwindow, (-self.portraitwindow.width, self.portraitwindow.y))
        trans.AddWindowReverse(self.statwindow, (ika.Video.xres, self.statwindow.y))
        trans.AddWindowReverse(self.description, (self.description.x, -self.description.height))
        trans.AddWindowReverse(self.itemlist, (self.itemlist.x, ika.Video.yres))
        trans.AddWindowReverse(self.equipwindow, (self.equipwindow.x, -self.equipwindow.height))

    def StartHide(self):
        self.statbar.DockTop().DockRight()
        trans.AddWindowReverse(self.statbar, (self.statbar.x, ika.Video.yres + self.statbar.border * 2))
        trans.AddWindow(self.portraitwindow, (ika.Video.xres, self.portraitwindow.y), remove = True)
        trans.AddWindow(self.statwindow, (-self.statwindow.width, self.statwindow.y), remove = True)
        trans.AddWindow(self.description, (self.description.x, -self.description.height), remove = True)
        trans.AddWindow(self.itemlist, (self.itemlist.x, ika.Video.yres), remove = True)
        trans.AddWindow(self.equipwindow, (self.equipwindow.x, -self.equipwindow.height), remove = True)

    def Refresh(self, char):
        for x in (self.equipwindow, self.portraitwindow, self.statwindow):
            x.Refresh(char)

        # Layout
        self.portraitwindow.DockTop().DockLeft()

        self.description.DockTop().DockLeft(self.portraitwindow)
        self.description.Right = ika.Video.xres - self.statbar.border #self.statbar.x - self.statbar.border * 2

        self.statwindow.DockTop(self.portraitwindow).DockLeft()
        self.statwindow.width = self.portraitwindow.width
        self.equipwindow.DockTop(self.description).DockLeft(self.portraitwindow)
        self.itemlist.DockTop(self.equipwindow).DockLeft(self.statwindow)
        self.statbar.Refresh()
        self.equipwindow.Right = self.description.Right #self.statbar.x - self.statbar.border * 2

        if len(party.inv) > 0:
            self.itemlist.Refresh(lambda i: char.CanEquip(i.name) and i.equiptype == self.CurEquipType())
            self.itemlist.Right = self.description.Right#width = self.equipwindow.width
            self.itemlist.Layout()
        else:
            self.itemlist.Clear()
            self.itemlist.AddText('No Items')

    def UpdateEquipWindow(self):
        char = self.CurChar

        if left() and self.charidx > 0:
            self.charidx -= 1
            self.Refresh(self.CurChar)

        if right() and self.charidx < len(party.party) - 1:
            self.charidx += 1
            self.Refresh(self.CurChar)

        eqtype = self.CurEquipType()
        result = self.equipwindow.Update()
        eqtype2 = self.CurEquipType()
        # rehighlight the item list so that items that this character can equip in this slot are highlighted.
        if eqtype != eqtype2:
            self.itemlist.Rehighlight(lambda i: char.CanEquip(i.name) and i.equiptype == eqtype2)

        i = char.equip[self.equipwindow.CursorPos].item
        self.description.Text[0] = i and i.desc or ''

        if result is menu.Cancel or result is None:
            return result

        if len(party.inv) > 0:
            self.slotidx = result
            self.state = self.UpdateItemWindow
            self.equipwindow.active = False
            self.itemlist.active = True
        return None

    def UpdateItemWindow(self):
        oldPos = self.itemlist.CursorPos

        result = self.itemlist.Update()

        i = party.inv[self.itemlist.CursorPos].item
        self.description.Text[0] = i and i.desc or ''


        if self.itemlist.CursorPos != oldPos:
            self.RefreshStatPreview()

        if result == None:
            return None
        elif result == menu.Cancel:
            ika.Input.Unpress()
            self.state = self.UpdateEquipWindow
            self.equipwindow.active = True
            self.itemlist.active = False

            # Clear the stat preview thing.
            s = self.CurChar.stats
            self.statwindow.RefreshItemColumn(s, s)
            return None

        # actually change equipment here
        char = self.CurChar
        selecteditem = party.inv[self.itemlist.CursorPos].item
        slot = char.equip[self.equipwindow.CursorPos].type

        if char.CanEquip(selecteditem.name) and slot == selecteditem.equiptype:
            char.Equip(selecteditem.name, self.equipwindow.CursorPos)

            self.Refresh(party.party[self.charidx])

        return None

    def Draw(self):
        ika.Input.Update()
        ika.Map.Render()
        trans.Draw()
        #for x in (self.equipwindow, self.portraitwindow, self.statwindow, self.itemlist, self.statbar, self.description):
        #    x.Draw()

    def Execute(self):
        self.state = self.UpdateEquipWindow
        time = ika.GetTime()
        fps = FPSManager()

        while True:
            result = self.state()
            if cancel():
                break
            if result is not None:
                break

            fps.Render(self.Draw)

        return True
