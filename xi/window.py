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

i = 0

class Window(object):
    __slots__ = [ 'iLeft', 'iRight', 'iTop', 'iBottom', 'iTopleft', 'iTopright', 'iBottomleft', 'iBottomright', 'iCentre', 'borderwidth', 'Blit' ]
    def __init__(_, srcimage, bordersize, stretch = 0):
        def ss(x, y, w, h):
            global i
            
            c = ika.Canvas(w, h)
            srcimage.Blit(c, -x, -y)
            i += 1
            return ika.Image(c)

        if isinstance(srcimage, str):
            srcimage = ika.Canvas(srcimage)
        
        edgex = srcimage.width  - (bordersize * 2)
        edgey = srcimage.height - (bordersize * 2)

        _.iLeft         = ss(0,                 bordersize,     bordersize, edgey)
        _.iRight        = ss(edgex + bordersize, bordersize,    bordersize, edgey)
        _.iTop          = ss(bordersize,        0,                  edgex, bordersize)
        _.iBottom       = ss(bordersize,        edgey + bordersize, edgex, bordersize)
        _.iTopleft      = ss(0,                 0,              bordersize, bordersize)
        _.iTopright     = ss(edgex + bordersize, 0,             bordersize, bordersize)
        _.iBottomleft   = ss(0,                 edgey + bordersize, bordersize, bordersize)
        _.iBottomright  = ss(edgex + bordersize, edgey + bordersize, bordersize, bordersize)
        _.iCentre       = ss(bordersize,        bordersize,     edgex, edgey)
        
        if stretch:
            _.Blit = ika.Video.ScaleBlit
        else:
            _.Blit = ika.Video.TileBlit

    #----------------------------------------------------------------

    def Draw(_, x1, y1, x2, y2):
        b = _.Left / 2
        x1 -= b
        y1 -= b
        x2 += b
        y2 += b

        ika.Video.Blit(_.iTopleft,  x1 - _.iTopleft.width, y1 - _.iTopleft.height)
        ika.Video.Blit(_.iTopright, x2, y1 - _.iTopright.height)
        ika.Video.Blit(_.iBottomleft, x1 - _.iBottomleft.width, y2)
        ika.Video.Blit(_.iBottomright, x2, y2)

        _.Blit(_.iLeft, x1 - _.iLeft.width, y1, _.iLeft.width, y2 - y1)
        _.Blit(_.iRight, x2, y1, _.iRight.width, y2 - y1)

        _.Blit(_.iTop, x1, y1 - _.iTop.height, x2 - x1, _.iTop.height)
        _.Blit(_.iBottom, x1, y2, x2 - x1, _.iBottom.height)

        _.Blit(_.iCentre, x1, y1, x2 - x1, y2 - y1)    

    """Left   = property( lambda _: _.iLeft.width )
    Right  = property( lambda _: _.iRight.width )
    Top    = property( lambda _: _.iTop.height )
    Bottom = property( lambda _: _.iBottom.height )"""
    Left = Right = property( lambda _: _.iLeft.width )
    Top = Bottom = property( lambda _: _.iTop.height )


#--------------------------------------------------------------------

class SimpleWindow(object):
    def __init__(_, bordercolour = ika.RGB(0, 0, 0), backgroundcolour = ika.RGB(0, 0, 255)):
        _.__border = bordercolour
        _.__bg = backgroundcolour
        _.Top = _.Bottom = _.Left = _.Right = 1 # border size

    def Draw(_, x1, y1, x2, y2):
        ika.Video.DrawRect(x1, y1, x2, y2, _.__bg, True)
        ika.Video.DrawRect(x1, y1, x2, y2, _.__border, False)

#--------------------------------------------------------------------

class GradientWindow(object):
    def __init__(_, c1, c2, c3, c4):
        _.c1 = c1
        _.c2 = c2
        _.c3 = c3
        _.c4 = c4

    def Draw(_, x1, y1, x2, y2):
        ika.Video.DrawTriangle((x1, y1, _.c1), (x2, y1, _.c2), (x2, y2, _.c3))
        ika.Video.DrawTriangle((x2, y2, _.c3), (x1, y2, _.c4), (x1, y1, _.c1))