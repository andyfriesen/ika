# Menu windows for the xi combat system.
# coded by Andy Friesen
# copyright whenever.  All rights reserved.
#
# This source code may be used for any purpose, provided that
# the original author is never misrepresented in any way.
#
# There is no warranty, express or implied on the functionality, or
# suitability of this code for any purpose.

from __future__ import generators
from widget import *
from menu import *
from misc import *
from transition import trans

import ika

class StatWindow(Frame):
    def __init__(_, char):
        Frame.__init__(_)
        b = Bitmap(char.portrait)
        b.DockLeft().DockTop()
        
        text = TextLabel()
        text.AddText(\
            '%s' % (char.name),
            '',
            'HP\t%i/%i' % (char.HP, char.maxHP),
            'MP\t%i/%i' % (char.MP, char.maxMP))
        text.AutoSize()

        text.DockLeft(b).DockTop()
        
        _.AddChild(b)
        _.AddChild(text)
        _.AutoSize()

class MainMenu(Menu):
    def __init__(_, actor):
        Menu.__init__(_)
        
        _.activemenu = _
        _.actor = actor
        _.char = actor.char
        _.statwindow = StatWindow(_.char)
        
        # I'd like to change this from "Attack" and "Skills" to "Skills" and "Magic" at some point.
        # "attack" is an infamously lame normal boring command that's either used too much, or not
        # enough.  Button mashing sucks.  (normal attacks would fit in the skills section under this
        # system)
        _.AddText('Attack', 'Skills', 'Items')
        
        # start the state machine shenanigans
        _.Update = _.Show().next
        
    def Show(_):
        _.DockRight().DockTop()
        trans.AddWindowReverse(_, (_.x, -_.height))
        
        _.statwindow.DockRight().DockBottom()
        trans.AddWindowReverse(_.statwindow, (XRes(), YRes()))
        
        while not trans.Done:
            trans.Update(1)
            yield None
            
        # done fading in.  Activate the bitch.
        _.Update = _.__Update().next
        yield None
        
    def Hide(_):
        trans.AddWindow(_, (_.x, -_.height), remove = True)
        trans.AddWindow(_.statwindow, (XRes(), YRes()), remove = True)
        
        while not trans.Done:
            trans.Update(1)
            yield None
            
        yield True

    def __Update(_):
        while True:
            if _.activemenu is _:
                result = Menu.Update(_)
                if result is None:
                    yield None
                # elif...
                else:
                    break
            else:
                _.activemenu.Update()
                yield None
        
        _.Update = _.Hide().next
        yield None
