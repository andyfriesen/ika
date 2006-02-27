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

import stats

from xi import gui
from xi import controls
from xi.fps import FPSManager
from xi.misc import wrapText
from xi.menu import Menu, Cancel
from xi.cursor import NullCursor

import stats
from stats import item
from stats.statset import StatSet

from menu.common import EquipWindow, PortraitWindow, InventoryWindow, StatusWindow

_nullCursor = NullCursor(8, 8, (8, 4))

class StatPreviewWindow(gui.ColumnedTextLabel):
    def __init__(self):
        super(type(self), self).__init__(columns=3, pad=8)

    def refresh(self, char):
        self.clear()
        def add(n, a):
            a = isinstance(a, int) and '%3i' % a or a
            self.addText(n, a, '     ')

        stats = char.stats
        nstats = char.naturalstats

        add('Attack', stats.atk)
        add('Guard', stats.grd)
        add('Hit %', stats.hit)
        add('Evade %', stats.eva)
        add('', '')

        add('Strength', stats.str)
        add('Vitality', stats.vit)
        add('Magic', stats.mag)
        add('Will', stats.wil)
        add('Speed', stats.spd)
        add('Luck', stats.luk)
        self.autoSize()


    def refreshItemColumn(self, oldStats, newStats):
        def plusMinus(old, new):
            if old > new:   s = 4  # Worse
            elif old < new: s = 6  # better
            else:           return '' # equal.  Show nothing.

            return '->~%i%3i' % (s, new)

        # more cheating. :P
        self._columns[2].clear()
        self._columns[2].addText(
            plusMinus(oldStats.atk, newStats.atk),
            plusMinus(oldStats.grd, newStats.grd),
            plusMinus(oldStats.hit, newStats.hit),
            plusMinus(oldStats.eva, newStats.eva),
            '',
            plusMinus(oldStats.str, newStats.str),
            plusMinus(oldStats.vit, newStats.vit),
            plusMinus(oldStats.mag, newStats.mag),
            plusMinus(oldStats.wil, newStats.wil),
            plusMinus(oldStats.spd, newStats.spd),
            plusMinus(oldStats.luk, newStats.luk),
            )

