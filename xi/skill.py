# skill module for the xi library
# coded by Andy Friesen
# copyright whenever.  All rights reserved.
#
# This source code may be used for any purpose, provided that
# the original author is never misrepresented in any way.
#
# There is no warranty, express or implied on the functionality, or
# suitability of this code for any purpose.

from skilldatabase import SkillDatabase

# constants
types = [ 'attack', 'defense', 'energy', 'resist', 'recovery', 'automatic' ]

class Skill(object):
    __slots__ = [ 'type', 'name', 'desc', 'leadsto', 'basic', 'minlevel', 'mp', 'fieldeffect', 'battleeffect', 'useby' ]
    def __init__(self):
        type = 'none'

        self.name = ''
        self.desc = ''

        self.leadsto = []

        self.basic = 0

        self.fieldeffect = None
        self.battleeffect = None

        self.minlevel = 0
        self.mp = 0

        self.useby = []

#-------------------------------------------

class SkillList(object):
    db = SkillDatabase()   # convenience only
    def __init__(self):
        self.skills = []   # list of skill objects

    #--------------------------------------------

    def __iter__(self):
        return iter(self.skills)

    #--------------------------------------------

    def __getitem__(self,val):
        return self.skills[val]

    #--------------------------------------------

    def __nonzero__(self):
        return True
    
    #--------------------------------------------

    def __len__(self):
        return len(self.skills)
    
    #--------------------------------------------

    def Find(self, skillname):
        'Returns the skill with skillname or None.'

        for s in self.skills:
            if s.name == skillname:
                return s
        return None

    #--------------------------------------------

    def Learn(self, skillname):
        'Adds the skill with skillname to the skill list.'

        i = self.Find(skillname)
        if i is None:
            i = self.db[skillname]
            self.skills.append(i)

    #--------------------------------------------

    def Forget(self, skillname):
        'Removes the skill with skillname from the skill list.'

        i = self.Find(skillname)
        if i is not None:
            self.skills.remove(i)    # just remove the skill altogeather

    #--------------------------------------------
