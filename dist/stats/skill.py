# skill module for the xi library
# Coded by Andy Friesen
# Copyright whenever.  All rights reserved.
#
# This source code may be used for any purpose, provided that
# the original author is never misrepresented in any way.
#
# There is no warranty, express or implied on the functionality, or
# suitability of this code for any purpose.

import skilldatabase

# constants
types = [ 'attack', 'defense', 'energy', 'resist', 'recovery', 'automatic' ]
targetTypes = [ 'self', 'ally', 'all allies', 'enemy', 'all enemies', 'all', 'custom' ]

class Skill(object):
    __slots__ = [
        'type',         # the category to stuff the skill into.  One of the above.
        'name',         # name of the skill
        'desc',         # a brief description of the skill
        'leadsto',      # skills that can be learned by using this one
        'basic',        # if true, the skill has no pre-requisite skills
        'minlevel',     # The minimum level required to be able to learn this skill
        'mp',           # MP required to use this skill
        'fieldEffect',  # Function to call when the skill is used on the field
        'battleEffect', # Function called when the skill is used in battle
        'useby',        # List of names of people who can use this skill
        'target',       # list of strings describing how this skill can be targetted (see targetTypes above)
        ]

    def __init__(self):
        type = ''

        self.name = ''
        self.desc = ''

        self.leadsto = []

        self.basic = False

        self.fieldEffect = None
        self.battleEffect = None

        self.minlevel = 0
        self.mp = 0

        self.useby = []
        self.target = []

#-------------------------------------------

_db = skilldatabase.SkillDatabase() # convenience alias

class SkillList(object):
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

    def find(self, skillname):
        'Returns the skill with skillname or None.'

        for s in self.skills:
            if s.name == skillname:
                return s
        return None

    #--------------------------------------------

    def add(self, skillname):
        'Adds the skill with skillname to the skill list.'

        i = self.find(skillname)
        if i is None:
            i = _db[skillname]
            self.skills.append(i)

    #--------------------------------------------

    def remove(self, skillname):
        'Removes the skill with skillname from the skill list.'

        i = self.find(skillname)
        if i is not None:
            self.skills.remove(i)

    #--------------------------------------------
