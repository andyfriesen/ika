# Commonly used windows for the pi menu system.
# Coded by Andy Friesen
# Copyright whenever.  All rights reserved.
#
# This source code may be used for any purpose, provided that
# the original author is never misrepresented in any way.
#
# There is no warranty, express or implied on the functionality, or
# suitability of this code for any purpose.

import ika

import xi
import xi.party
from xi import gui
from xi import menu
from xi import layout
from xi.misc import *


class StatusBar(gui.StaticText):
    'Displays HP/MP counts for the party in a vertical status bar thing.'

    def __init__(self):
        super(StatusBar, self).__init__()

    def refresh(self):
        self.clear()
        for i, char in enumerate(xi.party.activeRoster):
            # Red if zero, yellow if less than a 4th max, and white otherwise.
            c = char.hp == 0 and '4' or char.hp < char.maxHP / 4 and '3' or '0'
            d = char.mp == 0 and '4' or char.mp < char.maxMP / 4 and '3' or '0'

            self.addText( char.name )
            self.addText( 'HP\t~%c%i~0/~%c%i' % (c, char.hp, c, char.maxHP) )
            self.addText( 'MP\t~%c%i~0/~%c%i' % (d, char.mp, d, char.maxMP) )

            if i + 1 < len(xi.party.activeRoster):
                self.addText( '' )

        self.autoSize()

class ShopEquipBar(gui.StaticText):
    "Displays the character's equipability stats for a certain item in the shop."

    def __init__(self, *args, **kw):
        gui.StaticText.__init__(self, *args, **kw)
        self.width = self.font.StringWidth('Owned:')

    def refresh(self, item=None):
        self.clear()
        if item is None:
            return              # probably not a good longterm thing

        invItem = xi.party.inventory.find(item.name)
        if invItem is not None:
            count = invItem.qty
        else:
            count = 0

        self.addText('Owned:')
        self.addText(str(count))

        if item.equipby:        # a piece of equipment?
            self.addText('')
            for char in xi.party.activeRoster:
                color = 2
                type = "No Use"
                if item is not None:
                    if char.canEquip(item.name):
                        slot = [c for c in char.equipment if c.type == item.type]
                        if slot and slot[0]:
                            slot = slot[0]      # assume the first slot that matches
                            if item.type == "weapon":
                                diff = item.stats.atk - slot.item.stats.atk
                                color = [4,0,6][clamp(diff,-1,1)+1]
                                type = '%i->%i' % (slot.item.stats.atk, item.stats.atk)
                                if diff == 0:  type = "Same"
                            elif item.type == "armour":
                                diff = item.stats.grd - slot.item.stats.grd
                                color = [4,0,6][clamp(diff,-1,1)+1]
                                type = '%i->%i' % (slot.item.stats.grd, item.stats.grd)
                                if diff == 0:  type = "Same"
                            else:
                                color = 5
                                type = "Use"
                                if item == slot.item:  type = "Same"

                self.addText( "~%i%s" % (color, char.name) )
                self.addText( "~%i%s" % (color, type) )
                self.addText( "" )

        self.autoSize()

class MiscWindow(gui.StaticText, xi.StatelessProxy):
    "Displays miscellaneous party stats such as gold and gameplay time."

    def __init__(self):
        super(type(self), self).__init__()

        self.addText('', '') # make sure we have two lines
        self.autoSize()

    def autoSize(self):
        self.width = max(
            self.font.StringWidth(xi.party.CURRENCY_FORMAT + '999999999'),        # one BILLION dollars!
            self.font.StringWidth('99:99:99'))
        self.height = self.font.height * 2 # two lines

    def draw(self, xoffset = 0, yoffset = 0):
        self.text[0] = xi.party.formatCurrency(stats.getMoney())
        t = xi.party.getGameTime()
        colon = (t / 50 & 1) and '~2:~0' or ':'
        self.text[1] = 'T %s' % formatTime(t, colon)

        y = self.y + yoffset
        x = self.x + self.width + xoffset
        for t in self.text:
            self.font.RightPrint(x, y, t)
            y += self.font.height

    def refresh(self):
        self.autoSize()

class PortraitWindow(gui.Frame):
    "Displays the character's portrait, HP, MP, and experience totals."

    def __init__(self):
        super(type(self), self).__init__()

    def refresh(self, char):
        self.client.removeAllChildren()
        portrait = gui.Picture(image=char.portrait)

        text = gui.StaticText(text=[
            char.name,
            'Lv %i' % char.level,
            'HP\t%i/%i' % (char.hp, char.maxHP),
            'MP\t%i/%i' % (char.mp, char.maxMP),
        ])

        t2 = gui.StaticText(text=[
            'Exp\t%i' % char.stats.exp,
            'Next\t%i' % (char.expneeded - char.stats.exp)
        ])

        portrait.position = (0, 0)
        text.position = (portrait.right, 0)
        t2.position = (0, portrait.bottom + 2)

        self.client.addChildren(portrait, text, t2)
        text.autoSize()
        t2.autoSize()
        self.autoSize()

