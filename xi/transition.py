# Menu transition animator
# coded by Andy Friesen
# copyright whenever.  All rights reserved.
#
# This source code may be used for any purpose, provided that
# the original author is never misrepresented in any way.
#
# There is no warranty, express or implied on the functionality, or
# suitability of this code for any purpose.

# Little note: I'm experimenting with notation here.
# In this particular file, an underscore is being used
# in place of the traditional 'self'.

# Second note: List comprehensions are so awesome.

import ika

class Transition(object):
    def __init__(_, time = 0):
        _.time = time
        _.curtime = 0
        _.windows = {}  # window : (start, end)
        _.killqueue = []
        
    def AddWindow(_, window, end, remove = False):
        if len(end) == 2:
            end = (end[0], end[1], window.width, window.height)

        _.windows[window] = (window.Rect, end)
        
        if remove:
            _.killqueue.append(window)

    def AddWindowReverse(_, window, start, remove = False):
        if len(start) == 2:
            start = (start[0], start[1], window.width, window.height)
            
        r = window.Rect
        window.Rect = start
        _.AddWindow(window, r, remove)
        
    def RemoveWindow(_, window):
        del _.windows[window]
        
    def Update(_, dt):
        if dt == 0:
            return
            
        if _.curtime + dt >= _.time:
            _.Finish()
            return
            
        _.curtime += dt
            
        factor = float(dt) / _.time
        for wnd, (start, end) in _.windows.items():
            delta = [ (y - x) for x, y in zip(start, end) ]
            wnd.Rect = [ (x + factor * y) for x, y in zip(wnd.Rect, delta) ]
            
    def Draw(_):
        for wnd in _.windows.keys():
            wnd.Draw()
            
    def Finish(_):
        _.curtime = _.time
        for wnd in _.windows.keys():
            start, end = _.windows[wnd]
            wnd.Rect = end
            _.windows[wnd] = (end, end)
            
        while len(_.killqueue):
            del _.windows[_.killqueue.pop()]
            
    def Execute(_):
        _.curtime = 0
        t = ika.GetTime()
        while not _.Done:
            dt = ika.GetTime() - t
            t = ika.GetTime()
            
            ika.map.Render()
            
            _.Update(dt)
            _.Draw()
            
            ika.ShowPage()
            while t == ika.GetTime():
                ika.input.Update()
            
    Done = property(lambda _: _.curtime == _.time)
