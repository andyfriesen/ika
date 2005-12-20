"""Status menu for xi."""

# Coded by Andy Friesen
# Copyright whenever.  All rights reserved.

# This source code may be used for any purpose, provided that the
# original author is never misrepresented in any way.

# There is no warranty, express or implied on the functionality, or
# suitability of this code for any purpose.

import ika

import controls

from menuwindows import StatusWindow, PortraitWindow, StatusBar, EquipWindow, \
                        SkillWindow
from party import party
from transition import Transition


class StatusMenu(object):
        
    def __init__(self, statbar):
        # The status bar that sits at the right edge of the screen.
        self.statbar = statbar
        # Window that contains the character's portrait, HP, MP, Level,
        # and EXP.
        self.portraitwindow = PortraitWindow()
        # Contains the character's strength, magic, et cetera.
        self.statwindow = StatusWindow()
        # Holds the character's current equipment.
        self.equipwindow = EquipWindow()
        # Shows the skills that the character can use
        self.skillwindow = SkillWindow()
        # Index of the current character
        self.charidx = 0
        
    CurChar = property(lambda self: party[self.charidx])
    
    def StartShow(self):
        self.Refresh(self.CurChar)
        transition = Transition()
        transition.AddWindowReverse(self.portraitwindow,
                                    (-self.portraitwindow.width,
                                     self.portraitwindow.y))
        transition.AddWindowReverse(self.statwindow, (ika.Video.xres,
                                                      self.statwindow.y))
        transition.AddWindowReverse(self.equipwindow,
                                    (self.equipwindow.x,
                                     -self.equipwindow.height))
        transition.AddWindowReverse(self.skillwindow, (self.skillwindow.x,
                                                       ika.Video.yres))
        
    def StartHide(self):
        transition = Transition()  # This was missing. (?)
        transition.AddWindow(self.portraitwindow,
                             (ika.Video.xres, self.portraitwindow.y),
                             remove=True)
        transition.AddWindow(self.statwindow,
                             (-self.statwindow.width, self.statwindow.y),
                             remove=True)
        transition.AddWindow(self.equipwindow,
                             (self.equipwindow.x, -self.equipwindow.height),
                             remove=True)
        transition.AddWindow(self.skillwindow,
                             (self.skillwindow.x, ika.Video.yres), remove=True)

    def Refresh(self, currentcharacter):
        self.portraitwindow.Refresh(currentcharacter)
        self.statwindow.Refresh(currentcharacter)
        self.equipwindow.Refresh(currentcharacter)
        self.skillwindow.Refresh(currentcharacter)
        if self.skillwindow.Text.Length == 0:
            self.skillwindow.AddText('No skills.')
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
        currentcharacter = 0
        self.Refresh(party[currentcharacter])
        while True:
            ika.Render()
            for window in (self.equipwindow, self.statwindow, self.statbar,
                           self.portraitwindow, self.skillwindow):
                window.Draw()
            ika.Video.ShowPage()
            ika.Input.Update()
            if controls.left() and currentcharacter > 0:
                currentcharacter -= 1
                self.Refresh(party[currentcharacter])
            if controls.right() and currentcharacter < len(party) - 1:
                currentcharacter += 1
                self.Refresh(party[currentcharacter])
            if controls.enter() or controls.cancel():
                break
        return True
