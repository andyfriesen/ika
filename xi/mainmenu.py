# Main menu
# coded by Andy Friesen
# copyright whenever.  All rights reserved.
#
# This source code may be used for any purpose, provided that
# the original author is never misrepresented in any way.
#
# There is no warranty, express or implied on the functionality, or
# suitability of this code for any purpose.

import ika
import party
import widget
from menu import Menu
from statelessproxy import StatelessProxy

from menuwindows import StatusBar
import statusmenu
import itemmenu
import equipmenu

class MainMenu(StatelessProxy):
	def __init__(self):
		StatelessProxy.__init__(self)
		if len(self.__dict__) != 0:
			return

		# "static constructor" logic follows
		mm = self.mainmenu = Menu()
		mm.AddText('Item','Magic','Equip','Status')
		mm.Position = 10,10

		self.statbar = StatusBar()
		self.statbar.Update()
		self.statbar.Position = (ika.GetScreenImage().width - self.statbar.width - 10), 10

		self.submenu = [ itemmenu.Execute, dummy, dummy, statusmenu.Execute ]

	def Draw(self):
		self.statbar.Draw()
		self.mainmenu.Draw()

	def Update(self):
		return self.mainmenu.Update()

	def Execute(self):
		self.statbar.Update()
		
		done = 0
		while not done:
			ika.map.Render()
			self.Draw()
			ika.ShowPage()
			
			result = self.Update()

			if result == -1:
				done = 1
			
			elif result != None:
				result = self.submenu[result]()
				ika.input.enter = 0				
				if not result:
					break

#------------------------------------------------------------------------------

def dummy():
	return 1
