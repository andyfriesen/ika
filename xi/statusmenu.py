# Status menu for xi
# coded by Andy Friesen
# copyright whenever.  All rights reserved.
#
# This source code may be used for any purpose, provided that
# the original author is never misrepresented in any way.
#
# There is no warranty, express or implied on the functionality, or
# suitability of this code for any purpose.

import ika
import widget
import mainmenu
import party
import equipmenu

from ika import input

from menuwindows import StatusWindow, PortraitWindow, StatusBar, EquipWindow

from party import party
from equipmenu import GenerateEquipMenu

def Update(curchar):
	portraitwindow.Update(curchar)
	statwindow.Update(curchar)
	equipwindow.Update(curchar)
	portraitwindow.Position = (10, 10)
	statwindow.Position = (portraitwindow.Right) + 30, portraitwindow.y
	equipwindow.Position = statwindow.x, statwindow.Bottom + 16

def Execute():
	global portraitwindow, statwindow, equipwindow
	nCurchar=0
	statbar=StatusBar()
	statbar.Update()
	statbar.DockTop()
	statbar.DockRight()

	portraitwindow = PortraitWindow()
	statwindow = StatusWindow()
	equipwindow = EquipWindow()

	Update(party[nCurchar])

	while 1:
		ika.map.Render()
		[ x.Draw() for x in (portraitwindow, statwindow, equipwindow, statbar) ]
		ika.ShowPage()

		input.Update()
		if input.left and nCurchar>0:
			input.left=0
			nCurchar-=1
			Update(party[nCurchar])

		if input.right and nCurchar<len(party)-1:
			input.right=0
			nCurchar+=1
			Update(party[nCurchar])

		if input.enter or input.cancel:
			input.enter=input.cancel=0
			break

	return 1
