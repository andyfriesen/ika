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
        self.AutoSize()

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

class StatusWindow(widget.Frame):
    "Displays a character's stats in a frame."

    def __init__(self):
        widget.Frame.__init__(self)
        self.__text = widget.ColumnedTextLabel(columns = 3)
        self.AddChild(self.__text)

    def Update(self,char):
        self.__text.Clear()

        def add(n, a):
            self.__text.AddText(n, str(a))

        add('Attack', char.atk)
        add('Defend', char.Def)
        add('Hit %', char.hit)
        add('Evade %', char.eva)

        def add(n, a, b):
            self.__text.AddText(n, str(a), '~2' + str(b) )

        self.__text.AddText( '' )
        add('Strength', char.str, char.nstr)
        add('Vitality', char.vit, char.nvit)
        add('Magic', char.mag, char.nmag)
        add('Will', char.wil, char.nwil)
        add('Speed', char.spd, char.nspd)
        add('Luck', char.luk, char.nluk)
        self.AutoSize()

class EquipWindow(widget.TextFrame):
    "Displays a character's current equipment."

    def Update(self, char):
        self.Clear()

        for e in char.equip.items():
            self.AddText( '%s:\t%s' % (e[0].capitalize(), e[1] and e[1].name or '') )

class InventoryWindow(widget.TextFrame):
    "Displays the group's inventory."

    def Update(self):
        for i in party.inv:
            self.AddText( '%s\t%i' % (i.item.name, i.qty) )
