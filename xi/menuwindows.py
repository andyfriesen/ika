# Commonly used windows for the xi menu system.
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
import menu
import party
import item
import skill

class StatusBar(widget.TextFrame):
    'Displays HP/MP counts for the party in a vertical status bar thing.'

    def __init__(self):
        widget.TextFrame.__init__(self)

    def Refresh(self):
        self.Clear()
        for char in party.party:
            # Red if zero, yellow if less than a 4th max, and white otherwise.
            c = char.HP == 0 and '4' or char.HP < char.maxHP / 4 and '3' or '0'
            d = char.MP == 0 and '4' or char.MP < char.maxMP / 4 and '3' or '0'

            self.AddText( char.name )
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
        #text.AddText( '%s\tLv %i' % (char.name, char.level) )
        text.AddText( char.name )
        text.AddText( 'Lv %i' % char.level )
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

    def AutoSize(self):
        menu.Menu.AutoSize(self)
        self.text.columns[0].width = max([self.text.font.StringWidth(x + ': ') for x in item.equiptypes])
        self.text.columns[1].x = self.text.columns[0].Right
        self.text.width = self.text.columns[1].Right
        self.width = self.text.Right

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

        self.YMax = (ika.Video.yres - self.y) / self.Font.height
        self.AutoSize()

        self.CursorPos = min(self.CursorPos, self.menuitems.Length - 1)

    def Layout(self):
        self.text.width = self.width - self.text.x
        col1width = max([self.text.font.StringWidth(x) for x in skill.types]) + 5 # lil bit of padding
        col2width = self.text.font.StringWidth('888')
        self.text.columns[2].x = self.text.width - col2width
        self.text.columns[1].x = self.text.width - col2width - col1width

class InventoryWindow(menu.Menu):
    "Displays the group's inventory."

    __slots__ = menu.Menu.__slots__ + [
        'text'
        ]

    def __init__(self):
        #self.text = widget.ColumnedTextLabel(columns = 3)
        self.text = self.CreateTextLabel()
        menu.Menu.__init__(self, textcontrol = self.text)

    def Rehighlight(self, condition = lambda i: True):
        for i, item in enumerate(party.inv):
            c = condition(item.item)

            self.RehighlightItem(i, c)

    def Refresh(self, condition = lambda i: True):
        p = self.CursorPos
        self.text.Clear()

        for i in party.inv:
            self.AddItem(i, condition(i.item))

        # Make sure it fits onscreen where it is.
        self.YMax = (ika.Video.yres - self.y) / self.Font.height
        self.AutoSize()

        self.CursorPos = min(p, self.menuitems.Length - 1)

    # ------ interface ------
    # override these methods to change how the window displays items

    def CreateTextLabel(self):
        text = widget.ColumnedTextLabel(columns = 2)
        # We'll use the right column to show the quantity, so we'll make it as wide as
        # any quantity has a right to be
        text.columns[-1].width = text.font.StringWidth('888')
        return text

    def AddItem(self, inventorySlot, highlight):
        c = highlight and '~0' or '~2'

        self.text.AddText(
            c + inventorySlot.item.name,
            c + str(inventorySlot.qty)
            )

    # You shouldn't need to override this unless you're doing something
    # special.  It does not rely on the number of columns.
    def Layout(self):
        '''
        Spaces the columns out all nice and pretty.
        The left column is left justified, and given as much space as possible.
        All other columns are right justified. (nothing is done to resize them,
        just reposition based on their current size)
        '''

        # Make the text control as wide as the frame
        self.text.width = self.width - self.text.x

        curX = self.text.width
        # for each column except the first, in reverse order
        for column in self.text.columns[:0:-1]:
            curX -= column.width
            column.x = curX

        # the first column gets whatever is left over
        # I hope curX is positive. @_x
        assert curX > 0, 'Layout problem.  The columns are way too wide.'
        self.text.columns[0].width = curX

    # This doesn't rely on the number of columns, only that each element
    # in each column begins with a font subset command.
    # You shouldn't need to override this unless you need to do something different.
    def RehighlightItem(self, index, highlight):
        # White if True, gray if False
        c = highlight and '~0' or '~2'

        for col in self.text.columns:
            # Since every item begins with a font subset command,
            # it's simply a matter of replacing the first two characters
            # of every element in the column
            col.text[index] = c + col.text[index][2:]
