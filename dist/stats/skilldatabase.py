# skill module for the xi library
# Coded by Andy Friesen
# Copyright whenever.  All rights reserved.
#
# This source code may be used for any purpose, provided that
# the original author is never misrepresented in any way.
#
# There is no warranty, express or implied on the functionality, or
# suitability of this code for any purpose.


from xi.token import TokenStream
from xi.statelessproxy import StatelessProxy
from xi.exception import XiException

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

    def init(self, filename, fieldEffects = None, battleEffects = None):
        import skill

        def parseSkill(f):
            i = skill.Skill()

            i.name = f.GetLine()

            while not f.EOF():
                t = f.Next().lower()

                if t == 'desc':
                    d = []
                    s = f.GetLine()
                    while s != 'end':
                        d.append(s.strip())
                        s = f.GetLine()

                    i.desc='\n'.join(d)

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

                elif t == 'target':
                    s = f.Next().lower()
                    while s != 'end':
                        i.target.append(s)
                        s = f.Next().lower()

                elif t == 'basic':          i.basic = True
                elif t == 'minlevel':       i.minlevel = int(f.Next())
                elif t == 'mp':             i.mp = int(f.Next())
                elif t == 'fieldeffect':
                    if fieldEffects:
                        try:
                            effectName = f.Next()
                            i.fieldEffect  = getattr(fieldEffects, effectName)
                        except KeyError:
                            raise XiException('Unable to find field effect %s for skill %s' % (`effectName`, `i.name`))
                    else:
                        i.fieldEffect = f.Next()

                elif t == 'battleeffect':
                    if battleEffects:
                        try:
                            effectName = f.Next()
                            i.battleEffect = getattr(battleEffects, effectName)
                        except KeyError:
                            raise XiException('Unable to find battle effect %s for skill %s' % (`effectName`, `i.name`))
                    else:
                        i.battleEffect = f.Next()

                elif t == 'end':
                    break
                else:
                    raise XiException('Unknown skills.dat directive ' + `t`)

            return i

        file = TokenStream(filename)

        while not file.EOF():
            t = file.Next().lower()

            if t == 'name':
                i = parseSkill(file)
                self.__skills[i.name] = i
            else:
                raise XiException('Unknown skills.dat token ' + `t`)
