"""
A simple ikaMap tool for painting random tiles.

It's pretty sucky right now, though, because
I'm too lazy to look up a random number generating
function.
"""

import ikamap
import random

#tiles now can handle many groups of tiles!
tiles = [
       [ #group 1, grass
        ( 7, 33),
        ( 8, 33),
        ( 9, 34),
    ], [ #group 2, gravel?
        ( 5, 70),
        ( 6, 30),
    ], [ # dirt
        (1, 25),
        (2, 25),
        (3, 25),
        (4, 25),
    ]
]

bleh = [[ #group 3, patches/rock/grass
        ( 75, 1),
        (102, 1),
        (103, 1),
        (104, 1),
        (105, 3),
        (106, 3),
    ], [ #group 4, ice
        (108, 13),
        (109, 17),
        (110, 17),
        (111, 13),
        (112, 17),
        (113, 23),
    ], [ #group 5, dark ice
        (128, 25),
        (129, 25),
        (134, 25),
        (135, 25),
    ], [ #group 6, water
        (150, 25),
        (151, 25),
        (156, 25),
        (157, 25),
    ], [ #group 7, cave floor tiles
        (242, 20),
        (243, 20),
        (247, 15),
        (248, 15),
        (249, 15),
        (250, 15),
    ], [ #group 8, cave floor patches
        (296, 25),
        (297, 25),
        (298, 25),
        (299, 25),
    ], [ #group 9 cave wall top fringe
        (218, 50),
        (219, 50),
    ], [ #group 10 cave wall top top
        (224, 50),
        (225, 50)
    ], [ #group 11 cave wall top middle
        (230, 50),
        (231, 50)
    ], [ #group 12 cave wall top bottom
        (236, 50),
        (237, 50)
    ], [ #group 13 cave wall left
        (252, 50),
        (253, 50),
    ], [ #group 14 cave wall bottom
        (254, 50),
        (255, 50),
    ], [ #group 15 cave wall right
        (256, 50),
        (257, 50),
    ], [ #group 16 pit top edge
        (259, 50),
        (260, 50),
    ], [ #group 17 pit top descent
        (265, 50),
        (266, 50),
    ], [ #group 18 pit bottom
        (262, 50),
        (263, 50),
    ], [ #group 19 pit left
        (268, 50),
        (274, 50),
    ], [ #group 20 pit right
        (269, 50),
        (275, 50),
    ]
]


buttonDown = False
oldX = -1
oldY = -1


def RandomTile(t):
   global tiles

   for group in tiles:
      for a in group:
         if a[0] == t: #tile is in the range
            return ChooseTile(group)

   return t


def ChooseTile(tilegroup):
   weight = random.randint(0, 100)
   for tile in tilegroup:
      if weight < tile[1]:
         return tile[0]
      weight -= tile[1]

   return tilegroup[0][0]


def Draw(x, y):
    global oldX, oldY

    x, y = ika.Editor.MapToTile(x, y)

    if x == oldX and y == oldY:
        return
    oldX, oldY = x, y

    ika.Map.SetTile(x, y, ika.Editor.curlayer, RandomTile(ika.Editor.curtile))


def OnMouseDown(x, y):
    global buttonDown
    buttonDown = True
    Draw(x, y)


def OnMouseMove(x, y):
    if buttonDown:
        Draw(x, y)


def OnMouseUp(x, y):
    global buttonDown, oldX, oldY
    buttonDown = False
    oldX = -1
    oldY = -1
