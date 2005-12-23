#!/usr/bin/env python

# xi system party management stuff
# coded by Andy Friesen
# copyright whenever.  All rights reserved.
#
# This source code may be used for any purpose, provided that
# the original author is never misrepresented in any way.
#
# There is no warranty, express or implied on the functionality, or
# suitability of this code for any purpose.

import xi.character
import xi.itemdatabase
import xi.skilldatabase
from xi.item import Inventory


CURRENCY_FORMAT = '$%s'
# Current group line-up.
activeRoster = []
# All currently loaded characters.
characters = {}
# Player's current inventory.
inventory = Inventory()
# On-hand currency.
_money = 0 
# Game flag things.  Used to store game-specific information.
flags = {}


# Initialization functions
def initializeItems(datFile, fieldEffects = None, battleEffects = None):
    if isinstance(datFile, str):
        datFile = file(datFile)

    db = xi.itemdatabase.ItemDatabase()
    db.init(datFile, fieldEffects, battleEffects)


def initializeSkills(datFile, fieldEffects = None, battleEffects = None):
    if isinstance(datFile, str):
        datFile = file(datFile)

    db = xi.skilldatabase.SkillDatabase()
    db.init(datFile, fieldEffects, battleEffects)


def cacheCharacter(name, datFile = None):
    if name in characters:
        return characters[name]
    else:
        c = xi.character.Character(datFile or name + '.dat')
        characters[name] = c
        return c


def addCharacter(name, datFile = None):
    c = cacheCharacter(name, datFile)
    if c not in activeRoster:
        activeRoster.append(c)
    return c


def setRoster(*args):
    activeRoster = []
    for name in args:
        activeRoster.append(cacheCharacter(name))


def rosterIndex(charName):
    """Returns the index into the active roster where the character
    currently resides.  If the character is not in the active roster at
    all, None is returned.
    """
    for index, char in enumerate(activeRoster):
        if char.name.lower() == charName.lower():
            return index


def isInRoster(charName):
    """Returns true if that character is currently present."""
    return rosterIndex(charName) is not None


def getMoney():
    global _money
    return _money


def setMoney(money):
    global _money
    _money = money
    return _money


def giveMoney(money):
    global _money
    _money += money
    return _money


def takeMoney(money):
    global _money
    _money = max(0, _money - money)
    return _money


def getGameTime():
    import ika
    return ika.GetTime()


formatCurrency = lambda amount: CURRENCY_FORMAT % amount
