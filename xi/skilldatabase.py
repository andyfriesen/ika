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

    def Init(self, filename, fieldeffects=None, battleeffects=None):
        def ParseSkill(f):
            i = xi.skill.Skill()
            i.name = f.GetLine()
            while not f.EOF():
                t = f.Next().lower()
                if t == 'desc':
                    s = f.GetLine()
                    while s != 'end':
                        i.desc += s
                        s = f.GetLine()
                elif t == 'type':
                    e = f.Next()
                    if e not in xi.skill.types:
                        raise xi.XiException('Unknown skill type %s.' % e)
                    i.type = e
                elif t == 'leadsto':
                    s = f.GetLine()
                    while s != 'end':
                       i.leadsto.append(s)
                       s = f.GetLine()
                elif t == 'useby':
                    s = f.Next().lower()
                    while s != 'end':
                       i.useby.append(s)
                       s = f.Next().lower()
                elif t == 'basic':
                    i.basic = True
                elif t == 'minlevel':
                    i.minlevel = int(f.Next())
                elif t == 'mp':
                    i.mp = int(f.Next())
                elif t == 'fieldeffect':
                    try:
                        effectName = f.Next()
                        i.fieldeffect  = fieldeffects.__dict__[effectName]
                    except KeyError:
                        raise xi.XiException('Unable to find field effect %s for skill %s.' % (effectName, i.name))
                elif t == 'battleeffect':
                    try:
                        effectName = f.Next()
                        i.battleeffect = battleeffects.__dict__[effectName]
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
                i = ParseSkill(file)
                self.__skills[i.name] = i
            else:
                raise xi.XiException('Unknown skills.dat token %s.' % t)
