"""Common colour constant things for writing coloured text
# Coded by Andy Friesen
# Copyright whenever.  All rights reserved.
#
# This source code may be used for any purpose, provided that
# the original author is never misrepresented in any way.
#
# There is no warranty, express or implied on the functionality, or
# suitability of this code for any purpose.
"""

import binascii
import struct

import ika


def hexColour(r, g, b, a=255):
    c = ika.RGB(r, g, b, a)
    v = binascii.hexlify(struct.pack('!l', c))
    return v

hexColor = hexColour


_o = 0x00
_x = 0xFF

colours = dict([(name, '#[%s]' % hexColour(*value))
                for name, value in (('black',   (_o, _o, _o)),
                                    ('blue',    (_o, _o, _x)),
                                    ('green',   (_o, _x, _o)),
                                    ('aqua',    (_o, _x, _x)),
                                    ('red',     (_x, _o, _o)),
                                    ('violet',  (_x, _o, _x)),
                                    ('yellow',  (_x, _x, _o)),
                                    ('white',   (_x, _x, _x)))])

colors = colours

__all__ = ['colors', 'colours', 'hexColor', 'hexColour']
