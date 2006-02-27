# Core statistics set class.
# Coded by Andy Friesen
# Copyright whenever.  All rights reserved.
#
# This source code may be used for any purpose, provided that
# the original author is never misrepresented in any way.
#
# There is no warranty, express or implied on the functionality, or
# suitability of this code for any purpose.

from xi.misc import clamp

class StatSet(object):
    __slots__ = [
        'hp',
        'mp',
        'maxhp',
        'maxmp',
        'atk',
        'grd',
        'hit',
        'eva',
        'str',
        'vit',
        'mag',
        'wil',
        'spd',
        'luk',
        'exp'
        ]

    def __init__(self, **kw):
        for stat in self.__slots__:
            setattr(self, stat, kw.get(stat, 0))

    def cap(self):
        # clamp HP/MP to 0-999 (inclusive)
        for attr in self.__slots__[:4]:
            setattr(self, attr,
                clamp(getattr(self, attr), 0, 9999)
                )

        # all other stats capped to 0-99
        for attr in self.__slots__[4:]:
            setattr(self, attr,
                clamp(getattr(self, attr), 0, 99)
                )

    def clone(self):
        r = StatSet()

        r.hp    = self.hp
        r.mp    = self.mp
        r.maxhp = self.maxhp
        r.maxmp = self.maxmp
        r.atk   = self.atk
        r.grd   = self.grd
        r.hit   = self.hit
        r.eva   = self.eva
        r.str   = self.str
        r.vit   = self.vit
        r.mag   = self.mag
        r.wil   = self.wil
        r.spd   = self.spd
        r.luk   = self.luk
        r.exp   = self.exp

        return r

    def __iadd__(self, rhs):
        self.hp    += rhs.hp
        self.mp    += rhs.mp
        self.maxhp += rhs.hp
        self.maxmp += rhs.mp
        self.atk   += rhs.atk
        self.grd   += rhs.grd
        self.hit   += rhs.hit
        self.eva   += rhs.eva
        self.str   += rhs.str
        self.vit   += rhs.vit
        self.mag   += rhs.mag
        self.wil   += rhs.wil
        self.spd   += rhs.spd
        self.luk   += rhs.luk
        self.exp   += rhs.exp

        self.cap()

        return self

    def __add__(self, rhs):
        r = self.clone()
        r += rhs
        return r

    def __isub__(self, rhs):
        self.hp    -= rhs.hp
        self.mp    -= rhs.mp
        self.maxhp -= rhs.hp
        self.maxmp -= rhs.mp
        self.atk   -= rhs.atk
        self.grd   -= rhs.grd
        self.hit   -= rhs.hit
        self.eva   -= rhs.eva
        self.str   -= rhs.str
        self.vit   -= rhs.vit
        self.mag   -= rhs.mag
        self.wil   -= rhs.wil
        self.spd   -= rhs.spd
        self.luk   -= rhs.luk
        self.exp   -= rhs.exp

        self.cap()

        return self

    def __sub__(self, rhs):
        r = self.clone()
        r -= rhs
        return r