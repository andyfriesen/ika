# standard Player Character class
# coded by Andy Friesen
# copyright whenever.  All rights reserved.
#
# This source code may be used for any purpose, provided that
# the original author is never misrepresented in any way.
#
# There is no warranty, express or implied on the functionality, or
# suitability of this code for any purpose.

import ika
import token
from exception import XiException
from item import *
from skill import *
from statset import *
from itemdatabase import ItemDatabase
from skilldatabase import SkillDatabase
import party

itemdb = ItemDatabase()
skilldb = SkillDatabase()

class _EquipSlot(object):
    __slots__ = ['type', 'item']
    def __init__(self, type, item = None):
        self.type = type
        self.item = item

    def __str__(self):
        return self.type + '\t' + self.item.name

    def __repr__(self):
        return self.__str__()

class Character(object):

    __slots__ = [
        'datfilename',
        'name',
        'portrait',
        'chrfile',
        'charclass',
        'equip',
        'skills',
        'level',
        'initstats',
        'endstats',
        'naturalstats',
        'stats',
        'expneeded',
        'ent'
        ]

    def __init__(self, datfile):
        self.datfilename = datfile

        self.name = 'null'
        self.portrait = None
        self.chrfile = 'null'
        self.charclass = 'null'
        self.equip = [] # list of _EquipSlot objects

        self.initstats = StatSet()
        self.endstats = StatSet()

        self.LoadDatFile(datfile)

        self.naturalstats = self.initstats.Clone()
        self.stats = self.naturalstats.Clone()
        self.HP = self.maxHP
        self.MP = self.maxMP

        self.level = 1
        self.stats.exp = self.initstats.exp
        self.expneeded = 1000        

        self.ent = None        

        self.skills = SkillList()        

        self.CalcEquip()

    # max(min(value, maximum), minimum) keeps value between maximum and
    # minimum, inclusive.  It looks scarier than it is.
    def set_HP(self, value):    self.stats.hp = max(min(value, self.stats.maxhp), 0)
    def set_MP(self, value):    self.stats.mp = max(min(value, self.stats.maxmp), 0)

    HP = property(lambda self: self.stats.hp, set_HP)
    MP = property(lambda self: self.stats.mp, set_MP)
    maxHP = property(lambda self: self.stats.maxhp)
    maxMP = property(lambda self: self.stats.maxmp)

    #--------------------------------------------------------------------

    def Heal(self, amount):
        if self.HP > 0:
            self.HP = min(self.stats.maxhp, self.HP + amount)

    #--------------------------------------------------------------------

    def Hurt(self, amount):
        self.HP = max(0, self.HP - amount)

    #--------------------------------------------------------------------

    def CanEquip(self, itemname):
        "Returns true if the character can equip the item"

        #if type(item) is str:   # you can pass a string or an Item object
        item = itemdb[itemname]
        
        if self.charclass in item.equipby or 'all' in item.equipby:
            return True
        else:
            return False

    #--------------------------------------------------------------------

    def CanUse(self, itemname):
        "Returns true if the character can use the item"

        item = itemdb.GetItem(itemname)

        if self.charclass in item.useby or 'all' in item.useby:
            return True
        return False

    #--------------------------------------------------------------------

    def SetEquip(self, itemname):
        """
        Adds the item to the character's current equipment.

        Whatever that item is replacing is destroyed.  The
        group's inventory is untouched.
        """
        item = itemdb[itemname]

        if not item.equiptype in equiptypes:
            raise XiException('Invalid equipment type '+`item.equiptype`)

        self.equip[item.equiptype] = item
        self.CalcEquip()

    #--------------------------------------------------------------------

    def Equip(self, itemname, slot = None):
        '''
        Equips the specified item

        If there's one in the group's inventory, it's taken from there.
        The currently equipped item is put in the inventory, if applicable.
        
        If slot is omitted, the first applicable slot in the character's equip
        list is chosen.
        '''

        item = itemdb[itemname]
        
        if slot is None:
            for i in range(len(self.equip)):
                if self.equip[i].type == item.equiptype:
                    self.Equip(itemname, i)
                    return
            return        
        
        # put what was equipped before back (if anything was there)
        if self.equip[slot].item:
            party.inv.Give(self.equip[slot].item.name)

        if party.inv.Find(itemname) is not None:
            party.inv.Take(itemname)
            
        self.equip[slot].item = item
        self.CalcEquip()

    #--------------------------------------------------------------------

    def Unequip(self, slot):
        "Unequips the item in the specified slot"
        slot = slot.lower()
        if not slot in equiptypes:
            raise XiException('char.unequip: Invalid equip type specified.')

        self.equip[slot] = None
        self.CalcEquip()

    #--------------------------------------------------------------------

    def CalcEquip(self):
        "Recalculates stats based on current equipment"
        # base stats

        hp = self.HP
        mp = self.MP

        self.stats = self.naturalstats.Clone()

        # init the derived stats
        self.stats.eva = self.stats.spd * 4

        # equipment bonuses
        for equip in self.equip:
            if equip.item is not None:
                self.stats += equip.item.stats

        # calculate the derived stats
        self.stats.atk += self.stats.str
        self.stats.grd += self.stats.vit
        self.stats.eva = min(self.stats.eva, 99)    # cap evade and hit# to 99%
        self.stats.hit = min(self.stats.hit, 99)

        # restore HP/MP, since the stats have since been nuked
        self.HP = hp
        self.MP = mp

    #--------------------------------------------------------------------

    def LoadDatFile(self, datfile):

        #----------------------------------

        def GetPairOfNumbers(tokens):
            s1 = tokens.Next()
            s2 = tokens.Next()
            if s2 == '-':
                s2 = tokens.Next()

            return (int(s1), int(s2))

        #---------------------------------- 
    
        try:
            tokens = token.TokenStream(datfile)
        except IOError:
            raise Exception('Could not open ' + datfile)

        while not tokens.EOF():
            t = tokens.Next().lower()

            if t == '':
                break

            elif t == 'name':
                self.name = tokens.Next()
            elif t == 'portrait':
                self.portrait = ika.Image(tokens.Next())
            elif t == 'chr':
                self.chrfile = tokens.Next()
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
                    self.equip.append(_EquipSlot(s))
                    s = tokens.Next().lower()

            else:
                ika.Exit('Unknown '+datfile+' token, '+`t`)

    #--------------------------------------------------------------

    def Spawn(self, x, y, layer = 0):
        if self.ent is None:
            self.ent = ika.Entity(x, y, layer, self.chrfile)
            self.ent.name = self.name
            if self is party.party[0]:
                ika.SetPlayer(self.ent)
            else:
                self.ent.isobs = False
        else:
            self.ent.x = x
            self.ent.y = y
            self.ent.layer = layer

#------------------------------------------------------------------
