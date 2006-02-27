# item module for the xi library
# Coded by Andy Friesen
# Copyright whenever.  All rights reserved.
#
# This source code may be used for any purpose, provided that
# the original author is never misrepresented in any way.
#
# There is no warranty, express or implied on the functionality, or
# suitability of this code for any purpose.

import xi.token
from xi.statelessproxy import StatelessProxy
from xi.exception import XiException

import item

class ItemDatabase(StatelessProxy):
    def __init__(self):
        StatelessProxy.__init__(self)

        if len(self.__dict__) != 0:
            return

        self.__items = {}  # name:item object

    def __getitem__(self, name):
        return self.__items[name]

    def __iter__(self):
        return self.__items.iteritems()

    def __len__(self):
        return len(self.__items)

    def init(self, file, fieldEffects = None, battleEffects = None):
        def parseItem(f):
            i = item.Item()
            stats = i.stats

            i.name = f.GetLine()

            while not f.EOF():
                t = f.Next().lower()

                if t == 'desc':
                    d = []
                    s = f.GetLine()
                    while s != 'end':
                        d.append(s.strip())
                        s = f.GetLine()

                    i.desc='\n'.join(d)

                elif t == 'type':
                    e = f.Next()
                    if e not in item.types:
                        raise XiException('Unknown item type '+`e`)
                    i.type = e

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

                elif t == 'consumable':     i.consumable = True
                elif t == 'cost':           i.cost = int(f.Next())
                elif t == 'fieldeffect':
                    if fieldEffects:
                        try:
                            effectName = f.Next()
                            i.fieldEffect  = getattr(fieldEffects, effectName)
                        except KeyError:
                            raise XiException('Unable to find field effect %s for item %s' % (`effectName`, `i.name`))
                    else:
                        i.fieldEffect = f.Next()

                elif t == 'battleeffect':
                    if battleEffects:
                        try:
                            effectName = f.Next()
                            i.battleeeffect  = getattr(battleEffects, effectName)
                        except KeyError:
                            raise XiException('Unable to find battle effect %s for item %s' % (`effectName`, `i.name`))
                    else:
                        i.battleEffect = f.Next()

                elif t == 'hp':             stats.maxhp = int(f.Next()) # HP and MP stay 0 because equipping doesn't change them.  It changes the maximums.
                elif t == 'mp':             stats.maxmp = int(f.Next())
                elif t == 'atk':            stats.atk = int(f.Next())
                elif t == 'grd':            stats.grd = int(f.Next())
                elif t == 'hit':            stats.hit = int(f.Next())
                elif t == 'eva':            stats.eva = int(f.Next())

                elif t == 'str':            stats.str = int(f.Next())
                elif t == 'vit':            stats.vit = int(f.Next())
                elif t == 'mag':            stats.mag = int(f.Next())
                elif t == 'wil':            stats.wil = int(f.Next())
                elif t == 'spd':            stats.spd = int(f.Next())
                elif t == 'luk':            stats.luk = int(f.Next())
                elif t == 'end':
                    break
                else:
                    raise XiException('Unknown items.dat directive ' + `t`)

            return i

        file = xi.token.TokenStream(file)

        while not file.EOF():
            t = file.Next().lower()

            if t == 'name':
                i = parseItem(file)
                self.__items[i.name] = i
            else:
                raise XiException('Unknown items.dat token ' + `t`)
