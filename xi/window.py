# Window drawer
# coded by Andy Friesen
# copyright whenever.  All rights reserved.
#
# This source code may be used for any purpose, provided that
# the original author is never misrepresented in any way.
#
# There is no warranty, express or implied on the functionality, or
# suitability of this code for any purpose.

from ika import *

class Window:
    def __init__(self):
        self.iLeft = None
        self.iRight = None
        self.iTop = None
        self.iBottom = None
        self.iTopleft = None
        self.iTopright = None
        self.iBottomleft = None
        self.iBottomRight = None
        self.iCentre = None

#----------------------------------------------------------------

    def _CreateCorners(self, img, borderwidth):
        self.iTopleft = Image(borderwidth, borderwidth)
        self.iTopright = Image(borderwidth, borderwidth)
        self.iBottomleft = Image(borderwidth, borderwidth)
        self.iBottomright = Image(borderwidth, borderwidth)

        SetRenderDest(self.iTopleft)
        img.Blit(0, 0, 0)

        SetRenderDest(self.iTopright)
        img.Blit(-(img.width - borderwidth), 0, 0)

        SetRenderDest(self.iBottomleft)
        img.Blit(0, -(img.height - borderwidth), 0)

        SetRenderDest(self.iBottomright)
        img.Blit(-(img.width - borderwidth), -(img.height - borderwidth), 0)

#----------------------------------------------------------------

    def _CreateEdges(self, img, borderwidth):
        self.iTop = Image(img.width - borderwidth * 2, borderwidth)
        self.iBottom = Image(img.width - borderwidth * 2, borderwidth)
        self.iLeft = Image(borderwidth, img.height - borderwidth * 2)
        self.iRight = Image(borderwidth, img.height - borderwidth * 2)

        SetRenderDest(self.iTop)
        img.Blit(-borderwidth, 0, 0)

        SetRenderDest(self.iBottom)
        img.Blit(-borderwidth, borderwidth - img.height, 0)

        SetRenderDest(self.iLeft)
        img.Blit(0, -borderwidth, 0)

        SetRenderDest(self.iRight)
        img.Blit(borderwidth - img.width, -borderwidth, 0)

#----------------------------------------------------------------

    def Load(self, filename, borderwidth):
        img = Image()
        img.Load(filename)

        if img.width < borderwidth * 2:
            self.iCentre = img
            return

        centrewidth = img.width - borderwidth * 2
        centreheight = img.height - borderwidth * 2

        oldrenderdest = GetRenderDest()

        self._CreateCorners(img, borderwidth)
        self._CreateEdges(img, borderwidth)

        # lastly, the centre image
        self.iCentre = Image(img.width - borderwidth * 2, img.height - borderwidth * 2)
        SetRenderDest(self.iCentre)
        img.Blit(-borderwidth, -borderwidth, 0)
        
        SetRenderDest(oldrenderdest)

#----------------------------------------------------------------

    def Draw(self, x1, y1, x2, y2):
        self.iTopleft.Blit(x1-self.iTopleft.width, y1-self.iTopleft.height)
        self.iTopright.Blit(x2, y1-self.iTopright.height)
        self.iBottomleft.Blit(x1-self.iBottomleft.width, y2)
        self.iBottomright.Blit(x2, y2)

        self.iLeft.ScaleBlit (x1-self.iLeft.width, y1, self.iLeft.width, y2-y1)
        self.iRight.ScaleBlit(x2, y1, self.iRight.width, y2-y1)

        self.iTop.ScaleBlit   (x1, y1-self.iTop.height, x2-x1, self.iTop.height)
        self.iBottom.ScaleBlit(x1, y2, x2-x1, self.iBottom.height)
        self.iCentre.ScaleBlit(x1, y1, x2-x1, y2-y1)

class SimpleWindow(object):
    def __init__(self, bordercolour, backgroundcolour):
        self.__border = bordercolour
        self.__bg = backgroundcolour

    def Draw(self, x1, y1, x2, y2):
        scr = GetScreenImage()
        scr.Rect(x1, y1, x2, y2, __bg, 1)
        scr.Rect(x1, y1, x2, y2, __border, 0)
