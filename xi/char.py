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
    def __init__(_, type, item = None):
        _.type = type
        _.item = item
    
    def __str__(_):
        return _.type + '\t' + _.item.name
        
    def __repr__(_):
        return _.__str__()

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
            
    def __init__(_, datfile):
        _.datfilename = datfile

        _.name = 'null'
        _.portrait = ika.Image(ika.Canvas(16,16))
        _.chrfile = 'null'
        _.charclass = 'null'
        _.equip = [] # list of _EquipSlot objects

        _.initstats = StatSet()
        _.endstats = StatSet()

        _.LoadDatFile(datfile)

        _.naturalstats = _.initstats.Clone()
        _.stats = _.naturalstats.Clone()

        _.level = 1
        _.stats.exp = _.initstats.exp
        _.expneeded = 1000

        _.ent = None        

        _.skills = SkillList()        

        _.CalcEquip()

    # max(min(value, maximum), minimum) keeps value between maximum and
    # minimum, inclusive.  It looks scarier than it is.
    def set_HP(_, value):    _.stats.hp = max(min(value, _.stats.maxhp), 0)
    def set_MP(_, value):    _.stats.mp = max(min(value, _.stats.maxmp), 0)

    HP = property(lambda _: _.stats.hp, set_HP)
    MP = property(lambda _: _.stats.mp, set_MP)
    maxHP = property(lambda _: _.stats.maxhp)
    maxMP = property(lambda _: _.stats.maxmp)

    #--------------------------------------------------------------------

    def Heal(_, amount):
        if _.HP > 0:
            _.HP = min(_.stats.maxhp, _.HP + amount)

    #--------------------------------------------------------------------

    def Hurt(_, amount):
        _.HP = max(0, _.HP - amount)

    #--------------------------------------------------------------------

    def CanEquip(_, itemname):
        "Returns true if the character can equip the item"

        #if type(item) is str:   # you can pass a string or an Item object
        item = itemdb[itemname]
        
        if _.charclass in item.equipby or 'all' in item.equipby:
            return True
        else:
            return False

    #--------------------------------------------------------------------

    def CanUse(_, itemname):
        "Returns true if the character can use the item"

        item = itemdb.GetItem(itemname)

        if _.charclass in item.useby or 'all' in item.useby:
            return True
        return False

    #--------------------------------------------------------------------

    def SetEquip(_, itemname):
        """
        Adds the item to the character's current equipment.

        Whatever that item is replacing is destroyed.  The
        group's inventory is untouched.
        """
        item = itemdb[itemname]

        if not item.equiptype in equiptypes:
            raise XiException('Invalid equipment type '+`item.equiptype`)

        _.equip[item.equiptype] = item
        _.CalcEquip()

    #--------------------------------------------------------------------

    def Equip(_, itemname, slot = None):
        '''
        Equips the specified item.equipby

        If there's one in the group's inventory, it's taken from there.
        The currently equipped item is put in the inventory, if applicable.
        
        If slot is omitted, the first applicable slot in the character's equip
        list is chosen.
        '''

        item = itemdb[itemname]
        
        if slot is None:
            for i in range(len(_.equip)):
                if _.equip[i].type == item.equiptype:
                    _.Equip(itemname, i)
                    return
            return        
        
        # put what was equipped before back (if anything was there)
        if _.equip[slot].item:
            party.inv.Give(_.equip[slot].item.name)

        if party.inv.Find(itemname) is not None:
            party.inv.Take(itemname)
            
        _.equip[slot].item = item
        _.CalcEquip()

    #--------------------------------------------------------------------

    def Unequip(_, slot):
        "Unequips the item in the specified slot"
        slot = slot.lower()
        if not slot in equiptypes:
            raise XiException('char.unequip: Invalid equip type specified.')

        _.equip[slot]=None
        _.CalcEquip()

    #--------------------------------------------------------------------

    def CalcEquip(_):
        "Recalculates stats based on current equipment"
        # base stats

        _.stats = _.naturalstats.Clone()

        # init the derived stats
        _.stats.eva = _.stats.spd * 4

        # equipment bonuses
        for equip in _.equip:
            if equip.item is not None:
                _.stats += equip.item.stats

        # calculate the derived stats
        _.stats.atk += _.stats.str
        _.stats.grd += _.stats.vit
        _.stats.eva = min(_.stats.eva, 99)    # cap evade and hit# to 99%
        _.stats.hit = min(_.stats.hit, 99)

    #--------------------------------------------------------------------

    def LoadDatFile(_, datfile):

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

            if t == '':
                break
            
            elif t == 'name':
                _.name = tokens.Next()
            elif t == 'portrait':
                _.portrait = ika.Image(tokens.Next())
            elif t == 'chr':
                _.chrfile = tokens.Next()
            elif t == 'class':
                _.charclass = tokens.Next().lower()
            elif t == 'xp':
                _.initstats.exp , _.endstats.exp  = GetPairOfNumbers(tokens)
            elif t == 'hp':
                _.initstats.hp , _.endstats.hp  = GetPairOfNumbers(tokens)
            elif t == 'mp':
                _.initstats.mp , _.endstats.mp  = GetPairOfNumbers(tokens)
            elif t == 'str':
                _.initstats.str, _.endstats.str = GetPairOfNumbers(tokens)
            elif t == 'vit':
                _.initstats.vit, _.endstats.vit = GetPairOfNumbers(tokens)
            elif t == 'mag':
                _.initstats.mag, _.endstats.mag = GetPairOfNumbers(tokens)
            elif t == 'wil':
                _.initstats.wil, _.endstats.wil = GetPairOfNumbers(tokens)
            elif t == 'spd':
                _.initstats.spd, _.endstats.spd = GetPairOfNumbers(tokens)
            elif t == 'luk':
                _.initstats.luk, _.endstats.luk = GetPairOfNumbers(tokens)
            elif t == 'equipslots':
                s = tokens.Next().lower()
                while s != 'end':
                    _.equip.append(_EquipSlot(s))
                    s = tokens.Next().lower()

            else:
                ika.Exit('Unknown '+datfile+' token, '+`t`)

    #--------------------------------------------------------------

    def Spawn(_, x, y):
        if _.ent is None:
            _.ent = ika.Entity(x, y, _.chrfile)
            _.ent.name = _.name

#------------------------------------------------------------------
