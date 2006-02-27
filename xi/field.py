# Field (map) handling utility functions
# Coded by Andy Friesen
# 2 November 2003
# Copyright whenever.  All rights reserved.
#
# This source code may be used for any purpose, provided that
# the original author is never misrepresented in any way.
#
# There is no warranty, express or implied on the functionality, or
# suitability of this code for any purpose.

import ika
import effects

player = None

def spawnPlayer(spriteName, x, y, layer = None):
    global player

    if layer is None:
        l = ika.Map.GetMetaData().get('entitylayer', 0)
    else:
        l = layer

    if isinstance(l, str):
        l = ika.Map.FindLayerByName(l)

    if player is None:
        player = ika.Entity(x, y, l, spriteName)
    else:
        player.x = x
        player.y = y
        if layerNum is not None:
            player.layer = l

    ika.SetPlayer(player)

def mapSwitch(mapName, x, y, layerNum = None, fadeOut = False, fadeIn = False, fade = True):
    global player

    if fade or fadeOut:
        effects.fadeOut(50)

    if player is not None:
        player.x = x
        player.y = y

    ika.Map.Switch(mapName)

    if player is not None:
        if layerNum is None:
            lay = ika.Map.GetMetaData().get('entitylayer')
            if lay is not None:
                lay = ika.Map.FindLayerByName(lay)
            player.layer = lay

        else:
            player.layer = layerNum

    if fade or fadeIn:
        effects.fadeIn(50)

def warp(x, y, layerNum = None, fadeOut = False, fadeIn = False, fade = False):
    global player

    if fade or fadeOut:
        effects.fadeOut(50)

    player.x = x
    player.y = y
    if layerNum is not None:
        player.layer = layerNum

    if fade or fadeIn:
        effects.fadeIn(50)

