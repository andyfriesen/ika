
import ika
import stats
from xi import field, effects

def title():
    splash = ika.Image('splash.png')
    while not ika.Input.keyboard['RETURN'].Pressed():
        ika.Video.Blit(splash, 0, 0, False)
        ika.Video.ShowPage()
        ika.Input.Update()

    # TODO: menu
    newGame()

def newGame():
    # Add Clyde to the active roster.  Grab the return value
    # because we want to do some stuff with him right away...
    clyde = stats.addCharacter('clyde')

    # ...like give him some initial equipment...
    clyde.equip('Wood Sword')
    clyde.equip('Shirt')
    # ...and the Heal spell
    clyde.skills.add('Heal')

    # don't add claudia yet, but load her up so we can give her stuff
    claudia = stats.cacheCharacter('claudia')
    # Claudia can equip two weapons at once, so we have to specify where to equip each.
    claudia.equip('Dagger', 0)
    claudia.equip('Dagger', 1)
    claudia.equip('Shirt')

    # give a manual, and some pills
    stats.inventory.give('Manual')
    stats.inventory.give('Aspirin', 3)
    stats.inventory.give('Buffrin')

    stats.giveMoney(1000000)

    # and start the map engine up
    field.mapSwitch('towncity-interior.ika-map', 0, 0, fade = False)
    field.spawnPlayer(clyde.spriteName, 13*16, 18 * 16)

    effects.fadeIn(50)
