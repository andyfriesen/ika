#!/usr/bin/env python

"""Item module for the xi library."""

# Coded by Andy Friesen.
# Copyright whenever.  All rights reserved.

# This source code may be used for any purpose, provided that the
# original author is never misrepresented in any way.

# There is no warranty, express or implied on the functionality, or
# suitability of this code for any purpose.

import xi
import xi.item
from xi.token import TokenStream


class ItemDatabase(xi.StatelessProxy):

    def __init__(self):
        super(ItemDatabase, self).__init__()
        if len(self.__dict__) != 0:
            return
        # name:item object
        self._items = {}

    def __getitem__(self, name):
        return self._items[name]

    def __iter__(self):
        return self._items.iteritems()

    def __len__(self):
        return len(self._items)

    # We already have an init function--this should be something else.
    def init(self, filename, fieldeffects=None, battleeffects=None):
        def parseItem(f):
            i = xi.item.Item()
            stats = i.stats
            i.name = f.GetLine()
            while not f.EOF():
                t = f.Next().lower()
                if t == 'desc':
                    s = f.GetLine()
                    while s != 'end':
                        i.desc += s
                        s = f.GetLine()
                elif t == 'equiptype':
                    e = f.Next()
                    if e not in xi.item.equiptypes:
                        raise xi.XiException("Unknown equip type '%s'." % e)
                    i.equiptype = e
                elif t == 'equipby':
                    s = f.Next().lower()
                    while s != 'end':
                       i.equipby.append(s)
                       s = f.Next().lower()
                elif t == 'useby':
                    s = f.Next().lower()
                    while s != 'end':
                       i.useby.append(s)
                       s = f.Next().lower()
                elif t == 'consumable':
                    i.consumable = True
                elif t == 'cost':
                    i.cost = int(f.Next())
                elif t == 'fieldeffect':
                    try:
                        effectName = f.Next()
                        i.fieldeffect  = fieldeffects.__dict__[effectName]
                    except KeyError:
                        raise xi.XiException('Unable to find field effect %s for item %s.' % (effectName, i.name))
                elif t == 'battleeffect':
                    try:
                        effectName = f.Next()
                        i.battleeeffect  = battleeffects.__dict__[effectName]
                    except KeyError:
                        raise xi.XiException('Unable to find battle effect %s for item %s.' % (effectName, i.name))
                elif t == 'hp':
                    # HP and MP stay 0 because equipping doesn't change
                    # them.  It changes the maximums.
                    stats.maxhp = int(f.Next())
                elif t == 'mp':
                    stats.maxmp = int(f.Next())
                elif t == 'atk':
                    stats.atk = int(f.Next())
                elif t == 'grd':
                    stats.grd = int(f.Next())
                elif t == 'hit':
                    stats.hit = int(f.Next())
                elif t == 'eva':
                    stats.eva = int(f.Next())
                elif t == 'str':
                    stats.str = int(f.Next())
                elif t == 'vit':
                    stats.vit = int(f.Next())
                elif t == 'mag':
                    stats.mag = int(f.Next())
                elif t == 'wil':
                    stats.wil = int(f.Next())
                elif t == 'spd':
                    stats.spd = int(f.Next())
                elif t == 'luk':
                    stats.luk = int(f.Next())
                elif t == 'end':
                    break
                else:
                    raise xi.XiException("Unknown items.dat directive '%s'." % t)
            return i
        file = TokenStream(filename)
        while not file.EOF():
            t = file.Next().lower()
            if t == 'name':
                i = parseItem(file)
                self._items[i.name] = i
            else:
                raise xi.XiException("Unknown items.dat token '%s'" % t)
