
from mapsys import *

def AutoExec():
    music.stopMusic()

    if stats.isInRoster('claudia'):
        ika.Map.entities.pop('Claudia')

    if 'soapified' in flags:
        pass # TODO

def LeaveTown():
    if 'soapified' not in flags and not stats.isInRoster('claudia'):
        text(
            caption='Clyde', text='Claudia is waiting for me outside her house.'
        )
        if field.player.x > 64:
            field.player.MoveTo(field.player.x - 32, field.player.y)
        else:
            field.player.MoveTo(field.player.x + 32, field.player.y)
        return

    field.mapSwitch('overworld.ika-map', 179 * 16, 47 * 16)

def ClydesHouse():
    field.mapSwitch('towncity-interior.ika-map', 17 * 16, 17 * 16)

def Store():
    field.mapSwitch('towncity-interior.ika-map', 43 * 16, 15 * 16)

# people
def RedMage():
    text("I'll tell you about how %(red)smagic%(white)s works later.\n"
           "It's neat-O." % colours)

def Crier():
    text("Welcome to Town City!")

def Joe():
    text("LOOK AT ME I'M REALLY ANNOYING BLOOP BLOOP YAAAAAAAYyy")

JoeMove = Wander(0, 128)

def Claudia():
    text(
        caption='Claudia',
        text="So, are we going to the Cave or Ultimate Oblivion or what?  "
             "It's a fair hike to the West.")

    '''
    ubersimple cutscene: Claudia does the RPG body-meld with Clyde. (so hard to refrain from innuendo...)
    I think this tiny example makes it clear that we need an API dedicated to scripting cutscenes.
    '''
    clyde = ika.GetPlayer()

    # by popping we take the only reference to the claudia entity out of the entities dict
    # this way, the entity will be removed from the map when the function exits.
    claudia = ika.Map.entities.pop('Claudia')
    claudia.movescript = None # stop whatever it is you're doing
    claudia.Stop()

    # allow her to walk over everything for a moment
    claudia.entobs = claudia.mapobs = False
    # tell her where to move to
    claudia.MoveTo(clyde.x, clyde.y)

    # go until she's reached her destination.
    while True:
        ika.ProcessEntities()
        ika.Map.Render()
        ika.Video.ShowPage()
        ika.Delay(1)

        if not claudia.IsMoving():
            break

    stats.addCharacter('claudia')

ClaudiaMove = Wander(150, 32)
