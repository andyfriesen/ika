# Generic menu interface
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
import cursor

input = ika.input
#------------------------------------------------------------------------------

CURSOR_WIDTH=20
defaultcursor = cursor.Cursor(widget.defaultfont)

def SetDefaultCursor(csr):
	global defaultcursor
	
	defaultcursor = csr

class Menu(widget.Frame):
	"A menu window.  Has a list of items that the user can select."
	def __init__(self, x = 0 , y = 0, cursor = None, textcontrol = None):
		global defaultcursor

		widget.Frame.__init__(self)
		self.menuitems = textcontrol or widget.TextLabel()

		self.menuitems.x = CURSOR_WIDTH
		self.widgets.append(self.menuitems)
		self.cursor = cursor or defaultcursor

		self.Position = (x, y)
		self.Size = self.menuitems.Size
		self.width += CURSOR_WIDTH

		self.cursory = 0
		self.active = 1

	def Draw(self):
		widget.Frame.Draw(self)

		if self.active:
			self.cursor.Draw(self.x + CURSOR_WIDTH, self.y + (self.cursory + 0.5) * self.menuitems.font.height)

	def Clear(self):
		self.menuitems.Clear()

	def AddText(self,*args):
		self.menuitems.AddText(*args)
		self.AutoSize()

	def AutoSize(self):
		self.menuitems.AutoSize()
		widget.Frame.AutoSize(self)

	def Update(self):
		input.Update()
		if input.up:
			input.up=0
			if self.cursory>0:
				self.cursory-=1

		if input.down:
			input.down=0
			if self.cursory<len(self.menuitems)-1:
				self.cursory+=1

		if input.enter:
			input.enter=0
			return self.cursory

		if input.cancel:
			input.cancel=0
			return -1

		return None

	def Execute(self):
		input.enter=0
		self.cursory=0

		while 1:
			map.Render()

			result = self.Update()
			if result != None:
				return result
				
			self.Draw()
			ika.ShowPage()
