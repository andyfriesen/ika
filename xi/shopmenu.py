# Shop menu for pi
# Coded by Troy Potts
# Copyright whenever.  All rights reserved.
#
# This source code may be used for any purpose, provided that
# the original author is never misrepresented in any way.
#
# There is no warranty, express or implied on the functionality, or
# suitability of this code for any purpose.

import ika

from xi import gui
from xi import menu
from xi import sound
from xi.misc import *
from xi.transition import Transition
from xi.fps import FPSManager
from xi import item
import xi.menuwindows
import stats

class ShopMenu(object):
    def __init__(self, *args):
        self.inv = item.Inventory()
        for iter in args:
            self.inv.give(iter)

        self.itemList = xi.menuwindows.ShopWindow(self.inv)
        self.menu = gui.FrameDecorator(menu.Menu(textctrl=self.itemList))
        self.shopbar = gui.FrameDecorator(xi.menuwindows.ShopEquipBar())
        self.shopbar.refresh()

        self.description = gui.FrameDecorator(gui.StaticText(text=['','']))
        self.description.autoSize()
        self.money = gui.FrameDecorator(gui.StaticText(text=['','']))
        self.money.autoSize()
        self.setDescription('')

    def setDescription(self, desc):
        # wordwrap, take the first two lines (that's all we have room for) and join with a newline
        t = '\n'.join(wrapText(desc, self.description.client.width, self.description.font)[:2])
        self.description.text[0] = t

    def updateMoneyBar(self):
        # update the money display
        self.money.text[0] = "bling bling:\n" + stats.formatCurrency(stats.getMoney())

    def updateDescriptionBar(self, item):
        if item is None:
            self.setDescription('')
        else:
            self.setDescription(item.description)

    def layout(self):
        self.refresh()
        self.shopbar.dockRight().dockTop()
        self.description.dockTop().dockLeft()
        self.description.width = (ika.Video.xres - self.description.x -
            self.shopbar.width - self.description.border * 2)
        self.money.width = (ika.Video.xres * 0.33)
        self.money.dockBottom().dockRight()

        self.menu.dockLeft().dockTop(self.description)
        self.menu.width = self.description.width

    def startShow(self, trans):
        self.layout()
        self.refresh()

        trans.addChild(self.shopbar, startRect=(-self.shopbar.width * 2, self.shopbar.y))
        trans.addChild(self.description, startRect=(self.description.x, -self.description.height * 2))
        trans.addChild(self.money, startRect=(self.money.x, ika.Video.yres))
        trans.addChild(self.menu, startRect=(self.menu.x, ika.Video.yres))

    def startHide(self, trans):
        trans.addChild(self.shopbar, endRect=(-self.shopbar.width * 2, self.shopbar.y))
        trans.addChild(self.description, endRect=(self.description.x, -self.description.height * 2))
        trans.addChild(self.money, endRect=(self.money.x, ika.Video.yres))
        trans.addChild(self.menu, endRect=(self.menu.x, ika.Video.yres))

    def refresh(self):
        self.shopbar.refresh(self.inv[self.menu.cursorPos].item)
        self.itemList.refresh(lambda i: i.cost > 0 and i.cost <= stats.getMoney())
        #self.menu.autoSize()
        #self.menu.right = self.shopbar.left - self.shopbar.border * 2

        if len(stats.inventory) == 0:
            self.menu.addText('No Items')

        if self.menu.cursorPos < len(self.inv):
            self.updateDescriptionBar(self.inv[self.menu.cursorPos])
        else:
            self.updateDescriptionBar('')

        self.updateMoneyBar()        

    def update(self):
        result = self.menu.update()
        if result == menu.Cancel:
            return menu.Cancel

        if result is not None:
            item = self.inv[self.menu.cursorPos].item

            if item.cost <= stats.getMoney():
                # buy it!
                stats.takeMoney(item.cost)
                stats.inventory.give(item.name)
                self.refresh()
                sound.buysell.Play()

    def execute(self):
        def draw():
            ika.Map.Render()
            self.menu.draw()
            self.shopbar.draw()
            self.description.draw()
            self.money.draw()

        fps = FPSManager()

        while True:
            oldPos = self.menu.cursorPos
            result = self.update()

            if result is not None:
                break

            if oldPos != self.menu.cursorPos:
                self.shopbar.refresh(self.inv[self.menu.cursorPos].item)
                self.updateDescriptionBar(self.inv[self.menu.cursorPos])

            fps.render(draw)

        return True

#--------------------------------------------

def Shop(*args):
    menu = ShopMenu(*args)
    trans = Transition()
    menu.startShow(trans);   trans.execute()
    menu.execute()
    menu.startHide(trans);   trans.execute()