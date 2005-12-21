#!/usr/bin/env python

"""Input control abstraction."""

# Coded by Andy Friesen.
# Copyright whenever.  All rights reserved.

# This source code may be used for any purpose, provided that the
# original author is never misrepresented in any way.

# There is no warranty, express or implied on the functionality, or
# suitability of this code for any purpose.

import ika


keynames = ['BACKSPACE', 'TAB', 'CLEAR', 'RETURN', 'PAUSE', 'ESCAPE', 'SPACE',
            'EXCLAIM', 'QUOTEDBL', 'HASH', 'DOLLAR', 'AMPERSAND', 'QUOTE',
            'LEFTPAREN', 'RIGHTPAREN', 'ASTERISK', 'PLUS', 'COMMA', 'MINUS',
            'PERIOD', 'SLASH', '0', '1', '2', '3', '4', '5', '6', '7', '8',
            '9', 'COLON', 'SEMICOLON', 'LESS', 'EQUALS', 'GREATER',
            'QUESTION', 'AT', 'LEFTBRACKET', 'BACKSLASH', 'RIGHTBRACKET',
            'CARET', 'UNDERSCORE', 'BACKQUOTE', 'A', 'B', 'C', 'D', 'E', 'F',
            'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S',
            'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'DELETE', 'KP0', 'KP1', 'KP2',
            'KP3', 'KP4', 'KP5', 'KP6', 'KP7', 'KP8', 'KP9', 'KP_PERIOD',
            'KP_DIVIDE', 'KP_MULTIPLY', 'KP_MINUS', 'KP_PLUS', 'KP_ENTER',
            'KP_EQUALS', 'UP', 'DOWN', 'RIGHT', 'LEFT', 'INSERT', 'HOME',
            'END', 'PAGEUP', 'PAGEDOWN', 'F1', 'F2', 'F3', 'F4', 'F5', 'F6',
            'F7', 'F8', 'F9', 'F10', 'F11', 'F12', 'F13', 'F14', 'F15',
            'NUMLOCK', 'CAPSLOCK', 'SCROLLOCK', 'RSHIFT', 'LSHIFT', 'RCTRL',
            'LCTRL', 'RALT', 'LALT', 'RMETA', 'LMETA', 'LSUPER', 'RSUPER',
            'MODE']


up = lambda: ika.Input.up.Position() > 0
down = lambda: ika.Input.down.Position() > 0
left = lambda: ika.Input.left.Position() > 0
right = lambda: ika.Input.right.Position() > 0
enter = lambda: ika.Input.enter.Pressed()
cancel = lambda: ika.Input.cancel.Pressed()