class StatusWindow(gui.ColumnedTextLabel):
    "Displays a character's stats in a frame."

    def __init__(self):
        super(type(self), self).__init__(columns=3, pad=8)

    def refresh(self,char):
        self.clear()

        def add(n, a):
            if isinstance(a, int):
                a = '%3i' % a

            self.addText(n, a, '')

        stats = char.stats
        nstats = char.naturalstats

        add('Attack', stats.atk)
        add('Guard', stats.grd)
        add('Hit %', stats.hit)
        add('Evade %', stats.eva)
        add('', '')

        def add(n, a, b):
            self.addText(n, '%3i' % a, '(~2%3i~0)' % b)

        add('Strength', stats.str, nstats.str)
        add('Vitality', stats.vit, nstats.vit)
        add('Magic', stats.mag, nstats.mag)
        add('Will', stats.wil, nstats.wil)
        add('Speed', stats.spd, nstats.spd)
        add('Luck', stats.luk, nstats.luk)

        self.autoSize()

class EquipWindow(gui.ColumnedTextLabel):
    "Displays a character's current equipment."

    def __init__(self):
        super(EquipWindow, self).__init__(columns=2, pad=8)

    def refresh(self, char):
        self.clear()

        for e in char.equipment:
            i = e.item
            self.addText(e.type.capitalize() + ':', i and i.name or '')

        self.autoSize()

class SkillWindow(gui.ColumnedTextLabel):
    "Displays a character's skills."

    def __init__(self):
        super(type(self), self).__init__(columns=3, pad=8)
        self.columnWeights = [1.0, 0, 0]

    def refresh(self, char, condition = lambda s: True):
        self.clear()
        for s in char.skills:
            c = (condition(s) and '~0' or '~2')
            self.addText(c + s.name, c + s.type.capitalize(), c + '%3i' % s.mp)

        if len(char.skills) == 0:
            self.addText('~2No skills', '', '')

        self.autoSize()

class InventoryWindow(gui.ColumnedTextLabel):
    "Displays the group's inventory."

    def __init__(self, inv):
        gui.ColumnedTextLabel.__init__(self, columns=3, pad=8)
        self._inventory = inv
        self.columnWeights = [1.0, 0, 0]

    def getInventory(self):
        return self._inventory
    inventory = property(getInventory)

    def rehighlight(self, condition = lambda i: True):
        for i, item in enumerate(xi.party.inventory):
            c = condition(item.item)

            self.rehighlightItem(i, c)

    def refresh(self, condition = lambda i: True):
        self.clear()
        for i in self.inventory:
            self.addItem(i, condition(i.item))
        self.autoSize()

    # ------ interface ------
    # override these methods to change how the window displays items

    def addItem(self, inventorySlot, highlight = True):
        c = highlight and '~0' or '~2'

        # show an x15 or whatever, but only if there's more than one
        if inventorySlot.qty > 1:
            qtyString = 'x' + str(inventorySlot.qty)
        else:
            qtyString = ''

        self.addText(
            c + inventorySlot.item.name,
            c + inventorySlot.item.type.capitalize(),
            c + qtyString)

    def rehighlight(self, condition = lambda i: True):
        for i in range(len(self.inventory)):
            self.rehighlightItem(i, condition(self.inventory[i].item))

    # This doesn't rely on the number of columns, only that each element
    # in each column begins with a font subset command.
    # You shouldn't need to override this unless you need to do something different.
    def rehighlightItem(self, index, highlight):
        # White if True, gray if False
        c = highlight and '~0' or '~2'

        # cheating. :x
        for col in self._columns:
            # Since every item begins with a font subset command,
            # it's simply a matter of replacing the first two characters
            # of every element in the column
            col.text[index] = c + col.text[index][2:]

class ShopWindow(InventoryWindow):
    def __init__(self, inv):
        InventoryWindow.__init__(self, inv)

    def addItem(self, inventorySlot, highlight):
        c = highlight and '~0' or '~2'

        self.addText(
            c + inventorySlot.item.name,
            c + inventorySlot.item.type.capitalize(),
            c + xi.party.formatCurrency(inventorySlot.item.cost).rjust(7)
            )
