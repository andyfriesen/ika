#!/usr/bin/env python

"""Skill module for the xi library."""

# Coded by Andy Friesen.
# Copyright whenever.  All rights reserved.

# This source code may be used for any purpose, provided that the
# original author is never misrepresented in any way.

# There is no warranty, express or implied on the functionality, or
# suitability of this code for any purpose.

import xi
import xi.skilldatabase


categories = ['none', 'attack', 'defense', 'energy', 'resist', 'recovery',
              'automatic']
targetTypes = ['none', 'self', 'ally', 'all allies', 'enemy', 'all enemies',
               'all', 'custom']

#class SkillCategory(object):
#    def __str__(cls):
#        return cls.name
#    __str__ = classmethod(__str__)
#
#class AttackCategory(SkillCategory): __metaclass__ = xi.Singleton; name = 'attack'
#class DefenseCategory(SkillCategory): __metaclass__ = xi.Singleton; name = 'defense'
#class EnergyCategory(SkillCategory): __metaclass__ = xi.Singleton; name = 'energy'
#class ResistCategory(SkillCategory): __metaclass__ = xi.Singleton; name = 'resist'
#class RecoveryCategory(SkillCategory): __metaclass__ = xi.Singleton; name = 'recovery'
#class AutomaticCategory(SkillCategory): __metaclass__ = xi.Singleton; name = 'automatic'
#
#categories = {}
#for category in [AttackCategory, DefenseCategory, EnergyCategory, ResistCategory,
#                 RecoveryCategory, AutomaticCategory]:
#    categories[str(category)] = category


#class TargetType(object): pass
#class TargetNone(TargetType): __metaclass__ = xi.Singleton
#class TargetSelf(TargetType): __metaclass__ = xi.Singleton
#class TargetAlly(TargetType): __metaclass__ = xi.Singleton
#class TargetAllAllies(TargetType): __metaclass__ = xi.Singleton
#class TargetEnemy(TargetType): __metaclass__ = xi.Singleton
#class TargetAllEnemies(TargetType): __metaclass__ = xi.Singleton
#class TargetAll(TargetType): __metaclass__ = xi.Singleton
#class TargetCustom(TargetType): __metaclass__ = xi.Singleton
#
#
#targetTypes = [TargetNone, TargetSelf, TargetAlly, TargetAllAllies,
#               TargetEnemy, TargetAllEnemies, TargetAll, TargetCustom]


class Skill(object):

    def __init__(self):
        # Skill category.
        self.category = 'none'
        # Name of the skill.
        self.name = ''
        # A brief description of the skill.
        self.desc = ''
        # Skills that can be learned by using this one.
        self.leads_to = []
        # If true, the skill has no pre-requisite skills.
        self.basic = False
        # Function to call when the skill is used on the field.
        self.field_effect = None
        # Function called when the skill is used in battle.
        self.battle_effect = None
        # The minimum level required to be able to learn this skill.
        self.minimumlevel = 0
        # MP required to use this skill.
        self.mp = 0
        # List of names of people who can use this skill.
        self.used_by = []
        # Description of what this skill can target.
        self.target = []


class SkillList(object):

    _db = None

    def __init__(self):
        if SkillList._db is None:
            # Convenience only.
            SkillList._db = xi.skilldatabase.SkillDatabase()
        # List of skill objects.
        self.skills = []

    def __iter__(self):
        return iter(self.skills)

    def __getitem__(self, value):
        return self.skills[value]

    def __len__(self):
        return len(self.skills)
    
    def find(self, skillname):
        """Returns the skill with skillname or None."""
        for skill in self.skills:
            if skill.name == skillname:
                return skill
        return None

    def add(self, skillname):
        """Adds the skill with skillname to the skill list."""
        i = self.Find(skillname)
        if i is None:
            i = self._db[skillname]
            self.skills.append(i)

    def remove(self, skillname):
        """Removes the skill with skillname from the skill list."""
        i = self.Find(skillname)
        if i is not None:
            self.skills.remove(i)
