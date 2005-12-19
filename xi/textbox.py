# Simple textbox functions
# Coded by Andy Friesen
# 1 January 2004
# Copyright whenever.  All rights reserved.
#
# This source code may be used for any purpose, provided that
# the original author is never misrepresented in any way.
#
# There is no warranty, express or implied on the functionality, or
# suitability of this code for any purpose.

import ika

from xi import gui
from xi import misc
from xi import controls
from xi.transition import Transition

# TODO: have a maximum line count per textbox, and split the text
# into pages if it is exceeded.  I'm still thinking about the most effective
# way to do this.

def createTextBox(text, **kw):
    wnd = kw.get('wnd', gui.default_window)
    font = kw.get('font', gui.default_font)
    textbox = gui.FrameDecorator(gui.StaticText(wnd=wnd, font=font))
    textbox.addText(*misc.wrapText(text, ika.Video.xres - textbox.border * 2, textbox.font))
    textbox.autoSize() # set the height
    textbox.width = ika.Video.xres - textbox.border * 2 # set the width

    return textbox

def text(text, **kw):
    # gather arguments
    draw = kw.get('draw', ika.Map.Render)
    portrait = kw.get('portrait', None)
    caption = kw.get('caption', None)

    # create the text frame
    textbox = createTextBox(text, **kw)
    textbox.dockLeft().dockBottom() # set the position

    # handle the portrait (if any)
    if portrait is not None:
        if isinstance(portrait, (str, ika.Canvas, ika.Image)):
            portrait = gui.Picture(image=portrait)
        elif portrait is not None and not isinstance(portrait, gui.Picture):
            assert False, 'portrait argument must be a string, Canvas, Image, Picture, or None'
        portrait.dockLeft().dockBottom(textbox)

    if caption is not None:
        caption = gui.FrameDecorator(gui.StaticText(text=caption))
        caption.autoSize()
        caption.dockRight().dockBottom(textbox)

    # Do the swoosh-in
    trans = Transition()
    trans.addChild(textbox, startRect=(textbox.x, ika.Video.yres))
    if portrait is not None:
        trans.addChild(portrait, startRect=(-portrait.width, portrait.y))
    if caption is not None:
        trans.addChild(caption, startRect=(ika.Video.xres, caption.y))
    trans.execute(draw)

    # unpress
    controls.enter()

    # Let the player read
    while not controls.enter():
        draw()
        trans.draw()

        ika.Video.ShowPage()
        ika.Input.Update()

    # swoosh-out
    trans.addChild(textbox, endRect=(textbox.x, ika.Video.yres))
    if portrait is not None:
        trans.addChild(portrait, endRect=(portrait.x, ika.Video.yres))
    if caption is not None:
        trans.addChild(caption, endRect=(ika.Video.xres, caption.y))
    trans.execute(draw)
