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
        
    def __init__(_, statbar):
        _.statbar = statbar
        _.portraitwindow = PortraitWindow()
        _.statwindow = StatusWindow()
        _.equipwindow = EquipWindow()
        _.skillwindow = SkillWindow()
        _.charidx = 0
        
    CurChar = property(lambda _: party[_.charidx])
    
    def StartShow(_):
        _.Refresh(_.CurChar)
        
        trans = Transition()
        trans.AddWindowReverse(_.portraitwindow, (-_.portraitwindow.width, _.portraitwindow.y))
        trans.AddWindowReverse(_.statwindow, (ika.Video.xres, _.statwindow.y))
        trans.AddWindowReverse(_.equipwindow, (_.equipwindow.x, -_.equipwindow.height))
        trans.AddWindowReverse(_.skillwindow, (_.skillwindow.x, ika.Video.yres))
        
    def StartHide(_):
        trans.AddWindow(_.portraitwindow, (ika.Video.xres, _.portraitwindow.y), remove = True)
        trans.AddWindow(_.statwindow, (-_.statwindow.width, _.statwindow.y), remove = True)
        trans.AddWindow(_.equipwindow, (_.equipwindow.x, -_.equipwindow.height), remove = True)
        trans.AddWindow(_.skillwindow, (_.skillwindow.x, ika.Video.yres), remove = True)

    def Refresh(_, curchar):
        _.portraitwindow.Refresh(curchar)
        _.statwindow.Refresh(curchar)
        _.equipwindow.Refresh(curchar)
        _.skillwindow.Refresh(curchar)

        if _.skillwindow.Text.Length == 0:
            _.skillwindow.AddText('No skills')
    
        _.statbar.Refresh()
        _.portraitwindow.DockTop().DockLeft()
        _.statwindow.DockTop(_.portraitwindow).DockLeft()
        _.statwindow.width = _.portraitwindow.width
        _.equipwindow.DockTop().DockLeft(_.portraitwindow)
        _.skillwindow.DockTop(_.equipwindow).DockLeft(_.statwindow)
        _.equipwindow.Right = _.statbar.x - _.statbar.border * 2
        _.skillwindow.width = _.equipwindow.width
        

    def Execute(_):
        curchar = 0
    
        _.Refresh(party[curchar])
    
        while 1:
            ika.Map.Render()
            for x in (_.equipwindow, _.statwindow, _.statbar, _.portraitwindow, _.skillwindow):
                x.Draw()
                
            ika.Video.ShowPage()
    
            ika.Input.Update()
            if left() and curchar > 0:
                curchar -= 1
                _.Refresh(party[curchar])
    
            if right() and curchar < len(party) - 1:
                curchar += 1
                _.Refresh(party[curchar])
    
            if enter() or cancel():
                break
    
        return True
