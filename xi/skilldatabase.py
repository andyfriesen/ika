#!/usr/bin/env python

"""Skill module for the xi library."""

# Coded by Andy Friesen.
# Copyright whenever.  All rights reserved.

# This source code may be used for any purpose, provided that the
# original author is never misrepresented in any way.

# There is no warranty, express or implied on the functionality, or
# suitability of this code for any purpose.

import xi
import xi.skill

from xi.token import TokenStream


class SkillDatabase(xi.StatelessProxy):

    def __init__(self):
        super(SkillDatabase, self).__init__()
        if self.__dict__:
            return
        # name: skill object
        self.__skills = {}

    def __getitem__(self, name):
        return self.__skills[name]

    def __iter__(self):
        return self.__skills.iteritems()

    def __len__(self):
        return len(self.__skills)

    def init(self, filename, fieldeffects=None, battleeffects=None):
        def parseSkill(f):
            i = xi.skill.Skill()
            i.name = f.GetLine()
            while not f.EOF():
                t = f.Next().lower()
                if t == 'desc':
                    s = f.GetLine()
                    while s != 'end':
                        i.desc += s
                        s = f.GetLine()
                elif t == 'category':
                    e = f.Next()
                    if e not in xi.skill.categories:
                        raise xi.XiException('Unknown skill category %s.' % e)
                    i.category = e
                elif t == 'leads_to':
                    s = f.GetLine()
                    while s != 'end':
                       i.leads_to.append(s)
                       s = f.GetLine()
                elif t == 'used_by':
                    s = f.Next().lower()
                    while s != 'end':
                       i.used_by.append(s)
                       s = f.Next().lower()
                elif t == 'target':
                    s = f.Next().lower()
                    while s != 'end':
                        i.target.append(s)
                        s = f.Next().lower()
                elif t == 'basic':
                    i.basic = True
                elif t == 'minimumlevel':
                    i.minimumlevel = int(f.Next())
                elif t == 'mp':
                    i.mp = int(f.Next())
                elif t == 'field_effect':
                    try:
                        effectName = f.Next()
                        i.field_effect  = fieldeffects.__dict__[effectName]
                    except KeyError:
                        raise xi.XiException('Unable to find field effect %s for skill %s.' % (effectName, i.name))
                elif t == 'battle_effect':
                    try:
                        effectName = f.Next()
                        i.battle_effect = battleeffects.__dict__[effectName]
                    except KeyError:
                        raise xi.XiException('Unable to find battle effect %s for skill %s.' % (effectName, i.name))
                elif t == 'end':
                    break
                else:
                    raise xi.XiException('Unknown skills.dat directive %s.' % t)
            return i
        file = TokenStream(filename)
        while not file.EOF():
            t = file.Next().lower()
            if t == 'name':
                i = parseSkill(file)
                self.__skills[i.name] = i
            else:
                raise xi.XiException('Unknown skills.dat token %s.' % t)
