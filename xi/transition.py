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

# Second note: Functional stuff is so nifty.

import ika

from misc import *

from statelessproxy import *

DEFAULT_TIME = 30

class Transition(StatelessProxy):
    def __init__(self):
        StatelessProxy.__init__(self)
        
        if len(self.__dict__) != 0:
            return
        
        self.time = DEFAULT_TIME
        self.curtime = 0
        self.windows = []  # The window itself
        self.startend = [] # (start, end) (indeces match up with the windows list)
        self.killqueue = []
        
    def AddWindow(self, window, end, remove = False):
        if len(end) == 2:
            end = (end[0], end[1], window.width, window.height)

        if window in self.windows:
            i = self.windows.index(window)
            self.windows.pop(i)
            self.startend.pop(i)

        self.windows.append(window)
        self.startend.append((window.Rect, end))

        if remove:
            self.killqueue.append(window)

    def AddWindowReverse(self, window, start, remove = False):
        if len(start) == 2:
            start = (start[0], start[1], window.width, window.height)

        r = window.Rect
        window.Rect = start
        self.AddWindow(window, r, remove)

    def RemoveWindow(self, window):
        i = self.windows.index(window)
        if i != -1:
            self.windows.pop(i)
            self.startend.pop(i)

    def Reset(self):
        self.curtime = 0
        i = 0
        for wnd in self.windows:
            r = wnd.Rect
            self.startend[i] = (r, r)
            i += 1

    def Finish(self):
        self.curtime = self.time
        i = 0
        for i in range(len(self.startend)):
            start, end = self.startend[i]
            self.windows[i].Rect = end
            self.startend[i] = (end, end)

        while len(self.killqueue):
            i = self.windows.index(self.killqueue.pop())
            self.windows.pop(i)
            self.startend.pop(i)

    def Update(self, dt):
        if dt == 0:
            return

        if self.curtime + dt >= self.time:
            self.Finish()
            return

        self.curtime += dt

        factor = float(dt) / self.time
        for wnd, (start, end) in zip(self.windows, self.startend):
            delta = map(lambda x, y: y - x, start, end)
            wnd.Rect = map(lambda x, y: int(x + factor * y), wnd.Rect, delta)

    def Draw(self):
        for wnd in self.windows:
            wnd.Draw()

    def Execute(self):
        self.curtime = 0
        t = ika.GetTime()
        while not self.Done:
            t2 = ika.GetTime()
            dt = t2 - t
            t = t2

            ika.Map.Render()

            self.Update(dt)
            self.Draw()

            ika.Video.ShowPage()
            while t == ika.GetTime():
                ika.Input.Update()

        ika.Input.Unpress()

    Done = property(lambda self: self.curtime == self.time)

trans = Transition()
