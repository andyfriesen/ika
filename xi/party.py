# xi system party management stuff
# coded by Andy Friesen
# copyright whenever.  All rights reserved.
#
# This source code may be used for any purpose, provided that
# the original author is never misrepresented in any way.
#
# There is no warranty, express or implied on the functionality, or
# suitability of this code for any purpose.

# TODO: party followers?

import ika
import char
from item import *
from itemdatabase import ItemDatabase

#--------------- globals ----------------

chars = {}  # every character in the game is in this dictionary.  The keys are the characters' names.
party = []  # the characters in the current party are in this list, ordered according to the current roster

player = None # the player entity.

itemdb = ItemDatabase('items.dat')
inv = Inventory()

#------------------------------------------------------------------------------

def AddCharacter(name,datname=''):
    global chars
    global party
    global player

    if name not in chars:
        if datname == '':
            datname=name + '.dat'

        chars[name] = char.Character(itemdb,datname)

    if chars[name] in party:        # already in the party?
        return

    party.append(chars[name])

    if len(party) == 1:
        chars[name].Spawn(0, 0)     # arbitrary position
        player = chars[name].ent
        ika.SetPlayer(player)
    else:
        chars[name].Spawn(player.x,player.y)
        chars[name].ent.Chase(player,32)
        chars[name].ent.isobs = False
        chars[name].ent.mapobs = False
        chars[name].ent.entobs = False

#------------------------------------------------------------------------------

def RemoveChar(name):
    global chars
    global party

    if name not in chars:
        return

    if len(party) > 0:
        if party[0] == chars[name]:   		# are we killing off the leader?
            del party[0].ent        		# kill off the player
            party[1].Spawn(player.x,player.y) 	# create the new leader
            player = party[1].ent

        party.remove(chars[name])


#------------------------------------------------------------------------------

def IsCharInParty(name):
    global chars

    if name not in chars:
        return False

    if chars[name] in party:
        return True
    else:
        return False

#------------------------------------------------------------------------------

def Warp(x,y,fade = 0):
    for ch in party:
        ch.ent.x = x
        ch.ent.y = y

#------------------------------------------------------------------------------

def MapSwitch(x,y,mapname,fade = 0):
    ika.map.Switch(mapname)
    Warp(x, y, fade)

#------------------------------------------------------------------------------

def PartyMove(movescript):
    ika.SetPlayer(None)
    player.Move(movescript)
    ika.ProcessEntities()
    while player.IsMoving():
        ika.Wait(10)
        ika.input.Update()
    ika.SetPlayer(player)

#------------------------------------------------------------------------------

