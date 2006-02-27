# item module for the xi library
# Coded by Andy Friesen
# Copyright whenever.  All rights reserved.
#
# This source code may be used for any purpose, provided that
# the original author is never misrepresented in any way.
#
# There is no warranty, express or implied on the functionality, or
# suitability of this code for any purpose.

import itemdatabase
from statset import *

# constants
types = [ 'weapon', 'armour', 'accessory', '?' ]

class Item(object):
    __slots__ = [
        'type',
        'name',
        'desc',
        'stats',        # StatSet holding the stat bonuses bestowed by this item, if it is a piece of equipment
        'consumable',
        'fieldEffect',
        'battleEffect',
        'cost',
        'equipby',
        'useby'
        ]

    def __init__(self):
        self.type = ''

        self.name = ''
        self.desc = ''

        self.stats = StatSet()

        self.consumable = False

        self.fieldEffect = None
        self.battleEffect = None

        self.cost = 0

        self.equipby = []
        self.useby = []

#--------------------------------------------

class InventoryEntry(object):
    def __init__(self, item, qty):
        self.item = item
        self.qty = qty

    name = property( lambda self: self.item.name )
    description = property( lambda self: self.item.desc )

#--------------------------------------------

class Inventory(object):
    db = None

    def __init__(self):
        if Inventory.db is None:
            Inventory.db = itemdatabase.ItemDatabase()   # convenience only

        self.items = []   # list of InventoryEntry objects

    #--------------------------------------------

    def __iter__(self):
        return iter(self.items)

    #--------------------------------------------

    def __getitem__(self,val):
        try:
            return self.items[val]
        except IndexError, ie:
            raise ie

    #--------------------------------------------

    def __len__(self):
        return len(self.items)

    #--------------------------------------------

    def __nonzero__(self):
        return True

    #--------------------------------------------

    def find(self, itemname):
        'Returns the InventoryEntry containing the specified item, or None.'

        for i in self.items:
            if i.name == itemname:
                return i

        return None # item not found in inventory

    #--------------------------------------------

    def give(self, itemname, qty=1):
        'Adds the specified number of an item to the inventory.'

        i = self.find(itemname)
        if i is not None:
            i.qty += qty
        else:
            i = InventoryEntry(self.db[itemname], qty)
            self.items.append(i)

    #--------------------------------------------

    def take(self, itemname, qty=1):
        'Removes the specified number of an item from the inventory.'

        i = self.find(itemname)
        if i is None:
            return          # player doesn't have this item to begin with

        if i.qty<=qty:          # if we're taking them all away
            self.items.remove(i)    # just remove the inventory entry altogeather
        else:
            i.qty-=qty      # the player will have one or more left

    #--------------------------------------------
