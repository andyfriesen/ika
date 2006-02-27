# Basic GUI elements
# Coded by Andy Friesen
# Copyright whenever.  All rights reserved.
#
# This source code may be used for any purpose, provided that
# the original author is never misrepresented in any way.
#
# There is no warranty, express or implied on the functionality, or
# suitability of this code for any purpose.

"""
I have tried time and again to make such a system as elegant as possible, and
have repeatedly failed.  This time, I am bound and determined to make a simple,
consistent framework.

Style:
Everything public uses camel notation.  Classes have their first letter uppercase.
Private things are preceded with a single underscore.  Under no circumstance
are private attributes to be used instead of their accessor properties.  This is
essential to preserve polymorphic behaviour.

Prefer keyword arguments.  They should almost always be the same as properties
defined in the class.

Concepts:
Location - The outer bounding box surrounding a widget.
    Get or set this to move the widget around on its parent.

Client - The size of the inner area of the widget.
    Child widgets store their position relative to this.
    This is also how much room is *inside* the widget.

Margin - Size of the rectangle around the widget.

+----------------
|  This is the margin.  Sizers set aside this much room.
|   +------------
|   |  Location describes this middle rect here.
|   |   +--------
|   |   |
|   |   |  Client area is here
"""

class Widget(object):
    '''
    Base widget class.  All UI elements inherit this. (they don't really
    have to, but, either way, this is the interface)

    '''

    margin = property(
        fget=lambda self: self.getMargin(),
        fset=lambda self, value: self.setMargin(value)
    )

    location = property(
        fget=lambda self: self.getLocation(),
        fset=lambda self, value: self.setLocation(value)
    )

    clientSize = property(
        fget=lambda self: self.getClientSize(),
        fset=lambda self, value: self.setClientSize(value)
    )

    def __init__(self):
        self.__margin = 0
        self.__location = [0, 0, 0, 0]
        self.__clientSize = [0, 0]

    def getMargin(self):
        return self.__margin

    def getLocation(self):
        return tuple(self.__location)

    def getClientSize(self):
        return tuple(self.getLocation[2:])

    def setMargin(self, value):
        self.__margin = int(value)

    def setLocation(self, (x, y, w, h)):
        self.__location = [x, y, w, h]

    def setClientSize(self, (w, h)):
        self.__clientSize = [w, h]

    def draw(self, x, y):
        pass
