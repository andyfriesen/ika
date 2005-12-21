#!/usr/bin/env python

"""Skill menu for xi."""

# Coded by Andy Friesen
# Copyright whenever.  All rights reserved.

# This source code may be used for any purpose, provided that the
# original author is never misrepresented in any way.

# There is no warranty, express or implied on the functionality, or
# suitability of this code for any purpose.

import ika

import xi.controls
import xi.menu
import xi.menuwindows
import xi.party
import xi.widget


class SkillMenu(object):
    def __init__(self, statbar):
        self.skillwindow = xi.menuwindows.SkillWindow()
        self.portraitwindow = xi.menuwindows.PortraitWindow()
        self.statwindow = xi.menuwindows.StatusWindow()
        self.statbar = statbar
        self.charidx = 0
        self.skillwindow.active = True
        self.description = xi.widget.TextFrame()
        self.description.AddText('')

    CurChar = property(lambda self: xi.party.party[self.charidx])

    def StartShow(self):
        self.Refresh(self.CurChar)
        
        trans.AddWindowReverse(self.portraitwindow, (-self.portraitwindow.width, self.portraitwindow.y))
        trans.AddWindowReverse(self.statwindow, (ika.Video.xres, self.statwindow.y))
        trans.AddWindowReverse(self.description, (self.description.x, -self.description.height))
        trans.AddWindowReverse(self.skillwindow, (self.skillwindow.x, ika.Video.yres))
        
    def StartHide(self):
        trans.AddWindow(self.portraitwindow, (ika.Video.xres, self.portraitwindow.y), remove=True)
        trans.AddWindow(self.statwindow, (-self.statwindow.width, self.statwindow.y), remove=True)
        trans.AddWindow(self.description, (self.description.x, -self.description.height), remove=True)
        trans.AddWindow(self.skillwindow, (self.skillwindow.x, ika.Video.yres), remove=True)

    def Refresh(self, char):
        for x in (self.portraitwindow, self.statwindow):
            x.Refresh(char)
        self.skillwindow.Refresh(char,
                                 lambda skill: (skill.fieldeffect is not None)
                                               and (skill.mp <= char.MP))
        if len(char.skills) > 0:
            self.skillwindow.active = True
        else:
            self.skillwindow.AddText('No skills')
            self.skillwindow.active = False
            self.skillwindow.CursorPos = 0
        # Layout
        self.portraitwindow.DockTop().DockLeft()
        self.statwindow.DockTop(self.portraitwindow).DockLeft()
        self.statwindow.width = self.portraitwindow.width
        self.description.DockTop().DockLeft(self.portraitwindow)
        self.description.Right = self.statbar.x - self.statbar.border * 2
        self.skillwindow.DockTop(self.description).DockLeft(self.portraitwindow)
        self.skillwindow.Right = self.statbar.x - self.statbar.border * 2
        self.skillwindow.Layout()
        self.statbar.Refresh()
        trans.Reset()
        
    def UpdateSkillWindow(self):
        if left() and self.charidx > 0:
            self.charidx -= 1
            self.Refresh(xi.party.party[self.charidx])
        if right() and self.charidx < len(xi.party.party) - 1:
            self.charidx += 1
            self.Refresh(xi.party.party[self.charidx])
        char = self.CurChar
        if self.skillwindow.CursorPos < len(char.skills):
            s = char.skills[self.skillwindow.CursorPos]
            self.description.Text[0] = s.desc
        else:
            self.description.Text[0] = ''
        result = self.skillwindow.Update()
        return result

    def Execute(self):
        self.charidx = 0
        self.Refresh(xi.party.party[self.charidx])
        curstate = self.UpdateSkillWindow
        while True:
            ika.Input.Update()
            ika.Map.Render()
            for x in (self.skillwindow, self.portraitwindow, self.statwindow, self.statbar, self.description):
                x.Draw()
            ika.Video.ShowPage()
            result = curstate()
            if xi.controls.cancel():
                break
            if result is None:
                continue
            elif result == xi.menu.Cancel:
                break
            else:
                skill = self.CurChar.skills[result]
                if skill.fieldeffect is not None and self.CurChar.MP >= skill.mp:
                    result = skill.fieldeffect(self.CurChar)
                    # if the effect wasn't cancelled somehow...
                    if result is None:
                        self.CurChar.MP -= skill.mp
                        self.Refresh(self.CurChar)
                else:
                    # battle-only?  Not useable at all? @_x
                    pass
        return True
