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
import party
import item

class StatusBar(widget.TextFrame):
    'Displays HP/MP counts for the party in a vertical status bar thing.'

    __slots__ = widget.TextFrame.__slots__ # no extra data members    

    def Refresh(self):
        self.Clear()
        for char in party.party:
            # Red if zero, yellow if less than a 4th max, and white otherwise.
            c = char.HP == 0 and '4' or char.HP < char.maxHP / 4 and '3' or '0'
            d = char.MP == 0 and '4' or char.MP < char.maxMP / 4 and '3' or '0'
                
            self.AddText( '%s\tLv %i' % (char.name, char.level) )
            self.AddText( 'HP\t~%c%i~0/~%c%i' % (c, char.HP, c, char.maxHP) )
            self.AddText( 'MP\t~%c%i~0/~%c%i' % (d, char.MP, d, char.maxMP) )
            self.AddText( '' )
        self.Text.pop() # drop the last blank line
        self.AutoSize()

class PortraitWindow(widget.Frame):
    "Displays the character's portrait, HP, MP, and experience totals."

    __slots__ = widget.Frame.__slots__ + [
        'text',         # The text label used by the window
        'portrait'      # The bitmap used by the window
        ]

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
        text.AddText( 'Exp\t%i' % char.stats.exp )
        text.AddText( 'Next\t%i' % (char.expneeded - char.stats.exp) )
        
        self.AutoSize()

class StatusWindow(widget.Frame):
    "Displays a character's stats in a frame."

    __slots__ = widget.Frame.__slots__ + [
        'text'      # The text control used to display all the statistics
        ]

    def __init__(self):
        widget.Frame.__init__(self)
        self.text = widget.ColumnedTextLabel(columns = 3)
        self.AddChild(self.text)

    def Refresh(self,char):
        self.text.Clear()

        def add(n, a):
            self.text.AddText(n, str(a))

        stats = char.stats
        nstats = char.naturalstats

        add('Attack', stats.atk)
        add('Guard', stats.grd)
        add('Hit %', stats.hit)
        add('Evade %', stats.eva)

        def add(n, a, b):
            self.text.AddText(n, str(a), '(~2%i~0)' % b )

        self.text.AddText( '' )
        add('Strength', stats.str, nstats.str)
        add('Vitality', stats.vit, nstats.vit)
        add('Magic', stats.mag, nstats.mag)
        add('Will', stats.wil, nstats.wil)
        add('Speed', stats.spd, nstats.spd)
        add('Luck', stats.luk, nstats.luk)
        self.AutoSize()

class EquipWindow(menu.Menu):
    "Displays a character's current equipment."

    __slots__ = menu.Menu.__slots__ + [
        'text'      # The text control used to display the equipment.        
        ]

    def __init__(self):
        self.text = widget.ColumnedTextLabel(columns = 2)
        menu.Menu.__init__(self, textcontrol = self.text)
        self.active = False

    def Refresh(self, char):
        self.text.Clear()

        for e in char.equip:
            i = e.item
            self.text.AddText(e.type.capitalize() + ':', i and i.name or '')
        self.AutoSize()
        
        self.CursorPos = min(self.CursorPos, self.menuitems.Length - 1)

class SkillWindow(menu.Menu):
    "Displays a character's skills."

    __slots__ = menu.Menu.__slots__ + [
        'text'
        ]
    
    def __init__(self):
        self.text = widget.ColumnedTextLabel(columns = 3)
        menu.Menu.__init__(self, textcontrol = self.text)
        self.active = False
       
    def Refresh(self, char, condition = lambda s: True):
        self.text.Clear()
        for s in char.skills:
            c = (condition(s) and '~0' or '~2')
            self.text.AddText(
                c + s.name,
                c + s.type.capitalize(),
                c + str(s.mp))
        self.AutoSize()
        
        self.CursorPos = min(self.CursorPos, self.menuitems.Length - 1)

class InventoryWindow(menu.Menu):
    "Displays the group's inventory."

    __slots__ = menu.Menu.__slots__ + [
        'text'
        ]

    def __init__(self):
        self.text = widget.ColumnedTextLabel(columns = 3)
        menu.Menu.__init__(self, textcontrol = self.text)

    def Rehighlight(self, condition = lambda i: True):
        for i in range(len(party.inv)):
            c = (condition(party.inv[i].item) and '~0' or '~2')

            for col in self.text.columns:
                col.text[i] = c + col.text[i][2:]

    def Refresh(self, condition = lambda i: True):
        self.text.Clear()
        for i in party.inv:
            c = (condition(i.item) and '~0' or '~2')
            self.text.AddText(
                c + i.item.name,
                c + i.item.equiptype.capitalize(),
                c + str(i.qty)
                )

        self.AutoSize()

        self.CursorPos = min(self.CursorPos, self.menuitems.Length - 1)