'''A simple iterative flood fill tool. Easy to use, simply import
it into ikaMap, turn on ikaMap's script mode, select the tile
you want to fill with, and click. Note that fills only affect the
currently selected layer.

Thanks to Hatchet for writing the original, recursive version.

- CYwolf
'''

import ikamap

def Fill(tx, ty):
    layer = ikamap.Editor.curlayer # currently selected layer
    w, h = ikamap.Map.GetLayerSize(layer) # dimensions of current layer
    newtile = ikamap.Editor.curtile  # replacement tile index
    oldtile = ikamap.Map.GetTile(tx, ty, layer)  # tile index to be replaced
    tiles = [(tx, ty)] # list of tiles to modify
    steptiles = [(tx, ty)]
    nextstep = []
    while(1):
        for x,y in steptiles:
            for x2,y2 in [(x+1, y), (x-1, y), (x, y+1), (x, y-1)]:
                if (x2, y2) not in tiles and (x2, y2) not in nextstep \
                  and x2 >= 0 and y2 >= 0 and x2 < w and y2 < h \
                  and ikamap.Map.GetTile(x2, y2, layer) == oldtile:
                    nextstep.append((x2, y2))
        tiles.extend(steptiles)
        if not nextstep:
            break
        steptiles = nextstep
        nextstep = []
    for x,y in tiles:
        ikamap.Map.SetTile(x, y, layer, newtile)

def OnMouseDown(mx, my): #called when the mouse is clicked
    tx, ty = ikamap.Editor.MapToTile(mx, my)  # get coordinates of clicked tile
    Fill(tx, ty)