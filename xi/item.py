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
from statset import *

# constants
equiptypes= [ 'weapon', 'armour', 'accessory' ]

class Item(object):
    __slots__ = [
        'equiptype',
        'name',
        'desc',
        'stats',        # StatSet holding the stat bonuses bestowed by this item, if it is a piece of equipment
        'consumable',
        'fieldeffect',
        'battleeffect',
        'cost',
        'equipby',
        'useby'
        ]
    
    def __init__(self):
        self.equiptype = ''
        
        self.name = ''
        self.desc = ''

        self.stats = StatSet()        
        
        self.consumable = False

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
        try:
            return self.items[val]
        except IndexError, ie:
            print val, len(self.items)
            raise ie

    #--------------------------------------------

    def __len__(self):
        return len(self.items)

    #--------------------------------------------

    def __nonzero__(self):
        return True

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
