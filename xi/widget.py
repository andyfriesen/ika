# Base UI layer
# coded by Andy Friesen
# copyright whenever.  All rights reserved.
#
# This source code may be used for any purpose, provided that
# the original author is never misrepresented in any way.
#
# There is no warranty, express or implied on the functionality, or
# suitability of this code for any purpose.

# Frames hold an arbitrary number of widgets. (and are widgets themselves)
# Widgets are, currently, either text labels, or images.  Maybe more later.

import ika
import window

# globals (hurk)

defaultwindow = window.Window()

defaultwindow.Load('window.png',8)
defaultfont = ika.Font('arial.fnt')
#defaultfont = Font('eb.fnt')
defaultfont.tabsize = 64

class Widget(object):
	"Basic widget interface."
	def __init__(self,x = 0,y = 0,width = 0,height = 0):
		self.x = x
		self.y = y
		self.width = width
		self.height = height
		
	def Draw(self,xoffset = 0,yoffset = 0):
		pass

	def DockLeft(self):		self.x = 10
	def DockRight(self):		self.x = ika.GetScreenImage().width - self.width - 10
	def DockTop(self):		self.y = 10
	def DockBottom(self):		self.y = ika.GetScreenImage().height - self.height - 10

	def get_Position(self):		return self.x, self.y
	def set_Position(self,pos):	(self.x, self.y) = pos
	
	def get_Size(self):		return self.width, self.height
	def set_Size(self,value):	(self.width, self.height) = value

	Position = property(get_Position, set_Position, doc = 'Gets or sets the position')
	Size     = property(get_Size, set_Size, doc = 'Gets or sets the size (in pixels)')

	Left     = property(lambda self: self.x)
	Top	 = property(lambda self: self.y)
	Right    = property(lambda self: self.x + self.width)
	Bottom   = property(lambda self: self.y + self.height)

class Frame(Widget):
	"Base frame class.  A window, with things in it."
	def __init__(self, wnd = defaultwindow, x = 0, y = 0, width = 0, height = 0):
		Widget.__init__(self, x, y, width, height)
		
		self.wnd = wnd
		self.widgets = []

	def Draw(self):
		self.wnd.Draw(self.x,self.y,self.x+self.width,self.y+self.height)
		for x in self.widgets:
			x.Draw(self.x,self.y)

	def AutoSize(self):				# makes the frame big enough to hold its contents
		self.width = 0
		self.height = 0

		for w in self.widgets:
			self.width = max(self.width,w.x+w.width)
			self.height = max(self.height,w.y+w.height)

class TextFrame(Frame):
	"A frame with a simple text widget.  Nothing else."
	def __init__(self,wnd = defaultwindow,x = 0,y = 0,width = 0,height = 0):
		Frame.__init__(self, wnd, x, y, width, height)
		self.text = TextLabel()
		self.widgets.append(self.text)

	def Clear(self):
		self.text.Clear()
		self.text.width,self.text.height = 0,0

	def AddText(self,args):
		self.text.AddText(args)
		self.Size = self.text.Size

class TextLabel(Widget):
	"Textlabels hold one or more lines of text."
	def __init__(self,fnt = defaultfont,*text):
		Widget.__init__(self)
		
		self.fnt = fnt
		self.x = 0
		self.y = 0
		self.width = 0
		self.height = 0

		self.text = list(text)

		self.LeftJustify()

	def LeftJustify(self):
		self.PrintString = self.fnt.Print

	def RightJustify(self):
		self.PrintString = self.fnt.RightPrint

	def Center(self):
		self.PrintString = self.fnt.CenterPrint

	def Clear(self):
		self.text = []
		self.width = 0
		self.height = 0

	def SetText(self,*text):
		self.Clear()
		self.AddText(text)

	def AddText(self,text):
		for x in text.split('\n'):
			self.text.append(x)
			self.width = max(self.width,self.fnt.StringWidth(x))
			self.height += self.fnt.height

	def Draw(self,xoffset = 0,yoffset = 0):
		curx = self.x+xoffset
		cury = self.y+yoffset

		for t in self.text:
			self.PrintString(curx,cury,t)
			cury+=self.fnt.height

class Bitmap(Widget):
	"Bitmap widgets are images."
	def __init__(self,img = None):
		Widget.__init__(self)
		
		self.img = img
		self.x = 0
		self.y = 0
		if img is not None:
			self.Size = img.width, img.height

	def set_Image(self,val):
		self.img = val
		if val is not None:
			self.Size = val.width, val.height

	def get_Image(self,val):
		return self.img

	Image = property(get_Image, set_Image)

	def Draw(self,xoffset = 0,yoffset = 0):
		# TODO: allow the widget to be resized, thus scaling the image?
		if self.img is not None:
			self.img.Blit(self.x+xoffset,self.y+yoffset)
