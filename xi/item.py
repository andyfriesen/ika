# item module for the xi library
# coded by Andy Friesen
# copyright whenever.  All rights reserved.
#
# This source code may be used for any purpose, provided that
# the original author is never misrepresented in any way.
#
# There is no warranty, express or implied on the functionality, or
# suitability of this code for any purpose.

from itemdatabase import ItemDatabase

# constants
equiptypes= [ 'weapon', 'shield', 'armour', 'accessory' ]

class Item(object):
    __slots__ = [ 'equiptype', 'name', 'desc', 'atk', 'Def', 'hit', 'eva',
                  'hp', 'mp', 'str', 'vit', 'mag', 'wil', 'spd', 'luk', 'consumable',
                  'fieldeffect', 'battleeffect', 'cost', 'equipby', 'useby' ]
    def __init__(self):
        self.equiptype = ''
        
        self.name = ''
        self.desc = ''
        
        # secondary stats.  Most normal equipment alters these.
        self.atk = 0
        self.Def = 0
        self.hit = 0
        self.eva = 0
        
        # basic stats.  Usually only altered by the more exotic equipment.
        self.hp = 0
        self.mp = 0
        self.str = 0
        self.vit = 0
        self.mag = 0
        self.wil = 0
        self.spd = 0
        self.luk = 0
        
        self.consumable = 0

        self.fieldeffect = None
        self.battleeffect = None
        
        self.cost = 0
        
        self.equipby = []
        self.useby = []

#--------------------------------------------

class InventoryEntry(object):
    def __init__(self, item, qty):
        self.item = item
        self.qty = qty

    Name = property( lambda self: self.item.name )
    Description = property( lambda self: self.item.desc )

#--------------------------------------------

class Inventory(object):
    db = ItemDatabase()   # convenience only
    def __init__(self):
        self.items = []   # list of InventoryEntry objects
        
    #--------------------------------------------

    def __iter__(self):
        return iter(self.items)

    #--------------------------------------------

    def __getitem__(self,val):
        return self.items[val]

    #--------------------------------------------

    def Find(self, itemname):
        'Returns the InventoryEntry containing the specified item, or None.'

        for i in self.items:
            if i.Name == itemname:
                return i
                
        return None # item not found in inventory

    #--------------------------------------------

    def Give(self, itemname, qty=1):
        'Adds the specified number of an item to the inventory.'

        i = self.Find(itemname)       
        if i is not None:
            i.qty += qty
        else:
            i = InventoryEntry(self.db[itemname], qty)
            self.items.append(i)        
        
    #--------------------------------------------

    def Take(self, itemname, qty=1):
        'Removes the specified number of an item from the inventory.'
        
        i = self.Find(itemname)
        if i is None:
            return          # player doesn't have this item to begin with

        if i.qty<=qty:          # if we're taking them all away
            self.items.remove(i)    # just remove the inventory entry altogeather
        else:
            i.qty-=qty      # the player will have one or more left
            
    #--------------------------------------------
