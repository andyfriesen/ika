# Textbox
# coded by Andy Friesen
# copyright whenever.  All rights reserved.
#
# This source code may be used for any purpose, provided that
# the original author is never misrepresented in any way.
#
# There is no warranty, express or implied on the functionality, or
# suitability of this code for any purpose.

import ika
import window
import widget

from misc import *

# font is the font to be used
def Text(text, portrait = None, font = None, wnd = None):
    wnd = wnd or widget.defaultwindow
    font = font or widget.defaultfont
    
    w=(ika.Video.xres - wnd.iLeft.width - wnd.iRight.width) / font.width
    t=WrapText(text, w);
    
    x1 = wnd.iLeft.width
    x2 = ika.Video.xres - wnd.iRight.width
    y2 = ika.Video.yres - wnd.iBottom.height
    y1 = y2 - len(t) * font.height

    print enter()

    while not enter():
        ika.Input.Update()
        
        ika.Map.Render()
        wnd.Draw(x1, y1, x2, y2)
        if portrait:
            portrait.Blit(x1, y1 - portrait.height)
        
        y = y1
        for s in t:
            font.Print(x1, y, s)
            y += font.height
        
        ika.Video.ShowPage()
