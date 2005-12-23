# Standard Player Character class
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
from xi import token

from skill import Skill, SkillList
from statset import StatSet
from itemdatabase import ItemDatabase
from skilldatabase import SkillDatabase

import item
import xi.party


itemdb = ItemDatabase()
skilldb = SkillDatabase()


class _EquipSlot(object):
    __slots__ = ['category', 'item']
    def __init__(self, category, item = None):
        self.category = category        # the type of slot
        self.item = item        # the item in the slot, or None

    def __str__(self):
        return self.category + '\t' + self.item.name

    def __repr__(self):
        return self.__str__()

class Character(object):
    '''A player character and all the stats and skills it possesses.
    '''
    def __init__(self, datFile):
        self.datFileName = datFile

        self.name = 'null'
        self.portrait = None
        self.spriteName = 'null'
        self.charclass = 'null'
        self.equipment = [] # list of _EquipSlot objects

        self.initstats = StatSet()      # level 1 stats
        self.endstats = StatSet()       # level 99 stats (we interpolate)

        self.bio = 'Something interesting about me goes here.'

        self.loadDatFile(datFile)       # read values

        self.naturalstats = self.initstats.clone()
        self.stats = self.naturalstats.clone()
        self.hp = self.maxHP
        self.mp = self.maxMP

        self.level = 1
        self.stats.exp = self.initstats.exp
        self.expneeded = 1000

        self.ent = None

        self.skills = SkillList()

        self.calcEquip()

    # max(min(value, maximum), minimum) keeps value between maximum and
    # minimum, inclusive.  It looks scarier than it is.
    def set_HP(self, value):
        self.stats.hp = max(min(value, self.stats.maxhp), 0)
    def set_MP(self, value):
        self.stats.mp = max(min(value, self.stats.maxmp), 0)

    hp = property(lambda self: self.stats.hp, set_HP)
    mp = property(lambda self: self.stats.mp, set_MP)
    maxHP = property(lambda self: self.stats.maxhp)
    maxMP = property(lambda self: self.stats.maxmp)

    #--------------------------------------------------------------------

    def heal(self, amount):
        '''C.heal(amount) -- Restores some HP

        This automatically fails on dead characters, unlike directly
        setting HP.
        '''
        if self.hp > 0:
            self.hp = min(self.stats.maxhp, self.hp + amount)

    #--------------------------------------------------------------------

    def hurt(self, amount):
        "C.hurt(amount) -- Reduces HP by amount"
        self.hp = max(0, self.hp - amount)

    #--------------------------------------------------------------------

    def canEquip(self, itemname):
        "C.canEquip(itemName) -> bool -- True if the character can equip the item"

        #if type(item) is str:   # you can pass a string or an Item object
        item = itemdb[itemname]

        if self.charclass in item.equipby or 'all' in item.equipby:
            return True
        else:
            return False

    #--------------------------------------------------------------------

    def canUse(self, itemname):
        "C.canUse(itemName) -> bool -- Returns true if the character can use the item"

        item = itemdb[itemname]

        if self.charclass in item.useby or 'all' in item.useby:
            return True
        return False

    #--------------------------------------------------------------------

    def equip(self, itemname, slot = None):
        '''C.equip(itemName[, slotIndex]) -- Equips the given item

        If there's one in the group's inventory, it's taken from there.
        The currently equipped item is put in the inventory, if applicable.

        If slot is omitted, the first applicable slot in the character's equip
        list is chosen.
        '''

        item = itemdb[itemname]

        if slot is None:
            for i, e in enumerate(self.equipment):
                if e.category == item.category:
                    self.equip(itemname, i)
                    return
            return

        # put what was equipped before back (if anything was there)
        if self.equipment[slot].item:
            xi.party.inventory.give(self.equipment[slot].item.name)

        if xi.party.inventory.find(itemname) is not None:
            xi.party.inventory.take(itemname)

        self.equipment[slot].item = item
        self.calcEquip()

    #--------------------------------------------------------------------

    def unequip(self, slot):
        "C.unequip(slotIndex) -- Unequip the item in the specified slot"
        slot = slot.lower()
        if not slot in item.categories:
            raise xi.XiException('char.unequip: Invalid equip type specified.')

        self.equipment[slot] = None
        self.calcEquip()

    #--------------------------------------------------------------------

    def calcEquip(self):
        "C.calcEquip() -- Recalculate stats based on current equipment"
        # base stats

        # save HP/MP; we're about to clobber them
        hp = self.hp
        mp = self.mp

        self.stats = self.naturalstats.clone()

        # init the derived stats
        self.stats.eva = self.stats.spd * 4

        # equipment bonuses
        for equip in self.equipment:
            if equip.item is not None:
                self.stats += equip.item.stats

        # calculate the derived stats
        self.stats.atk += self.stats.str
        self.stats.grd += self.stats.vit

        # unclobber
        self.hp = hp
        self.mp = mp

    #--------------------------------------------------------------------

    def loadDatFile(self, datFile):
        '''C.loadDatFile(datFile -> File) -- Read statistics from a data file
        This is typically used internally; it should not be needed anywhere
        else, really.
        '''

        #----------------------------------

        def GetPairOfNumbers(tokens):
            s1 = tokens.Next()
            s2 = tokens.Next()
            if s2 == '-':
                s2 = tokens.Next()

            return (int(s1), int(s2))

        #----------------------------------

        try:
            tokens = token.TokenStream(datFile)
        except IOError:
            raise xi.Exception('Could not open %s' % datFile)

        while not tokens.EOF():
            t = tokens.Next().lower()

            if t == '':
                break

            elif t == 'name':
                self.name = tokens.GetLine()
            elif t == 'bio':
                b = []
                t = tokens.GetLine()
                while t != 'end':
                    b.append(t.strip())
                    t = tokens.GetLine()
                self.bio = '\n'.join(b)
            elif t == 'portrait':
                self.portrait = ika.Image(tokens.GetLine())
            elif t == 'sprite':
                self.spriteName = tokens.GetLine()
            elif t == 'class':
                self.charclass = tokens.Next().lower()
            elif t == 'exp':
                self.initstats.exp , self.endstats.exp = GetPairOfNumbers(tokens)
            elif t == 'hp':
                self.initstats.maxhp , self.endstats.maxhp  = GetPairOfNumbers(tokens)
                self.initstats.hp = self.initstats.maxhp
                self.endstats.hp = self.endstats.maxhp
            elif t == 'mp':
                self.initstats.maxmp , self.endstats.maxmp  = GetPairOfNumbers(tokens)
                self.initstats.mp = self.initstats.maxmp
                self.endstats.mp = self.endstats.maxmp
            elif t == 'str':
                self.initstats.str, self.endstats.str = GetPairOfNumbers(tokens)
            elif t == 'vit':
                self.initstats.vit, self.endstats.vit = GetPairOfNumbers(tokens)
            elif t == 'mag':
                self.initstats.mag, self.endstats.mag = GetPairOfNumbers(tokens)
            elif t == 'wil':
                self.initstats.wil, self.endstats.wil = GetPairOfNumbers(tokens)
            elif t == 'spd':
                self.initstats.spd, self.endstats.spd = GetPairOfNumbers(tokens)
            elif t == 'luk':
                self.initstats.luk, self.endstats.luk = GetPairOfNumbers(tokens)
            elif t == 'equipslots':
                s = tokens.Next().lower()
                while s != 'end':
                    self.equipment.append(_EquipSlot(s))
                    s = tokens.Next().lower()

            else:
                ika.Exit("Unknown %s token '%s'" % (datFile, `t`))

    #--------------------------------------------------------------

    def spawn(self, x, y, layerIndex = None):
        '''C.spawn(x, y[, layerIndex]) -> Entity -- Create an entity for the character.
        If the character already has an entity, it is reused and placed
        at the coordinates specified
        '''
        if self.ent is None:
            if layerIndex is None:
                layerIndex = 0

            self.ent = ika.Entity(x, y, layerIndex, self.spriteName)
            self.ent.name = self.name
            if self is xi.party.activeRoster[0]:
                ika.SetPlayer(self.ent)
            else:
                self.ent.isobs = False
        else:
            self.ent.x = x
            self.ent.y = y
            if layerIndex is not None:
                self.ent.layer = layer

        return self.ent

    def unSpawn(self):
        "C.unSpawn() -- Destroys the character's entity, if it exists."
        if self.ent is not None:
            self.ent.x, self.ent.y = -1000, -1000
            self.ent.visible = False
            self.ent = None

#------------------------------------------------------------------
