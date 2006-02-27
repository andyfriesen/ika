
from xi.textbox import text
from xi import field
from xi.movescript import Wander, Script

def WestExit():
    field.mapSwitch('overworld.ika-map', 71 * 16, 21 * 16)

def EastExit():
    field.mapSwitch('overworld.ika-map', 73 * 16, 21 * 16)

def TownCrier():
    text('''\
Welcome to Town B!
We spent a lot if time carefully deliberating over a name for our town.''')

TownCrierMove = Wander(50, 32)

def WellWatcher():
    text("I'm a well-watcher!")
    text('''\
It's kind of like weight watchers, except that you watch the well, not your \
weight.''')

WellWatcherMove = (Script().
    MoveDown(16).
    Wait(150).
    MoveUp(16).
    Wait(150).
    Loop()
)
