# Generic menu interface
# coded by Andy Friesen
# copyright whenever.  All rights reserved.
#
# This source code may be used for any purpose, provided that
# the original author is never misrepresented in any way.
#
# There is no warranty, express or implied on the functionality, or
# suitability of this code for any purpose.

from ika import *
import widget

#------------------------------------------------------------------------------

class _MenuList(widget.TextLabel):
	def __init__(self,x=0,y=0):
		# call parent constructor
		widget.TextLabel.__init__(self)
		
		self.x = x
		self.y = y

		self.ywin=0
		self.ymax=10
		
	def Clear(self):
		self.text=[]

	def AddText(self,*text):
		for x in text:
			self.text.append(x)
			self.width = max(self.width,self.fnt.StringWidth(x))
			
			if len(text)<self.ymax:
				self.height+=self.fnt.height

	def Draw(self,xoffset=0,yoffset=0):
		curx = self.x+xoffset
		cury = self.y+yoffset
		for t in self.text[ self.ywin : self.ywin+self.ymax ]:
			self.PrintString(curx,cury,t)
			cury+=self.fnt.height

#------------------------------------------------------------------------------

CURSOR_WIDTH=10

class Menu(widget.Frame):
	"A menu window.  Has a list of items that the user can select."
	def __init__(self,x=0,y=0):
		widget.Frame.__init__(self)
		
		self.x = x
		self.y = y

		self.menuitems = _MenuList()
		self.menuitems.x = CURSOR_WIDTH
		self.widgets.append(self.menuitems)
		self.cursory = 0
		self.active = 1

	def Draw(self):
		widget.Frame.Draw(self)

		if self.active:
			self.menuitems.fnt.Print(self.x,self.y+self.cursory*widget.defaultfont.height,'>')
	
	def Clear(self):
		self.menuitems.Clear()
	
	def AddText(self,*args):
		self.menuitems.AddText(*args)
		self.AutoSize()

	def Update(self):
		input.Update()
		if input.up:
			input.up=0
			if self.cursory>0:
				self.cursory-=1

		if input.down:
			input.down=0
			if self.cursory<len(self.menuitems.text)-1:
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
			ShowPage()
