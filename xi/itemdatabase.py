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
    def __init__(self, fname = 'items.dat'):
        StatelessProxy.__init__(self)

        if len(self.__dict__) != 0:
            return

        self.__items = {}  # name:item object
        self.LoadData(fname)

    def __getitem__(self, name):
        return self.__items[name]

    def __iter__(self):
        return self.__items.iteritems()

    def LoadData(self, filename):
        def ParseItem(f):
            i = item.Item()
                
            i.name = f.GetLine()
            
            while not f.EOF():
                t = f.Next().lower()
                
                if t == 'desc':
                    i.desc = f.GetLine()
                elif t == 'equiptype':
                    e = f.Next()
                    if not e in item.equiptypes:
                        raise XiException('Unknown equip type '+`e`)
                    i.equiptype = e
                elif t == 'equipby':
                    while 1:
                       s = f.Next().lower()
                       if s == 'end':
                               break
                       i.eqby.append(s)
                elif t == 'useby':
                    while 1:
                       s = f.Next().lower()
                       if s == 'end':
                               break
                       i.useby.append(s)
                elif t == 'consumable':
                    i.consumable = True
                elif t == 'cost':     i.cost = int(f.Next())

                elif t == 'atk':      i.atk = int(f.Next())
                elif t == 'def':      i.Def = int(f.Next())
                elif t == 'hit':      i.hit = int(f.Next())
                elif t == 'eva':      i.eva = int(f.Next())

                elif t == 'str':      i.str = int(f.Next())
                elif t == 'vit':      i.vit = int(f.Next())
                elif t == 'mag':      i.mag = int(f.Next())
                elif t == 'wil':      i.wil = int(f.Next())
                elif t == 'spd':      i.spd = int(f.Next())
                elif t == 'luk':      i.luk = int(f.Next())
                elif t == 'end':
                    break
                else:
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
