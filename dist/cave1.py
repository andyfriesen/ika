from mapsys import *

from menu import util

def AutoExec():
    if 'cave1 switch1' in flags:
        ClearBlock1()
    if 'cave1 switch2' in flags:
        ClearBlock2()
    if 'cave1 switch3' in flags:
        ClearBlock3()

    initBox('Box1', 'cave1 box1', Box1)

def toWorldMap():
    field.mapSwitch('overworld.ika-map', 152 * 16, 36 * 16)

def BigSign():
    text("Wow. That's a pretty big sign.")

def Switch1():
    flags['cave1 switch1'] = True
    ClearBlock1()

def Switch2():
    flags['cave1 switch2'] = True
    ClearBlock2()

def Switch3():
    flags['cave1 switch3'] = True
    ClearBlock3()

def Box1():
    util.message(['  Got:', 'Aspirin x3'])
    stats.inventory.give('Aspirin', 3)


def ClearBlock1():
    for y in range(19, 24):
        for x in range(13, 16):
            ika.Map.SetTile(x, y, 1, 0)
            ika.Map.SetObs(x, y, 1, False)
    ika.Map.entities['Switch1'].specframe = 1

def ClearBlock2():
    for y in range(31, 35):
        for x in range(21, 23):
            ika.Map.SetTile(x, y, 2, 0)
    for y in range(34, 36):
        for x in range(21, 23):
            ika.Map.SetTile(x, y, 1, 0)
            ika.Map.SetObs(x, y, 1, False)
    ika.Map.entities['Switch2'].specframe = 1

def ClearBlock3():
    for y in range(8, 11):
        for x in range(1, 5):
            ika.Map.SetTile(x, y, 1, 0)
            ika.Map.SetObs(x, y, 1, False)
    ika.Map.entities['Switch3'].specframe = 1

