# skill menu for xi
# coded by Andy Friesen
# copyright whenever.  All rights reserved.
#
# This source code may be used for any purpose, provided that
# the original author is never misrepresented in any way.
#
# There is no warranty, express or implied on the functionality, or
# suitability of this code for any purpose.

import ika

import party
import menu
import statusmenu
import widget

from menu import Menu

from menuwindows import *
from transition import *
from misc import *

class SkillMenu(object):
    def __init__(_, statbar):
        _.skillwindow = SkillWindow()
        _.portraitwindow = PortraitWindow()
        _.statwindow = StatusWindow()
        _.statbar = statbar
        _.charidx = 0
        _.skillwindow.active = True

        _.description = widget.TextFrame()
        _.description.AddText('')

    CurChar = property(lambda _: party.party[_.charidx])     

    def StartShow(_):
        _.Refresh(_.CurChar)
        
        trans.AddWindowReverse(_.portraitwindow, (-_.portraitwindow.width, _.portraitwindow.y))
        trans.AddWindowReverse(_.statwindow, (ika.Video.xres, _.statwindow.y))
        trans.AddWindowReverse(_.description, (_.description.x, -_.description.height))
        trans.AddWindowReverse(_.skillwindow, (_.skillwindow.x, ika.Video.yres))
        
    def StartHide(_):
        trans.AddWindow(_.portraitwindow, (ika.Video.xres, _.portraitwindow.y), remove = True)
        trans.AddWindow(_.statwindow, (-_.statwindow.width, _.statwindow.y), remove = True)
        trans.AddWindow(_.description, (_.description.x, -_.description.height), remove = True)
        trans.AddWindow(_.skillwindow, (_.skillwindow.x, ika.Video.yres), remove = True)

    def Refresh(_, char):
        for x in (_.portraitwindow, _.statwindow):
            x.Refresh(char)

        _.skillwindow.Refresh(char, lambda skill: (skill.fieldeffect is not None) and (skill.mp <= char.MP))
        if len(char.skills) > 0:
            _.skillwindow.active = True
        else:
            _.skillwindow.AddText('No skills')
            _.skillwindow.active = False
            _.skillwindow.CursorPos = 0

        # Layout
        _.portraitwindow.DockTop().DockLeft()
        _.statwindow.DockTop(_.portraitwindow).DockLeft()
        _.statwindow.width = _.portraitwindow.width
        _.description.DockTop().DockLeft(_.portraitwindow)
        _.description.Right = _.statbar.x - _.statbar.border * 2
        _.skillwindow.DockTop(_.description).DockLeft(_.portraitwindow)
        _.skillwindow.Right = _.statbar.x - _.statbar.border * 2
        _.statbar.Refresh()
        trans.Reset()
        
    def UpdateSkillWindow(_):
        if left() and _.charidx > 0:
            _.charidx -= 1
            _.Refresh(party.party[_.charidx])

        if right() and _.charidx < len(party.party) - 1:
            _.charidx += 1
            _.Refresh(party.party[_.charidx])

        char = _.CurChar
        if _.skillwindow.CursorPos < len(char.skills):
            s = char.skills[_.skillwindow.CursorPos]
            _.description.text[0] = s.desc
        else:
            _.description.text[0] = ''

        result = _.skillwindow.Update()
        
        return result

    def Execute(_):
        _.charidx = 0
        _.Refresh(party.party[_.charidx])

        curstate = _.UpdateSkillWindow

        while True:
            ika.Input.Update()
            
            ika.Map.Render()

            for x in (_.skillwindow, _.portraitwindow, _.statwindow, _.statbar, _.description):
                x.Draw()

            ika.Video.ShowPage()

            result = curstate()

            if cancel():
                break
            if result is None:
                continue
            elif result == -1:
                break
            else:
                skill = _.CurChar.skills[result]
                if skill.fieldeffect is not None and _.CurChar.MP >= skill.mp:
                    result = skill.fieldeffect(_.CurChar)
                    
                    # if the effect wasn't cancelled somehow...
                    if result is None:
                        _.CurChar.MP -= skill.mp
                        _.Refresh(_.CurChar)
                else:
                    pass # battle-only?  Not useable at all? @_x

        return True
