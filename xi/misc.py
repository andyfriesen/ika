# Miscellany
# Coded by Andy Friesen
# Copyright whenever.  All rights reserved.
#
# This source code may be used for any purpose, provided that
# the original author is never misrepresented in any way.
#
# There is no warranty, express or implied on the functionality, or
# suitability of this code for any purpose.

from xi.textwrapping import *

def wrapText(text, maxWidth, font):
    '''wrapText(text, maxWidth, font) -> list -- Word-wrap text.

    Wraps the text to the given pixel width using the font specified.
    Returns a list of strings.

    (Deprecated, automatically calls xi.textwrapping.wordWrap for you)
    '''
    return wordWrap(text, maxWidth, font)

def clamp(value, lower, upper):
    '''clamp(value, min, max) -- clamp a value

    Returns the value, unless it is outside the min and max allowed values,
    in which case it returns whichever boundary the value crossed. (min
    if value is to low, max if value is too high)
    '''
    return max(min(value, upper), lower)

def formatTime(time, separator = ':'):
    '''formatTime(time[, separator]) -> str -- Format time all pretty-like.'''
    hours = time / 360000
    minutes = time / 6000 % 60
    seconds = time / 100 % 60

    return separator.join(['%02i' % i for i in (hours, minutes, seconds)])
