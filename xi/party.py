# xi system party management stuff
# coded by Andy Friesen
# copyright whenever.  All rights reserved.
#
# This source code may be used for any purpose, provided that
# the original author is never misrepresented in any way.
#
# There is no warranty, express or implied on the functionality, or
# suitability of this code for any purpose.

#--------------- Globals ----------------

chars = {}  # every character in the game is in this dictionary.  The keys are the characters' names.
party = []  # the characters in the current party are in this list, ordered according to the current roster

player  = None # the player entity.
itemdb  = None # item database thingie
skilldb = None # Skill database thingie
inv     = None # party's current inventory

#--------------- Imports ----------------

import ika
import char
import effects

from item import *
from skill import *
from exception import *
from itemdatabase import ItemDatabase
from skilldatabase import SkillDatabase

def Init(itemdat = 'items.dat', fielditemeffects = None, battleitemeffects = None, skilldat = 'skills.dat', fieldskilleffects = None, battleskilleffects = None):
    global itemdb, skilldb, inv

    #try:
    itemdb = ItemDatabase()
    itemdb.Init(itemdat, fielditemeffects, battleitemeffects)
    skilldb = SkillDatabase()
    skilldb.Init(skilldat, fieldskilleffects, battleskilleffects)
    inv = Inventory()
    #except XiException, xe:
    #    ika.Exit('Xi error: ' + xe.__str__())

#------------------------------------------------------------------------------

def CacheCharacter(name, datName = ''):
    if name in chars:   return

    if datName == '':
        datName = name + '.dat'

    chars[name] = char.Character(datName)

def AddCharacter(name,datName=''):
    global chars
    global party
    global player

    CacheCharacter(name, datName)

    if chars[name] in party:        # already in the party?
        return

    party.append(chars[name])
    
    #if len(party) == 1:
    #    chars[name].Spawn(0, 0)     # arbitrary position
    #    player = chars[name].ent
    #    ika.SetPlayer(player)
    #else:
    #    chars[name].Spawn(player.x,player.y)
    #    chars[name].ent.Chase(party[-2].ent, 24) # chase the one ahead
    #    chars[name].ent.isobs = False
    #    chars[name].ent.mapobs = False
    #    chars[name].ent.entobs = False

#------------------------------------------------------------------------------

def RemoveChar(name):
    global chars
    global party

    if name not in chars:
        return

    if len(party) > 0:
        del party[0].ent        		        # kill off the entity
        if party[0] == chars[name]:   		    # are we killing off the leader?
            party[1].Spawn(player.x, player.y) 	# create the new leader
            player = party[1].ent

        party.remove(chars[name])
        FixFollowChain()

#------------------------------------------------------------------------------

def IsCharInParty(name):
    global chars

    if name not in chars:
        return False

    return chars[name] in party

#------------------------------------------------------------------------------

def Warp(x,y,fade = 0):
    for ch in party:
        ch.ent.x = x
        ch.ent.y = y

#------------------------------------------------------------------------------

def MapSwitch(x, y, layer, mapName, fade = False, fadeOut = False, fadeIn = None):
    global player

    if fade or fadeOut:
        effects.FadeOut(100)

    ika.Map.Switch(mapName)
    metaData = ika.Map.GetMetaData()
    if 'entitylayer' in metaData:
        layName = metaData['entitylayer']
        layer = ika.Map.FindLayerByName(metaData['entitylayer']) or layer

    party[0].Spawn(x, y, layer)
    player = party[0].ent

    if fade or fadeIn:
        effects.FadeIn(100)

    #for ch in party:
    #    if ch.ent is not None:
    #        ch.ent.x = x
    #        ch.ent.y = y
    #    else:
    #        ch.Spawn(x, y, layer)
    #Warp(x, y, fade)

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

def FixFollowChain():
    'Sets each party member to follow the one ahead.'
    for i in range(1, len(party)):
        party[i].ent.Chase(party[i - 1].ent, followdist)
