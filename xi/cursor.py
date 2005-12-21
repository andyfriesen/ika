#!/usr/bin/env python

"""Basic cursor classes."""

# Coded by Andy Friesen
# Copyright whenever.  All rights reserved.

# This source code may be used for any purpose, provided that the
# original author is never misrepresented in any way.

# There is no warranty, express or implied on the functionality, or
# suitability of this code for any purpose.

import ika

import xi


class Cursor(object):
    """Base cursor class.
    
    Sort of abstract, it can be used on its own as a null-cursor.
    """
    def __init__(self, width, height, hotSpot):
        super(Cursor, self).__init__()
        self._width = width
        self._height = height

    @xi.readonly
    def width(self):
        return self._width

    @xi.readonly
    def height(self):
        return self._height

    @xi.readonly
    def size(self):
        return self._width, self._height

    def setHotSpot(self, (x, y)):
        self._hotSpot = int(x), int(y)

    hotSpot = property(lambda self: self._hotSpot, setHotSpot)

    def draw(self, x, y):
        pass


# Just an alias.
NullCursor = Cursor


class TextCursor(Cursor):
    """Basic cursor class that uses a font string as a cursor."""

    def __init__(self, font, text='>'):
        width = font.StringWidth(text)
        height = font.height
        hotSpot = width, height / 2
        super(TextCursor, self).__init__(width, height, hotSpot)
        canvas = ika.Canvas(self.width, self.height)
        canvas.DrawText(font, 0, 0, text)
        self._image = ika.Image(canvas)

    def draw(self, x, y):
        ika.Video.Blit(self._image, x - self.hotSpot[0], y - self.hotSpot[1])


class ImageCursor(Cursor):

    def __init__(self, img, hotSpot=None):
        if isinstance(img, (basestring, ika.Canvas)):
            img = ika.Image(img)
        else:
            assert isinstance(img, ika.Image), \
                'img must be an ika.Image, an ika.Canvas, or a string.'
        if hotSpot is None:
            hotSpot = img.width, img.height / 2
        super(ImageCursor, self).__init__(img.width, img.height, hotSpot)
        self._image = img

    def draw(self, x, y):
        ika.Video.Blit(self._image, x - self.hotSpot[0], y - self.hotSpot[1])


class AnimatedCursor(Cursor):

    def __init__(self, frames, delay=10, hotspot=None):
        assert frames, 'At least one animation frame is required.'
        width = frames[0].width
        height = frames[0].height
        hotSpot = hotspot or (width, height / 2)
        super(AnimatedCursor, self).__init__(width, height, hotSpot)
        self._delay = delay
        self._frames = frames

    def draw(self, x, y):
        frame = ika.GetTime() / self._delay
        ika.Video.Blit(self._frames[frame % len(self._frames)],
            x - self._hotSpot[0], y - self._hotSpot[1])

    def createFromImageStrip(canvas, numFrames, width, height, delay=10,
                             hotspot=None):
        """Static method to create a cursor by cutting frames out of one
        big image (vertical strip).
        """
        assert canvas.height % numFrames == 0, \
            "Image's height is not an even multiple of the number of frames."
        frames = [None] * numFrames
        # cut up the canvas, and create our images
        for i in range(numFrames):
            c = ika.Canvas(width, height)
            canvas.Blit(c, 0, -(i * height), ika.Opaque)
            frames[i] = ika.Image(c)
        return AnimatedCursor(frames, delay, hotspot)

    createFromImageStrip = staticmethod(createFromImageStrip)
