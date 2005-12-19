#!/usr/bin/env python

"""Sound core."""

# Coded by Andy Friesen.
# Copyright whenever.  All rights reserved.

# This source code may be used for any purpose, provided that the
# original author is never misrepresented in any way.

# There is no warranty, express or implied on the functionality, or
# suitability of this code for any purpose.

class NullSound(object):
    """"Sound stub.
    
    Behaves like an ika sound object, but does nothing.
    """

    def __init__(self):
        self.pan = 1
        self.pitchshift = 1.0
        self.volume = 1.0

    def Play(*args):
        """Does nothing."""


null = NullSound()

# Simply assign these to a new value to add sound to the menus and
# such.  Be sure you don't use the from sounds import * form when you
# import, or it won't work.
cursormove = null
cursoraccept = null
cursorcancel = null
buzz = null
buysell = null

# Nuking everything in the namespace except the sounds themselves.
# that way, in the unusual occurance that you need a dict of all the UI
# sounds you can just use __dict__.
del null, NullSound
