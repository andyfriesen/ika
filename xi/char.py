# standard Player Character class
# coded by Andy Friesen
# copyright whenever.  All rights reserved.
#
# This source code may be used for any purpose, provided that
# the original author is never misrepresented in any way.
#
# There is no warranty, express or implied on the functionality, or
# suitability of this code for any purpose.

from ika import *
import token
from exception import XiException
from item import *
from itemdatabase import ItemDatabase

itemdb = ItemDatabase()

class Character:
    def __init__(self, datfile):
        self.datfilename = datfile

        self.name = 'null'
        self.portrait = Image()
        self.chrfile = 'null'
        self.charclass = 'null'

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

        self.equip = {}
        for it in equiptypes:
            self.equip[it]=None

        self.CalcEquip()

    #--------------------------------------------------------------------

    def CanEquip(self, itemname):
        "Returns true if the character can equip the item"

        item = itemdb[itemname]

        if self.charclass in item.equipby:
            return True
        if 'all' in item.equipby:
            return True
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

    def Equip(self, itemname):
        "Equips the specified item."
        
        item = itemdb[itemname]
        
        if not item.equiptype in equiptypes:
            raise XiException('Invalid equipment type '+`item.equiptype`)

        # put what was equipped before back (if anything was there)
        self.Unequip(item.equiptype)

        self.equip[item.equiptype]=item
        self.CalcEquip()

    #--------------------------------------------------------------------

    def Unequip(self, slot):
        "Unequips the item in the specified slot"
        slot = slot.lower()
        if not slot in equiptypes:
            raise XiException('char.unequip: Invalid equip type specified.')

        self.equip[slot]=None

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
        self.eva = 0

        # equipment bonuses
        for equip in self.equip.values():
            if equip is None:
                continue
            self.str += equip.str
            self.vit += equip.vit
            self.mag += equip.mag
            self.wil += equip.wil
            self.spd += equip.spd
            self.luk += equip.luk

            self.atk += equip.atk
            self.Def += equip.Def
            self.hit += equip.hit
            self.eva += equip.eva

        # calculate the derived stats
        self.atk += self.str
        self.Def += self.vit
        self.eva = min(self.eva + self.spd * 4, 99)    # cap evade and hit# to 99%
        self.hit = min(self.hit, 99)                   # hit% is dependant PURELY on equipment

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
                self.portrait.Load(tokens.Next())
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
    #         elif t =="magic':
    #            self.ParseMagicList(tokens)

            elif t =='/*':                  # comment skipper
                t = tokens.Next()
                while t != '*/':
                    t = tokens.Next()
                continue

            else:
                Exit('Unknown '+datfile+' token, '+`t`)

    #--------------------------------------------------------------

    def Spawn(self, x, y):
        self.ent = Entity(x, y, self.chrfile)

#------------------------------------------------------------------
