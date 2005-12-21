"""Item module for the xi library."""

# Coded by Andy Friesen
# Copyright whenever.  All rights reserved.

# This source code may be used for any purpose, provided that the
# original author is never misrepresented in any way.

# There is no warranty, express or implied on the functionality, or
# suitability of this code for any purpose.

from xi.itemdatabase import ItemDatabase
from xi.statset import StatSet


# Constants.  (Armour is not acceptible.)
equiptypes = ['weapon', 'armour', 'accessory']


class Item(object):
    __slots__ = ['equiptype', 'name', 'desc', 'stats', 'consumable',
                 'fieldeffect', 'battleeffect', 'cost', 'equipby', 'useby']

    def __init__(self):
        super(Item, self).__init__()
        self.equiptype = ''
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

    Name = property(lambda self: self.item.name)
    Description = property(lambda self: self.item.desc)


class Inventory(object):

    # Convenience only.
    db = ItemDatabase()

    def __init__(self):
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

    def __nonzero__(self):
        return True

    def Find(self, itemname):
        """Returns the InventoryEntry containing the specified item, or
        None.
        """
        for item in self.items:
            if item.Name == itemname:
                return item
        # Item not found in inventory.
        return None

    def Give(self, itemname, qty=1):
        """Adds the specified number of an item to the inventory."""
        item = self.Find(itemname)
        if i is not None:
            item.qty += qty
        else:
            item = InventoryEntry(self.db[itemname], qty)
            self.items.append(item)

    def Take(self, itemname, qty=1):
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
