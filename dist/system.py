
import ika

if 0:
    try:
        BLEH = BLEH
    except:
        global BLEH
        BLEH = 1

        import win32ui
        import pywin.framework.intpyapp
        import sys
        sys.argv = []
        app=win32ui.GetApp()
        app.InitInstance()
        app.Run()

'''
joy = ika.Input.joysticks[0]
ika.Input.right = joy.axes[0]
ika.Input.left = joy.reverseAxes[0]
ika.Input.down = joy.axes[1]
ika.Input.up = joy.reverseAxes[1]
ika.Input.enter = joy.buttons[2]
ika.Input.cancel = joy.buttons[1]
#'''

# initialize the GUI system

from xi import gui

gui.init(
    ika.Font('smallfont.fnt'))

# Little doodad for showing framerate
def hook():
    gui.default_font.Print(0, 280, 'FPS: %i' % ika.GetFrameRate())

def showFps(show=True):
    # unhook it (this keeps it from being hooked more than once, which would
    # be silly and inefficient)
    ika.UnhookRetrace(hook)

    # hook it up
    if show:
        ika.HookRetrace(hook)

def hideFps():
    hookFps(False)

# Initialize stats and items.
# fieldeffects holds all our field item/skill effects,
# so we give it to the stats system, which will link everything up.

import fieldeffects
import stats

stats.initializeItems('items.dat', fieldeffects)
stats.initializeSkills('skills.dat', fieldeffects)

# create the main menu, and connect it to the cancel key.
from menu import mainmenu
mm = mainmenu.MainMenu()
ika.Input.cancel.onpress = mm.execute

# TODO: intro screen goes here.
import intro
intro.title()
"""
# switch  to the initial map, spawn the player, then fade in.
from xi import field
from xi import effects
# don't fade here because there is no player sprite yet.
# note that the coordinates passed to mapSwitch are meaningless here.
#field.mapSwitch('towncity-interior.ika-map', 0, 0, fade = False)
field.mapSwitch('overworld.ika-map', 0, 0, fade=False)
# create the player sprite
#field.spawnPlayer(clyde.spriteName, 13*16, 18 * 16)
field.spawnPlayer(clyde.spriteName, 181 * 16, 49 * 16)
# THEN fade in.
effects.fadeIn(50)
"""
