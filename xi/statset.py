class StatSet(object):
    __slots__ = ['hp', 'mp', 'maxhp', 'maxmp', 'atk', 'grd', 'hit', 'eva',
                 'str', 'vit', 'mag', 'wil', 'spd', 'luk', 'exp']

    def __init__(self, hp=0, mp=0, atk=0, grd=0, hit=0, eva=0, str=0, vit=0,
                 mag=0, wil=0, spd=0, luk=0, exp=0):
        self.hp = hp
        self.mp = mp
        self.maxhp = hp
        self.maxmp = mp
        self.atk = atk
        self.grd = grd
        self.hit = hit
        self.eva = eva
        self.str = str
        self.vit = vit
        self.mag = mag
        self.wil = wil
        self.spd = spd
        self.luk = luk
        self.exp = exp

    def Clone(self):
        result = StatSet()
        for variable in self.__slots__:
            result.__dict__[variable] += self.__dict__[variable]
        return result        

    def __iadd__(self, rhs):
        for variable in self.__slots__:
            self.__dict__[variable] += other.__dict__[variable]
        return self

    def __add__(self, other):
        result = self.Clone()
        result += other
        return result

    def __isub__(self, rhs):
        for variable in self.__slots__:
            self.__dict__[variable] -= other.__dict__[variable]
        return self

    def __sub__(self, other):
        result = self.Clone()
        result -= other
        return result
        
    def __repr__(self):
        return repr(self.__dict__)
