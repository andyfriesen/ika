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

    def __init__(self, hp = 0, mp = 0, atk = 0, grd = 0, hit = 0, eva = 0, str = 0, vit = 0, mag = 0, wil = 0, spd = 0, luk = 0, exp = 0):
        self.hp    = hp
        self.mp    = mp
        self.maxhp = hp
        self.maxmp = mp
        self.atk   = atk
        self.grd   = grd
        self.hit   = hit
        self.eva   = eva
        self.str   = str
        self.vit   = vit
        self.mag   = mag
        self.wil   = wil
        self.spd   = spd
        self.luk   = luk
        self.exp   = exp

    def Clone(self):
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

        return self

    def __add__(self, rhs):
        r = self.Clone()
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

        return self

    def __sub__(self, rhs):
        r = self.Clone()
        r -= rhs
        return r
        
    def __repr__(self):
        return `self.__dict__`
