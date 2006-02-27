'''
Mass-obstructifier.

By telling the script which tiles should be obstructed, this
script will obstruct all tiles that have any of those indeces,
so long as they are adjacent to a tile that should not be obstructed.
(this way, you don't get gigantic solid blocks of obstructions that
don't do anything but slow ikaMap down)
'''

import ikamap

# When scripts can prompt the user for information, I'll add a dialog for defining these on the fly.
impassable = (
    0, 24
)
#(0, 103, 104, 105, 106, 121, 124, 139, 142, 157, 158, 159, 160)

def OnActivated():
    meta = ikamap.Map.GetMetaData()
    obsLay = meta.get('entitylayer')
    if obsLay:
        obsLay = ikamap.Map.FindLayerByName(obsLay)
    else:
        obsLay = ikamap.Editor.curlayer

    tileLay = ikamap.Editor.curlayer

    for y in range(100):
        for x in range(100):
            gt = lambda x, y: ikamap.Map.GetTile(x, y, tileLay)
            tile = gt(x, y)
            adjacent = (
                gt(x    , y - 1),       # up
                gt(x - 1, y    ),       # left
                gt(x + 1, y    ),       # right
                gt(x    , y + 1)        # bottom
                )

            # filter out all the adjacent impassable tiles
            passable = [a for a in adjacent if a not in impassable]

            # if the tile is adjacent to a tile that is passable, then we obstruct it.
            # That way, we don't have to obstruct big huge expanses of nothing.
            if tile in impassable and bool(passable):
                ikamap.Map.SetObs(x, y, obsLay, True)
