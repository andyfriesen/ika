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

    def __init__(_, hp = 0, mp = 0, atk = 0, grd = 0, hit = 0, eva = 0, str = 0, vit = 0, mag = 0, wil = 0, spd = 0, luk = 0, exp = 0):
        _.hp    = hp
        _.mp    = mp
        _.maxhp = hp
        _.maxmp = mp
        _.atk   = atk
        _.grd   = grd
        _.hit   = hit
        _.eva   = eva
        _.str   = str
        _.vit   = vit
        _.mag   = mag
        _.wil   = wil
        _.spd   = spd
        _.luk   = luk
        _.exp   = exp
               
    def Clone(_):
        r = StatSet()
        
        r.hp    = _.hp
        r.mp    = _.mp
        r.maxhp = _.hp
        r.maxmp = _.mp
        r.atk   = _.atk
        r.grd   = _.grd
        r.hit   = _.hit
        r.eva   = _.eva
        r.str   = _.str
        r.vit   = _.vit
        r.mag   = _.mag
        r.wil   = _.wil
        r.spd   = _.spd
        r.luk   = _.luk
        r.exp   = _.exp

        return r        

    def __iadd__(_, rhs):
        _.hp    += rhs.hp
        _.mp    += rhs.mp
        _.maxhp += rhs.hp
        _.maxmp += rhs.mp
        _.atk   += rhs.atk
        _.grd   += rhs.grd
        _.hit   += rhs.hit
        _.eva   += rhs.eva
        _.str   += rhs.str
        _.vit   += rhs.vit
        _.mag   += rhs.mag
        _.wil   += rhs.wil
        _.spd   += rhs.spd
        _.luk   += rhs.luk
        _.exp   += rhs.exp

        return _

    def __add__(_, rhs):
        r = _.Clone()
        r += rhs
        return r

