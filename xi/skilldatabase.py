# skill module for the xi library
# coded by Andy Friesen
# copyright whenever.  All rights reserved.
#
# This source code may be used for any purpose, provided that
# the original author is never misrepresented in any way.
#
# There is no warranty, express or implied on the functionality, or
# suitability of this code for any purpose.

from __future__ import generators

from token import TokenStream
from statelessproxy import StatelessProxy
from exception import XiException

import skill

class SkillDatabase(StatelessProxy):
    def __init__(self):
        StatelessProxy.__init__(self)

        if len(self.__dict__) != 0:
            return

        self.__skills = {}  # name:skill object

    def __getitem__(self, name):
        return self.__skills[name]

    def __iter__(self):
        return self.__skills.iteritems()

    def Init(self, filename, fieldeffects = None, battleeffects = None):
        def ParseSkill(f):
            i = skill.Skill()

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
                    if e not in skill.types:
                        raise XiException('Unknown skill type '+`e`)
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

                elif t == 'basic':          i.basic = True
                elif t == 'minlevel':       i.minlevel = int(f.Next())
                elif t == 'mp':             i.mp = int(f.Next())
                elif t == 'fieldeffect':    i.fieldeffect  = fieldeffects[f.Next()]
                elif t == 'battleeffect':   i.battleeffect = battleeffects[f.Next()]

                elif t == 'end':
                    break
                else:
                    raise XiException('Unknown skills.dat directive ' + `t`)

            return i

        file = TokenStream(filename)

        while not file.EOF():
            t = file.Next().lower()

            if t == 'name':
                i = ParseSkill(file)
                self.__skills[i.name] = i
            else:
                raise XiException('Unknown skills.dat token ' + `t`)
