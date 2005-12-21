#!/usr/bin/env python

"""Skill module for the xi library."""

# Coded by Andy Friesen.
# Copyright whenever.  All rights reserved.

# This source code may be used for any purpose, provided that the
# original author is never misrepresented in any way.

# There is no warranty, express or implied on the functionality, or
# suitability of this code for any purpose.

import xi
from xi.skilldatabase import SkillDatabase


class SkillCategory(object): pass
class AttackCategory(SkillCategory, xi.Singleton): pass
class DefenseCategory(SkillCategory, xi.Singleton): pass
class EnergyCategory(SkillCategory, xi.Singleton): pass
class ResistCategory(SkillCategory, xi.Singleton): pass
class RecoveryCategory(SkillCategory, xi.Singleton): pass
class AutomaticCategory(SkillCategory, xi.Singleton): pass

categories = [AttackCategory, DefenseCategory, EnergyCategory, ResistCategory,
              RecoveryCategory, AutomaticCategory]


class TargetType(object): pass
class TargetNone(TargetType, xi.Singleton): pass
class TargetSelf(TargetType, xi.Singleton): pass
class TargetAlly(TargetType, xi.Singleton): pass
class TargetAllAllies(TargetType, xi.Singleton): pass
class TargetEnemy(TargetType, xi.Singleton): pass
class TargetAll(TargetType, xi.Singleton): pass
class TargetCustom(TargetType, xi.Singleton): pass


targetTypes = [TargetNone, TargetSelf, TargetAlly, TargetAllAllies,
               TargetEnemy, TargetAllEnemies, TargetAll, TargetCustom]


class Skill(object):

    def __init__(self):
        # Skill category.
        self.category = 'none'
        # Name of the skill.
        self.name = ''
        # A brief description of the skill.
        self.description = ''
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
        self.target = TargetNone


class SkillList(object):

    # Convenience only.
    _db = SkillDatabase()

    def __init__(self):
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
