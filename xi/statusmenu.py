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

from ika import input

from menuwindows import StatusWindow, PortraitWindow, StatusBar, EquipWindow, SkillWindow
from misc import *

from party import party

class StatusMenu(object):
    def __init__(_, statbar):
        _.statbar = statbar
        _.portraitwindow = PortraitWindow()
        _.statwindow = StatusWindow()
        _.equipwindow = EquipWindow()
        _.skillwindow = SkillWindow()
        _.charidx = 0
        
    CurChar = property(lambda _: party[_.charidx])
    
    def StartShow(_, trans):
        _.Refresh(_.CurChar)
        trans.AddWindowReverse(_.portraitwindow, (-_.portraitwindow.width, _.portraitwindow.y))
        trans.AddWindowReverse(_.statwindow, (XRes(), _.statwindow.y))
        trans.AddWindowReverse(_.equipwindow, (_.equipwindow.x, -_.equipwindow.height))
        trans.AddWindowReverse(_.skillwindow, (_.skillwindow.x, YRes()))
        
    def StartHide(_, trans):
        trans.AddWindow(_.portraitwindow, (XRes(), _.portraitwindow.y), remove = True)
        trans.AddWindow(_.statwindow, (-_.statwindow.width, _.statwindow.y), remove = True)
        trans.AddWindow(_.equipwindow, (_.equipwindow.x, -_.equipwindow.height), remove = True)
        trans.AddWindow(_.skillwindow, (_.skillwindow.x, YRes()), remove = True)

    def Refresh(_, curchar):
        _.portraitwindow.Refresh(curchar)
        _.statwindow.Refresh(curchar)
        _.equipwindow.Refresh(curchar)
        _.skillwindow.Refresh(curchar)
    
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
            ika.map.Render()
            for x in (_.equipwindow, _.statwindow, _.statbar, _.portraitwindow, _.skillwindow):
                x.Draw()
                
            ika.ShowPage()
    
            input.Update()
            if input.left and curchar > 0:
                input.left = 0
                curchar -= 1
                _.Refresh(party[curchar])
    
            if input.right and curchar < len(party) - 1:
                input.right = 0
                curchar += 1
                _.Refresh(party[curchar])
    
            if input.enter or input.cancel:
                input.enter = input.cancel = False
                break
    
        return True
