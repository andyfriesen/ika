
from mapsys import *

def AutoExec():
    weather.cloudWeather()
    music.playMusic('music/UNKNOWN - Call to the Sun.it')

def TownCity():
    weather.endWeather()
    field.mapSwitch('towncity.ika-map', 38 * 16, 19 * 16)

def Bridge():
    text('The Cave of Ultimate Oblivion is to the north of here.',
        portrait=stats.characters['clyde'].portrait)
    field.player.MoveTo(field.player.x, field.player.y - 16)

def OblivionCave():
    #text("This cave isn't ready either!\nApologies!")
    #return
    weather.endWeather()
    field.mapSwitch('cave1.ika-map', 2 * 16, 33 * 16)
