#!/usr/bin/env python

import ika


def font(color):
    return '#[%X]' % color


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
