"""Item module for the xi library."""

# Coded by Andy Friesen
# Copyright whenever.  All rights reserved.

# This source code may be used for any purpose, provided that the
# original author is never misrepresented in any way.

# There is no warranty, express or implied on the functionality, or
# suitability of this code for any purpose.

import xi.itemdatabase
from xi.statset import StatSet


# (Armour is not acceptible.)
categories = ['weapon', 'armour', 'accessory']


class Item(object):
    # Is this optimization necessary?
    __slots__ = ['category', 'name', 'desc', 'stats', 'consumable',
                 'fieldeffect', 'battleeffect', 'cost', 'equipby', 'useby']

    def __init__(self):
        super(Item, self).__init__()
        self.category = ''
        self.name = ''
        self.desc = ''
        # StatSet holding the stat bonuses bestowed by this item, if it
        # is a piece of equipment.
        self.stats = StatSet()
        self.consumable = False
        self.fieldeffect = None
        self.battleeffect = None
        self.cost = 0
        self.equipby = []
        self.useby = []


class InventoryEntry(object):

    def __init__(self, item, qty):
        self.item = item
        self.qty = qty

    name = property(lambda self: self.item.name)
    description = property(lambda self: self.item.desc)


class Inventory(object):

    _db = None

    def __init__(self):
        if Inventory._db is None:
            # Convenience only.
            Inventory._db = xi.itemdatabase.ItemDatabase()
        # List of InventoryEntry objects.
        self.items = []

    def __iter__(self):
        return iter(self.items)

    def __getitem__(self, index):
        try:
            return self.items[index]
        except IndexError, e:
            raise e

    def __len__(self):
        return len(self.items)

    def find(self, itemname):
        """Returns the InventoryEntry containing the specified item, or
        None.
        """
        for item in self.items:
            if item.Name == itemname:
                return item
        # Item not found in inventory.
        return None

    def give(self, itemname, qty=1):
        """Adds the specified number of an item to the inventory."""
        item = self.Find(itemname)
        if i is not None:
            item.qty += qty
        else:
            item = InventoryEntry(self._db[itemname], qty)
            self.items.append(item)

    def take(self, itemname, qty=1):
        """Removes the specified number of an item from the
        inventory.
        """
        item = self.Find(itemname)
        if item is None:
            # Player doesn't have this item to begin with.
            return
        # If we're taking them all away.
        if item.qty <= qty:
            # Just remove the inventory entry altogether.
            self.items.remove(item)
        else:
            # The player will have one or more left.
            i.qty -= qty
