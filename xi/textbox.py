# Textbox
# coded by Andy Friesen
# copyright whenever.  All rights reserved.
#
# This source code may be used for any purpose, provided that
# the original author is never misrepresented in any way.
#
# There is no warranty, express or implied on the functionality, or
# suitability of this code for any purpose.

from ika import *
import window
from misc import WrapText
import widget

# font is the font to be used
def Text(text,portrait=None,font=None,wnd=None):
	if not wnd:
		wnd=widget.defaultwindow
	if not font:
		font=widget.defaultfont

	scr=GetScreenImage()
	w=(scr.width-wnd.iLeft.width-wnd.iRight.width) / font.width
	t=WrapText(text,w);

	x1=wnd.iLeft.width
	x2=scr.width-wnd.iRight.width
	y2=scr.height-wnd.iBottom.height
	y1=y2-len(t)*font.height

	input.SetButton(4,0)

	while not input.Button(4):
		input.Update()

		map.Render()
		wnd.Draw(x1,y1,x2,y2)
		if portrait:
			portrait.Blit(x1,y1-portrait.height)

		y=y1
		for s in t:
			font.Print(x1,y,s)
			y+=font.height

		ShowPage()
