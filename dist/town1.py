
from xi.textbox import text
from xi import field
from xi.movescript import Wander, Script

def ToOverWorld():
    field.mapSwitch('overworld.ika-map', 63 * 16, 14 * 16)

def TownCrier():
    text("Welcome to Town A!")

def WellWatcher():
    text("AIN'T NOBODY GONNA STEAL THIS WELL ON MY WATCH")

WellWatcherMove = (Script().
    Wait(100).
    MoveUp(16).
    Wait(100).
    MoveDown(16).
    Loop()
)

def TreeGuy():
    text('I like this tree')

def Ted():
    text('''\
Yeah, I know, it's a pretty lame demo.  But look at all the neat scripting!
Isn't it posh?
''')

TedMove = Wander(100, 16)

def Joe():
    text("LOOK AT ME I'M REALLY ANNOYING BLOOP BLOOP YAAAAAAAYyy")

JoeMove = Wander(0, 128)

def Jitters():
    text('ilovecoffeewantacupitsreallygoodilovecoffeeittastessogoodineedtohaveanothercupofcoffeeorillfallasleepagainandthatwouldbebadcoffeecoffeecoffeecoffeecoffeecoffee')

JittersMove = Wander(0, 4)
