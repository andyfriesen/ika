# xi combat system.
# coded by Andy Friesen
# copyright whenever.  All rights reserved.
#
# This source code may be used for any purpose, provided that
# the original author is never misrepresented in any way.
#
# There is no warranty, express or implied on the functionality, or
# suitability of this code for any purpose.

from __future__ import generators
from misc import *

import ika
import menu
import party
from combatmenus import *

'''
Hurray for pcode that was initially pcode but turned into actual python.  :D

globals/instance data: (depending on whether you want to look at the combat system as procedural or a singleton)'''
activemenu = None # The menu thing that the player currently has to deal with, or None.  (this needs to be explained in more detail)
actors  = [] # All battle participants.
players = [] # All actors under direct control of the player.
enemies = [] # All NPC actors. (thus an "enemy" may also be an NPC ally)

class Menu(object):
    'Differs from the normal xi menu in that a menu encapsulates any submenus.'
    def Update(_):
        '''
        Does one tick of the menu.  Just like actors, this is a coroutine thing, and not just a boring method.
        When a menu is done, it should yield something that is not None.  Don't call menu.Update after this happens.
        '''
        yield something

class Actor(object):
    def Update(_):
        '''
        This is the AI coroutine thing.
        In actual fact, it won't be a method declared this way; it'll be an iterator
        assigned from somewhere or other.  The Update iterator will constantly be
        discarded and re-assigned as the Actor's state changes.
        '''
        yield something
        
    def Alive(_):
        '''
        Real obvious.  True if the actor is still alive and kicking
        '''
        return _.HP > 0
        
    # "pure virtual" :)
    HP  = property()
    MP  = property()
    Stats = property()
    Status = property()
    Ent = property()
    Time = property()

class PlayerActor(Actor):
    def __init__(_, char):
        Actor.__init__(_)
        _.char = char
        _.ent = _.char.ent
        _.status = []
        _.Update = _.DoNothing().next

        if _.ent is None:
            _.ent = ika.Entity(0,0,'charles.chr')

        # maybe char.py should store stats like this too. (DEAR GOD MAYBE EAR WAS RIGHT)
        _.stats = \
        {
            'str' : _.char.str,
            'vit' : _.char.vit,
            'mag' : _.char.mag,
            'wil' : _.char.wil,
            'spd' : _.char.spd,
            'luk' : _.char.luk,
            'hit' : _.char.hit,
            'eva' : _.char.eva,
            'atk' : _.char.atk,
            'def' : _.char.Def
        }

    def DoNothing(_):
        while True:
            yield None
    
    def set_HP(_, val):
        _.char.HP = clamp(val, 0, _.char.maxHP)
    
    def set_MP(_, val):
        _.char.MP = clamp(val, 0, _.char.maxMP)
        
    HP = property(lambda _: _.char.HP, set_HP)
    MP = property(lambda _: _.char.MP, set_MP)
    Stats = property(lambda _ : _.stats, doc = "The actor's statistics.  (strength etc)")
    Status = property(lambda _: _.status, doc = 'A list of status effects (strings) currently afflicting the actor')
    Ent = property(lambda _: _.ent, doc = 'The entity associated with the actor.')

def MainLoop():
    global actors, activemenu
    
    activemenu = MainMenu(actors[0])
    
    done = False
    
    while not done:
        for a in actors:
            a.Update()
        
        if activemenu is not None:
            result = activemenu.Update()
            if result is not None:
                return
                
        ika.map.Render()
        trans.Draw()
        ika.Video.ShowPage()
        ika.input.Update()       

def Battle(*args):
    global actors, players, enemies
    
    actors = []
    for char in party.party:
        p = PlayerActor(char)
        actors.append(p)
        players.append(p)
       
    MainLoop()