class EquipMenu(object):
    def __init__(self):
        self.equipWindow = EquipWindow()
        self.portraitWindow = PortraitWindow()
        self.statWindow = gui.FrameDecorator(StatPreviewWindow())
        self.itemList = InventoryWindow(stats.inventory)
        self.equipMenu = gui.FrameDecorator(Menu(textctrl=self.equipWindow))
        self.itemMenu = gui.FrameDecorator(Menu(textctrl=self.itemList))
        self.charIdx = 0
        self.slotIdx = 0
        self.itemMenu.cursor = _nullCursor
        self.state = self.updateEquipWindow
        self.description = gui.FrameDecorator(gui.StaticText(text=['','']))

    curChar = property(lambda self: stats.activeRoster[self.charIdx])

    def curEquipType(self):
        return self.curChar.equipment[self.equipMenu.cursorPos].type

    def setDescription(self, desc):
        # wordwrap, take the first two lines (that's all we have room for) and join with a newline
        t = '\n'.join(wrapText(desc, self.description.client.width, self.description.font)[:2])
        self.description.text[0] = t

    def refreshStatPreview(self):
        char = self.curChar                                    # current character
        selecteditem = stats.inventory[self.itemMenu.cursorPos].item # item the cursor is pointing to
        slot = char.equipment[self.equipMenu.cursorPos].type     # name of the slot the equip pointer is pointing to

        if char.canEquip(selecteditem.name) and slot == selecteditem.type:
            oldItem = char.equipment[self.equipMenu.cursorPos].item
            newStats = char.stats.clone()

            if oldItem:
                oldStats = oldItem.stats
                newStats -= oldStats
                newStats.atk -= oldStats.str
                newStats.grd -= oldStats.vit

            newStats += selecteditem.stats

            # hack. update derived statistics
            newStats.atk += selecteditem.stats.str
            newStats.grd += selecteditem.stats.vit
            self.statWindow.refreshItemColumn(char.stats, newStats)
        else:
            self.statWindow.refreshItemColumn(char.stats, char.stats)

    def startShow(self, trans):
        self.charIdx = 0
        self.equipMenu.cursorPos = 0
        self.itemMenu.cursorPos = 0
        self.itemMenu.ywin = 0
        self.refresh(self.curChar)

        trans.addChild(self.portraitWindow, startRect=(-self.portraitWindow.width, self.portraitWindow.y))
        trans.addChild(self.statWindow, startRect=(ika.Video.xres, self.statWindow.y))
        trans.addChild(self.description, startRect=(self.description.x, -(self.description.height + self.description.border * 2)))
        trans.addChild(self.itemMenu, startRect=(self.itemMenu.x, ika.Video.yres))
        trans.addChild(self.equipMenu, startRect=(self.equipMenu.x, -self.equipMenu.height))

    def startHide(self, trans):
        trans.addChild(self.portraitWindow, endRect=(ika.Video.xres, self.portraitWindow.y))
        trans.addChild(self.statWindow, endRect=(-self.statWindow.width, self.statWindow.y))
        trans.addChild(self.description, endRect=(self.description.x, -(self.description.height + self.description.border * 2)))
        trans.addChild(self.itemMenu, endRect=(self.itemMenu.x, ika.Video.yres))
        trans.addChild(self.equipMenu, endRect=(self.equipMenu.x, -self.equipMenu.height))

    def refresh(self, char):
        for x in (self.equipWindow, self.portraitWindow, self.statWindow):
            x.refresh(char)

        if len(stats.inventory) > 0:
            self.itemList.refresh(lambda i: char.canEquip(i.name) and i.type == self.curEquipType())
        else:
            self.itemList.clear()
            self.itemList.addText('No Items', '', '')

        # Layout
        self.portraitWindow.dockTop().dockLeft()

        self.statWindow.dockTop(self.portraitWindow).dockLeft()
        self.statWindow.width = max(self.statWindow.width, self.portraitWindow.width)
        self.portraitWindow.width = self.statWindow.width

        self.description.autoSize()
        self.description.dockTop().dockLeft(self.portraitWindow)
        self.description.width = ika.Video.xres - self.description.x - self.description.border

        self.equipMenu.dockTop(self.description).dockLeft(self.portraitWindow)
        self.equipMenu.width = self.description.width

        self.itemMenu.dockTop(self.equipMenu).dockLeft(self.statWindow)
        self.itemMenu.width = self.description.width

        self.itemMenu.height = min(self.itemMenu.height, ika.Video.yres - self.itemMenu.y - self.itemMenu.border)

        self.itemMenu.cursorPos = max(0, min(self.itemMenu.cursorPos, len(stats.inventory) - 1))

    def updateEquipWindow(self):
        char = self.curChar

        if controls.left() and self.charIdx > 0:
            self.charIdx -= 1
            self.refresh(self.curChar)

        if controls.right() and self.charIdx < len(stats.activeRoster) - 1:
            self.charIdx += 1
            self.refresh(self.curChar)

        oldPos = self.equipMenu.cursorPos
        eqtype = self.curEquipType()

        result = self.equipMenu.update()

        if result is None:
            eqtype2 = self.curEquipType()
            # rehighlight the item list so that items that this character can equip in this slot are highlighted.
            if eqtype != eqtype2:
                self.itemList.rehighlight(lambda i: char.canEquip(i.name) and i.type == eqtype2)

            i = char.equipment[self.equipMenu.cursorPos].item
            self.setDescription(i and i.desc or '')

        if result is Cancel or result is None:
            return result

        if len(stats.inventory) > 0:
            self.slotIdx = result
            self.state = self.updateItemWindow

            self.equipMenu.cursor, self.itemMenu.cursor = self.itemMenu.cursor, self.equipMenu.cursor
        return None

    def updateItemWindow(self):
        oldPos = self.itemMenu.cursorPos

        result = self.itemMenu.update()

        i = stats.inventory[self.itemMenu.cursorPos].item
        self.setDescription(i and i.desc or '')

        if self.itemMenu.cursorPos != oldPos:
            self.refreshStatPreview()

        if result == None:
            return None
        elif result == Cancel:
            ika.Input.Unpress()
            self.state = self.updateEquipWindow
            self.equipMenu.cursor, self.itemMenu.cursor = self.itemMenu.cursor, self.equipMenu.cursor

            # Clear the stat preview thing.
            s = self.curChar.stats
            self.statWindow.refreshItemColumn(s, s)
            return None

        # actually change equipment here
        char = self.curChar
        selecteditem = stats.inventory[self.itemMenu.cursorPos].item
        slot = char.equipment[self.equipMenu.cursorPos].type

        if char.canEquip(selecteditem.name) and slot == selecteditem.type:
            char.equip(selecteditem.name, self.equipMenu.cursorPos)

            self.refresh(stats.activeRoster[self.charIdx])

        self.refreshStatPreview()

        return None

    def execute(self):

        def draw():
            ika.Map.Render()
            self.portraitWindow.draw()
            self.statWindow.draw()
            self.description.draw()
            self.equipMenu.draw()
            self.itemMenu.draw()

        self.state = self.updateEquipWindow
        time = ika.GetTime()

        fps = FPSManager()

        while True:
            result = self.state()
            if controls.cancel():
                break
            if result is not None:
                break

            fps.render(draw)

        return True
