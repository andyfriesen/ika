# Commonly used windows for the xi menu system.
# coded by Andy Friesen
# copyright whenever.  All rights reserved.
#
# This source code may be used for any purpose, provided that
# the original author is never misrepresented in any way.
#
# There is no warranty, express or implied on the functionality, or
# suitability of this code for any purpose.

import widget
import menu
import party # -_-

class StatusBar(widget.TextFrame):
	'Displays HP/MP counts for the party in a vertical status bar thing.'
		
	def Update(self):
		self.Clear()
		for char in party.party:
			self.AddText( char.name )
			self.AddText( 'HP\t%i/%i' % (char.HP, char.maxHP) )
			self.AddText( 'MP\t%i/%i' % (char.MP, char.maxMP) )
			self.AddText( '' )

class PortraitWindow(widget.Frame):
	"Displays the character's portrait, HP, MP, and experience totals."

	def __init__(self):
		widget.Frame.__init__(self)
		self.text = widget.TextLabel()
		self.portrait = widget.Bitmap()
		self.widgets.append(self.portrait)
		self.widgets.append(self.text)

	def Update(self,char):
		portrait = self.portrait
		text = self.text
		
		portrait.Image = char.portrait
		portrait.Position = (0, 0)

		text.Clear()
		text.AddText( char.name )
		text.AddText( 'HP\t%i/%i' % (char.HP, char.maxHP) )
		text.AddText( 'MP\t%i/%i' % (char.MP, char.maxMP) )
		text.AddText( '' )
		text.AddText( 'XP\t%i' % char.XP )
		text.AddText( 'Next\t%i' % (char.next - char.XP) )
		text.Position = (0, portrait.height)

		self.AutoSize()

class StatusWindow(widget.TextFrame):
	"Displays a character's stats in a frame."

	def Update(self,char):
		self.Clear()
		
		self.AddText( 'Strength\t%i\t~2%i' % (char.str, char.nstr) )
		self.AddText( 'Vitality\t%i\t~2%i' % (char.vit, char.nvit) )
		self.AddText( 'Magic\t%i\t~2%i'    % (char.mag, char.nmag) )
		self.AddText( 'Will\t%i\t~2%i'     % (char.wil, char.nwil) )
		self.AddText( 'Speed\t%i\t~2%i'    % (char.spd, char.nspd) )
		self.AddText( 'Luck\t%i\t~2%i'     % (char.luk, char.nluk) )
		

class EquipWindow(widget.TextFrame):
	"Displays a character's current equipment."

	def Update(self, char):
		self.Clear()

		for e in char.equip.items():
			self.AddText( '%s:\t%s' % (e[0].capitalize(), e[1] and e[1].name or '') )
