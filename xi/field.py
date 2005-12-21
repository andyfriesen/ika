#!/usr/bin/env python

"""Field (map) handling utility functions."""

# Coded by Andy Friesen.
# 2 November 2003
# Copyright whenever.  All rights reserved.

# This source code may be used for any purpose, provided that the
# original author is never misrepresented in any way.

# There is no warranty, express or implied on the functionality, or
# suitability of this code for any purpose.

import ika

import xi.effects


player = None

def spawnPlayer(spriteName, x, y, layer=None):
    global player
    if layer is None:
        layer = ika.Map.GetMetaData().get('entitylayer', 0)
    if isinstance(layer, basestring):
        layer = ika.Map.FindLayerByName(layer)
    if player is None:
        player = ika.Entity(x, y, layer, spriteName)
    else:
        player.x = x
        player.y = y
        if layerNum is not None:
            player.layer = layer
    ika.SetPlayer(player)

def mapSwitch(mapName, x, y, layerNum=None, fadeOut=False, fadeIn=False,
              fade=True):
    global player
    if fade or fadeOut:
        xi.effects.fadeOut(50)
    if player is not None:
        player.x = x
        player.y = y
    ika.Map.Switch(mapName)
    if player is not None:
        if layerNum is None:
            layer = ika.Map.GetMetaData().get('entitylayer')
            if layer is not None:
                layer = ika.Map.FindLayerByName(layer)
            player.layer = layer
        else:
            player.layer = layerNum
    if fade or fadeIn:
        xi.effects.fadeIn(50)

def warp(x, y, layerNum=None, fadeOut=False, fadeIn=False, fade=False):
    global player
    if fade or fadeOut:
        xi.effects.fadeOut(50)
    player.x = x
    player.y = y
    if layerNum is not None:
        player.layer = layerNum
    if fade or fadeIn:
        xi.effects.fadeIn(50)
