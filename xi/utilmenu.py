# Handy dandy utility menus
# Coded by Andy Friesen
# 1 Jan 2004
# Copyright whenever.  All rights reserved.
#
# This source code may be used for any purpose, provided that
# the original author is never misrepresented in any way.
#
# There is no warranty, express or implied on the functionality, or
# suitability of this code for any purpose.

import math

from xi import gui
from xi import controls
from xi.menu import Menu, Cancel
from xi.transition import Transition
from xi.layout import VerticalBoxLayout
from xi import wrapText
import ika
import stats

def selectCharacter(parent = None, caption = 'Use on whom?'):
    # A menu to select a character from the active roster.
    # TODO: maybe make this a bit more elaborate.
    caption = gui.FrameDecorator(gui.StaticText(text=caption))
    menu = gui.FrameDecorator(Menu())
    caption.autoSize()

    for char in stats.activeRoster:
        menu.addText(char.name)

    menu.autoSize()
    menu.width = max(menu.width, caption.width)
    caption.width = menu.width

    layout = VerticalBoxLayout(pad=8, children=[caption, menu])
    layout.layout()
    layout.position = (
        (ika.Video.xres - layout.width) / 2,
        (ika.Video.yres - layout.height) / 2
        )

    menu.x += layout.x
    menu.y += layout.y
    caption.x += layout.x
    caption.y += layout.y

    draw = parent and parent.draw or ika.Map.Render

    trans = Transition()
    trans.addChild(caption, startRect=(caption.x, -caption.height))
    trans.addChild(menu, startRect=(menu.x, ika.Video.yres))
    trans.execute(draw)

    result = None
    while result is None:
        result = menu.update()

        draw()
        caption.draw()
        menu.draw()
        ika.Video.ShowPage()

    trans.addChild(caption, endRect=(caption.x, -caption.height))
    trans.addChild(menu, endRect=(menu.x, ika.Video.yres))
    trans.execute(draw)

    return result

def message(msg, context = None):
    'Displays a message to the player in a frame.'

    caption = gui.FrameDecorator(gui.StaticText())

    if isinstance(msg, str):
        # wrap such that the window is about three times as wide as it is tall.
        # (assume square characters because it doesn't have to be precise)
        charWidth = 3 * int(math.sqrt(len(msg) / 3))

        # max out at yres - caption.border * 2
        w = min(ika.Video.xres - caption.border * 2, charWidth * caption.font.width)
        msg = wrapText(msg, w, caption.font)

    caption.addText(*msg)

    caption.autoSize()
    caption.position = (
        (ika.Video.xres - caption.width) / 2,
        (ika.Video.yres - caption.height) / 2
        )

    draw = context and context.draw or ika.Map.Render

    trans = Transition()
    trans.addChild(caption, startRect=(caption.x, -caption.height))
    trans.execute(draw)

    while not controls.enter():
        draw()
        caption.draw()
        ika.Video.ShowPage()
        ika.Input.Update()

    trans.addChild(caption, endRect=(caption.x, ika.Video.yres))
    trans.execute(draw)
