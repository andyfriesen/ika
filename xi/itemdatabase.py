# item module for the xi library
# coded by Andy Friesen
# copyright whenever.  All rights reserved.
#
# This source code may be used for any purpose, provided that
# the original author is never misrepresented in any way.
#
# There is no warranty, express or implied on the functionality, or
# suitability of this code for any purpose.

from __future__ import generators

from token import TokenStream
from statelessproxy import StatelessProxy
from exception import XiException

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

    def Init(self, filename, fieldeffects = None, battleeffects = None):
        def ParseItem(f):
            i = item.Item()
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
                    if e not in item.equiptypes:
                        raise XiException('Unknown equip type '+`e`)
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
                       
                elif t == 'consumable':     i.consumable = True
                elif t == 'cost':           i.cost = int(f.Next())
                elif t == 'fieldeffect':
                    try:
                        effectName = f.Next()
                        i.fieldeffect  = fieldeffects.__dict__[effectName]
                    except KeyError:
                        raise XiException('Unable to find field effect %s for item %s' % (`effectName`, `i.name`))
                elif t == 'battleeffect':
                    try:
                        effectName = f.Next()
                        i.battleeeffect  = battleeffects.__dict__[effectName]
                    except KeyError:
                        raise XiException('Unable to find battle effect %s for item %s' % (`effectName`, `i.name`))

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
                    print f.Next()
                    raise XiException('Unknown items.dat directive ' + `t`)
                            
            return i
                
        file = TokenStream(filename)
        
        while not file.EOF():
            t = file.Next().lower()
            
            if t == 'name':
                i = ParseItem(file)
                self.__items[i.name] = i
            else:
                raise XiException('Unknown items.dat token ' + `t`)
