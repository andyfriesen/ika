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
            
    def __init__(self, datfile):
        self.datfilename = datfile

        self.name = 'null'
        self.portrait = ika.Image(ika.Canvas(16,16))
        self.chrfile = 'null'
        self.charclass = 'null'
        self.equip = [] # list of _EquipSlot objects

        self.LoadDatFile(datfile)

        self.level = 1
        self.XP = self.initXP
        self.next = 1000

        self.HP = self.initHP
        self.MP = self.initMP
        self.maxHP = self.initHP
        self.maxMP = self.initMP

        # pre-equipment modified (natural) stats
        self.nstr = self.initSTR
        self.nvit = self.initVIT
        self.nmag = self.initMAG
        self.nwil = self.initWIL
        self.nspd = self.initSPD
        self.nluk = self.initLUK

        self.ent = None        

        self.skills = SkillList()        

        self.CalcEquip()

    #--------------------------------------------------------------------

    def Heal(self, amount):
        if self.HP > 0:
            self.HP = min(self.maxHP, self.HP + amount)

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

        if self.charclass in item.useby:
            return True
        if 'all' in item.useby:
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
        Equips the specified item.equipby

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

        self.equip[slot]=None
        self.CalcEquip()

    #--------------------------------------------------------------------

    def CalcEquip(self):
        "Recalculates stats based on current equipment"
        # base stats

        self.str = self.nstr
        self.vit = self.nvit
        self.mag = self.nmag
        self.wil = self.nwil
        self.spd = self.nspd
        self.luk = self.nluk

        # init the derived stats
        self.atk = 0
        self.Def = 0
        self.hit = 0
        self.eva = self.spd * 4

        # equipment bonuses
        for equip in self.equip:
            type = equip.type
            item = equip.item
            
            if item is None:
                continue
            #self.maxHP += item.hp
            #self.maxMP += item.mp
            self.str += item.str
            self.vit += item.vit
            self.mag += item.mag
            self.wil += item.wil
            self.spd += item.spd
            self.luk += item.luk

            self.atk += item.atk
            self.Def += item.Def
            self.hit += item.hit
            self.eva += item.eva

        # calculate the derived stats
        self.atk += self.str
        self.Def += self.vit
        self.eva = min(self.eva, 99)    # cap evade and hit# to 99%
        self.hit = min(self.hit, 99)

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
            return

        while not tokens.EOF():
            t = tokens.Next().lower()

            if t =='name':
                self.name = tokens.Next()
            elif t =='portrait':
                self.portrait = ika.Image(tokens.Next())
            elif t =='chr':
                self.chrfile = tokens.Next()
            elif t =='class':
                self.charclass = tokens.Next().lower()
            elif t =='xp':
                self.initXP , self.endXP  = GetPairOfNumbers(tokens)
            elif t =='hp':
                self.initHP , self.endHP  = GetPairOfNumbers(tokens)
            elif t =='mp':
                self.initMP , self.endMP  = GetPairOfNumbers(tokens)
            elif t =='str':
                self.initSTR, self.endSTR = GetPairOfNumbers(tokens)
            elif t =='vit':
                self.initVIT, self.endVIT = GetPairOfNumbers(tokens)
            elif t =='mag':
                self.initMAG, self.endMAG = GetPairOfNumbers(tokens)
            elif t =='wil':
                self.initWIL, self.endWIL = GetPairOfNumbers(tokens)
            elif t =='spd':
                self.initSPD, self.endSPD = GetPairOfNumbers(tokens)
            elif t =='luk':
                self.initLUK, self.endLUK = GetPairOfNumbers(tokens)
            elif t == 'equipslots':
                s = tokens.Next().lower()
                while s != 'end':
                    self.equip.append(_EquipSlot(s))
                    s = tokens.Next().lower()

            elif t =='/*':                  # comment skipper
                while t != '*/':
                    t = tokens.Next()

            else:
                Exit('Unknown '+datfile+' token, '+`t`)

    #--------------------------------------------------------------

    def Spawn(self, x, y):
        if self.ent is None:
            self.ent = ika.Entity(x, y, self.chrfile)

#------------------------------------------------------------------
