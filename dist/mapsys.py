'''
Map system... things.  This is just a convenience module that imports
most of the things that map scripts will want available.
'''

import ika

from xi import field, music
from xi.textbox import text
from xi.movescript import Wander, Script, Follow
from xi.colours import *

from menu.shopmenu import Shop

import stats
from stats import flags

import weather


def initBox(name, flag, openFunc):
    e = ika.Map.entities[name]
    if flag in flags:
        e.specframe = 7
        e.actscript = None
    else:
        def open():
            flags[flag] = True
            e.specanim = e.GetAnimScript('open')
            ika.Wait(80)
            openFunc()
        e.actscript = open
