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
import item

class StatusBar(widget.TextFrame):
    'Displays HP/MP counts for the party in a vertical status bar thing.'

    def Refresh(self):
        self.Clear()
        for char in party.party:
            self.AddText( '%s\tLv %i' % (char.name, char.level) )
            self.AddText( 'HP\t%i/%i' % (char.HP, char.maxHP) )
            self.AddText( 'MP\t%i/%i' % (char.MP, char.maxMP) )
            self.AddText( '' )
        self.Text.pop() # drop the last blank line
        self.AutoSize()

class PortraitWindow(widget.Frame):
    "Displays the character's portrait, HP, MP, and experience totals."

    def __init__(self):
        widget.Frame.__init__(self)
        self.text = widget.TextLabel()
        self.portrait = widget.Bitmap()
        self.widgets.append(self.portrait)
        self.widgets.append(self.text)

    def Refresh(self,char):
        portrait = self.portrait
        text = self.text

        portrait.Image = char.portrait
        portrait.DockTop().DockLeft()
        text.DockTop().DockLeft(portrait)

        text.Clear()
        text.AddText( '%s\tLv %i' % (char.name, char.level) )
        text.AddText( 'HP\t%i/%i' % (char.HP, char.maxHP) )
        text.AddText( 'MP\t%i/%i' % (char.MP, char.maxMP) )
        text.AddText( '' )
        text.AddText( 'XP\t%i' % char.XP )
        text.AddText( 'Next\t%i' % (char.next - char.XP) )
        
        self.AutoSize()

class StatusWindow(widget.Frame):
    "Displays a character's stats in a frame."

    def __init__(self):
        widget.Frame.__init__(self)
        self.__text = widget.ColumnedTextLabel(columns = 3)
        self.AddChild(self.__text)

    def Refresh(self,char):
        self.__text.Clear()

        def add(n, a):
            self.__text.AddText(n, str(a))

        add('Attack', char.atk)
        add('Defend', char.Def)
        add('Hit %', char.hit)
        add('Evade %', char.eva)

        def add(n, a, b):
            self.__text.AddText(n, str(a), '(~3%i~0)' % b )

        self.__text.AddText( '' )
        add('Strength', char.str, char.nstr)
        add('Vitality', char.vit, char.nvit)
        add('Magic', char.mag, char.nmag)
        add('Will', char.wil, char.nwil)
        add('Speed', char.spd, char.nspd)
        add('Luck', char.luk, char.nluk)
        self.AutoSize()

class EquipWindow(menu.Menu):
    "Displays a character's current equipment."

    def __init__(self):
        self.__text = widget.ColumnedTextLabel(columns = 2)
        menu.Menu.__init__(self, textcontrol = self.__text)
        self.active = False

    def Refresh(self, char):
        self.__text.Clear()

        for e in item.equiptypes:
            i = char.equip[e]
            self.__text.AddText(e.capitalize() + ':', i and i.name or '')
        self.AutoSize()

class SkillWindow(menu.Menu):
    "Displays a character's skills."
    
    def __init__(self):
        self.__text = widget.ColumnedTextLabel(columns = 3)
        menu.Menu.__init__(self, textcontrol = self.__text)
        self.active = False
       
    def Refresh(self, char, condition = lambda s: True):
        self.__text.Clear()
        for s in char.skills:
            c = (condition(s) and '~0' or '~3')
            self.__text.AddText(
                c + s.name,
                c + s.type.capitalize(),
                c + str(s.mp))
        self.AutoSize()

class InventoryWindow(menu.Menu):
    "Displays the group's inventory."

    def __init__(self):
        self.__text = widget.ColumnedTextLabel(columns = 3)
        menu.Menu.__init__(self, textcontrol = self.__text)

    def Refresh(self, condition = lambda i: True):
        self.__text.Clear()
        for i in party.inv:
            c = (condition(i.item) and '~0' or '~3')
            self.__text.AddText(
                c + i.item.name,
                c + i.item.equiptype.capitalize(),
                c + str(i.qty)
                )

        self.AutoSize()

        if self.CursorPos >= len(self.__text):
            self.CursorPos = len(self.__text) - 1