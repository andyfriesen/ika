#!/usr/bin/env python

import ika


def font(*args):
    if len(args) == 1:
        red, green, blue, alpha = ika.GetRGB(args[0])
    elif len(args) == 3:
        red, green, blue = args
        alpha = 0xFF
    elif len(args) == 4:
        red, green, blue, alpha = args
    else:
        raise TypeError('font() takes at most 4 arguments (%s given)' %
                        len(args))
    return '#[%X%X%X%X]' % (red, green, blue, alpha)


o = 0x00
x = 0xFF


transparent = ika.RGB(o, o, o, o)
black = ika.RGB(o, o, o)
blue = ika.RGB(o, o, x)
green = ika.RGB(o, x, o)
aqua = ika.RGB(o, x, x)
red = ika.RGB(x, o, o)
violet = ika.RGB(x, o, x)
yellow = ika.RGB(x, x, o)
white = ika.RGB(x, x, x)


del o, x
