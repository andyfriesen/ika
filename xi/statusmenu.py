# Status menu for xi
# coded by Andy Friesen
# copyright whenever.  All rights reserved.
#
# This source code may be used for any purpose, provided that
# the original author is never misrepresented in any way.
#
# There is no warranty, express or implied on the functionality, or
# suitability of this code for any purpose.

import ika
import widget
import mainmenu
import party
import equipmenu

from party import party

from transition import *

from menuwindows import StatusWindow, PortraitWindow, StatusBar, EquipWindow, SkillWindow
from misc import *


class StatusMenu(object):
    __slots__ = [
        'statbar',          # The status bar that sits at the right edge of the screen
        'portraitwindow',   # Window that contains the character's portrait, HP, MP, Level, and EXP
        'statwindow',       # Contains the character's strength, magic, et cetera.
        'equipwindow',      # Holds the character's current equipment.
        'skillwindow',      # Shows the skills that the character can use
        'charidx'           # Index of the current character
        ]
        
    def __init__(self, statbar):
        self.statbar = statbar
        self.portraitwindow = PortraitWindow()
        self.statwindow = StatusWindow()
        self.equipwindow = EquipWindow()
        self.skillwindow = SkillWindow()
        self.charidx = 0
        
    CurChar = property(lambda self: party[self.charidx])
    
    def StartShow(self):
        self.Refresh(self.CurChar)
        
        trans = Transition()
        trans.AddWindowReverse(self.portraitwindow, (-self.portraitwindow.width, self.portraitwindow.y))
        trans.AddWindowReverse(self.statwindow, (ika.Video.xres, self.statwindow.y))
        trans.AddWindowReverse(self.equipwindow, (self.equipwindow.x, -self.equipwindow.height))
        trans.AddWindowReverse(self.skillwindow, (self.skillwindow.x, ika.Video.yres))
        
    def StartHide(self):
        trans.AddWindow(self.portraitwindow, (ika.Video.xres, self.portraitwindow.y), remove = True)
        trans.AddWindow(self.statwindow, (-self.statwindow.width, self.statwindow.y), remove = True)
        trans.AddWindow(self.equipwindow, (self.equipwindow.x, -self.equipwindow.height), remove = True)
        trans.AddWindow(self.skillwindow, (self.skillwindow.x, ika.Video.yres), remove = True)

    def Refresh(self, curchar):
        self.portraitwindow.Refresh(curchar)
        self.statwindow.Refresh(curchar)
        self.equipwindow.Refresh(curchar)
        self.skillwindow.Refresh(curchar)

        if self.skillwindow.Text.Length == 0:
            self.skillwindow.AddText('No skills')
    
        self.statbar.Refresh()
        self.portraitwindow.DockTop().DockLeft()
        self.statwindow.DockTop(self.portraitwindow).DockLeft()
        self.statwindow.width = self.portraitwindow.width
        self.equipwindow.DockTop().DockLeft(self.portraitwindow)
        self.skillwindow.DockTop(self.equipwindow).DockLeft(self.statwindow)
        self.equipwindow.Right = self.statbar.x - self.statbar.border * 2
        self.skillwindow.width = self.equipwindow.width
        self.skillwindow.Layout()
        

    def Execute(self):
        curchar = 0
    
        self.Refresh(party[curchar])
    
        while 1:
            ika.Map.Render()
            for x in (self.equipwindow, self.statwindow, self.statbar, self.portraitwindow, self.skillwindow):
                x.Draw()
                
            ika.Video.ShowPage()
    
            ika.Input.Update()
            if left() and curchar > 0:
                curchar -= 1
                self.Refresh(party[curchar])
    
            if right() and curchar < len(party) - 1:
                curchar += 1
                self.Refresh(party[curchar])
    
            if enter() or cancel():
                break
    
        return True
