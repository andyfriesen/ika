# Simple FPS manager
# Coded by Andy Friesen
# Copyright whenever.  All rights reserved.
#
# This source code may be used for any purpose, provided that
# the original author is never misrepresented in any way.
#
# There is no warranty, express or implied on the functionality, or
# suitability of this code for any purpose.

import ika

MAX_SKIP = 10

class FPSManager(object):
    def __init__(self, rate = 100):
        self._rate = rate
        self._ticksPerFrame = 100.0 / rate
        self._nextTimeFrame = ika.GetTime()
        self._skipCount = 0

    def render(self, func):
        now = ika.GetTime()

        # ahead.  Pause a moment
        if now < self._nextTimeFrame:
            ika.Delay(int(self._nextTimeFrame - now))

        # behind, skip
        if now > self._nextTimeFrame and self._skipCount < MAX_SKIP:
            self._skipCount += 1
        else:
            self._skipCount = 0
            func()
            ika.Video.ShowPage()

        self._nextTimeFrame += self._ticksPerFrame

    def sync(self):
        self._nextTimeFrame = ika.GetTime()
