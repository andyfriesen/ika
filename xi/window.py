# Window drawer
# coded by Andy Friesen
# copyright whenever.  All rights reserved.
#
# This source code may be used for any purpose, provided that
# the original author is never misrepresented in any way.
#
# There is no warranty, express or implied on the functionality, or
# suitability of this code for any purpose.

import ika

class Window(object):
    __slots__ = [
        'iLeft',            # Left edge of the window
        'iRight',           # Right edge
        'iTop',             # Top edge
        'iBottom',          # Bottom edge
        'iTopleft',         # Top left corner
        'iTopright',        # Top right corner
        'iBottomleft',      # Bottom left corner
        'iBottomright',     # Bottom right corner
        'iCentre',          # Center area
        'Blit',             # Blitting function used to draw the edges and centre area.  For internal use only.
        'border',           # amound of "breathing room" the window wants to have.  Defaults to the exact size of the border images
        ]
    
    def __init__(self, srcimage, bordersize, stretch = 0):
        def ss(x, y, w, h):
            c = ika.Canvas(w, h)
            srcimage.Blit(c, -x, -y)
            return ika.Image(c)

        if isinstance(srcimage, str):
            srcimage = ika.Canvas(srcimage)
        
        edgex = srcimage.width  - (bordersize * 2)
        edgey = srcimage.height - (bordersize * 2)

        self.iLeft         = ss(0,                  bordersize,         bordersize, edgey)
        self.iRight        = ss(edgex + bordersize, bordersize,         bordersize, edgey)
        self.iTop          = ss(bordersize,         0,                  edgex,      bordersize)
        self.iBottom       = ss(bordersize,         edgey + bordersize, edgex,      bordersize)
        self.iTopleft      = ss(0,                  0,                  bordersize, bordersize)
        self.iTopright     = ss(edgex + bordersize, 0,                  bordersize, bordersize)
        self.iBottomleft   = ss(0,                  edgey + bordersize, bordersize, bordersize)
        self.iBottomright  = ss(edgex + bordersize, edgey + bordersize, bordersize, bordersize)
        self.iCentre       = ss(bordersize,         bordersize,         edgex,      edgey)

        self.border = self.iLeft.width        
        
        if stretch:
            self.Blit = ika.Video.ScaleBlit
        else:
            self.Blit = ika.Video.TileBlit

    #----------------------------------------------------------------

    def Draw(self, x1, y1, x2, y2):
        b = self.Left / 2
        x1 -= b
        y1 -= b
        x2 += b
        y2 += b

        ika.Video.Blit(self.iTopleft,  x1 - self.iTopleft.width, y1 - self.iTopleft.height)
        ika.Video.Blit(self.iTopright, x2, y1 - self.iTopright.height)
        ika.Video.Blit(self.iBottomleft, x1 - self.iBottomleft.width, y2)
        ika.Video.Blit(self.iBottomright, x2, y2)

        self.Blit(self.iLeft, x1 - self.iLeft.width, y1, self.iLeft.width, y2 - y1)
        self.Blit(self.iRight, x2, y1, self.iRight.width, y2 - y1)

        self.Blit(self.iTop, x1, y1 - self.iTop.height, x2 - x1, self.iTop.height)
        self.Blit(self.iBottom, x1, y2, x2 - x1, self.iBottom.height)

        self.Blit(self.iCentre, x1, y1, x2 - x1, y2 - y1)    

    Left   = property( lambda self: self.border )
    Right  = property( lambda self: self.border )
    Top    = property( lambda self: self.border )
    Bottom = property( lambda self: self.border )
    #Left = Right = property( lambda self: self.iLeft.width )
    #Top = Bottom = property( lambda self: self.iTop.height )


#--------------------------------------------------------------------

class SimpleWindow(object):
    def __init__(self, bordercolour = ika.RGB(0, 0, 0), backgroundcolour = ika.RGB(0, 0, 255)):
        self._border = bordercolour
        self._bg = backgroundcolour

    def Draw(self, x1, y1, x2, y2):
        ika.Video.DrawRect(x1, y1, x2, y2, self._bg, True)
        ika.Video.DrawRect(x1, y1, x2, y2, self._border, False)

    Top = Bottom = Left = Right = property(lambda a: 1)
    

#--------------------------------------------------------------------

class GradientWindow(object):
    __slots__ = ['c1', 'c2', 'c3', 'c4'] # Colour of each corner of the window
    
    def __init__(self, c1, c2, c3, c4):
        self.c1 = c1
        self.c2 = c2
        self.c3 = c3
        self.c4 = c4

    def Draw(self, x1, y1, x2, y2):
        ika.Video.DrawTriangle((x1, y1, self.c1), (x2, y1, self.c2), (x2, y2, self.c3))
        ika.Video.DrawTriangle((x2, y2, self.c3), (x1, y2, self.c4), (x1, y1, self.c1))
        ika.Video.DrawRect(x1, y1, x2, y2, ika.RGB(0, 0, 0), False)

    Top = Bottom = Left = Right = property(lambda a: 1)

