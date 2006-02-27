'''
Simple filter thingo for setting all the shore tiles.
'''

import ikamap

def mat33(*args):
    assert len(args) == 9
    return (args[0:3], args[3:6], args[6:9])

# includes all manner of tile bordery.
WATER = (24, 48, 49, 56, 57) + tuple(range(64, 96))

# egh
# 0 is water
# X is not
# E means we don't care
X = 1
E = -1
map = {
    mat33(
        E, X, E,
        X, 0, X,
        E, 0, E) : 48,
    mat33(
        E, 0, E,
        X, 0, X,
        E, X, E) : 49,
    mat33(
        E, X, E,
        X, 0, 0,
        E, X, E) : 56,
    mat33(
        E, X, E,
        0, 0, X,
        E, X, E) : 57,

    mat33(
        E, X, E,
        X, 0, 0,
        E, 0, 0) : 64,
    mat33(
        E, X, E,
        0, 0, X,
        0, 0, E) : 65,
    mat33(
        E, X, E,
        X, 0, 0,
        E, 0, X) : 66,
    mat33(
        E, X, E,
        0, 0, X,
        X, 0, E) : 67,

    mat33(
        E, X, E,
        0, 0, 0,
        E, X, E) : 68,
    mat33(
        E, 0, E,
        X, 0, X,
        E, 0, E) : 69,

    mat33(
        X, 0, E,
        0, 0, X,
        X, 0, E) : 70,
    mat33(
        E, 0, X,
        X, 0, 0,
        E, 0, X) : 71,

    mat33(
        E, 0, 0,
        X, 0, 0,
        E, X, E) : 72,
    mat33(
        0, 0, E,
        0, 0, X,
        E, X, E) : 73,

    mat33(
        E, 0, X,
        X, 0, 0,
        E, X, E) : 74,
    mat33(
        X, 0, E,
        0, 0, X,
        E, X, E) : 75,

    mat33(
        X, 0, X,
        0, 0, 0,
        E, X, E) : 78,
    mat33(
        E, X, E,
        0, 0, 0,
        X, 0, X) : 79,

    # 80 and 81 are just like 64 and 65

    mat33(
        E, 0, 0,
        X, 0, 0,
        E, 0, 0) : 82,
    mat33(
        0, 0, E,
        0, 0, X,
        0, 0, E) : 83,

    mat33(
        X, 0, X,
        0, 0, 0,
        X, 0, 0) : 84,
    mat33(
        X, 0, X,
        0, 0, 0,
        0, 0, X) : 85,

    mat33(
        X, 0, 0,
        0, 0, 0,
        X, 0, 0) : 86,
    mat33(
        0, 0, X,
        0, 0, 0,
        0, 0, X) : 87,

    # 88 and 89 go along with 80 and 81

    mat33(
        E, X, E,
        0, 0, 0,
        0, 0, 0) : 90,
    mat33(
        0, 0, 0,
        0, 0, 0,
        E, X, E) : 91,

    mat33(
        X, 0, 0,
        0, 0, 0,
        X, 0, X) : 92,
    mat33(
        0, 0, X,
        0, 0, 0,
        X, 0, X) : 93,

    mat33(
        X, 0, X,
        0, 0, 0,
        0, 0, 0) : 94,
    mat33(
        0, 0, 0,
        0, 0, 0,
        X, 0, X) : 95,

    mat33(
        0, 0, 0,
        0, 0, 0,
        0, 0, X) : 32,
    mat33(
        0, 0, 0,
        0, 0, 0,
        X, 0, 0) : 33,
    mat33(
        0, 0, X,
        0, 0, 0,
        0, 0, 0) : 40,
    mat33(
        X, 0, 0,
        0, 0, 0,
        0, 0, 0) : 41,
    mat33(
        X, 0, X,
        0, 0, 0,
        X, 0, X) : 41,

    # lastly, the trivial case:
    mat33(
        0, 0, 0,
        0, 0, 0,
        0, 0, 0) : 24,
}

down = False

def OnActivated():
    pass

def OnMouseUp(*args):
    global down
    down = False

def OnMouseMove(*args):
    if down:
        OnMouseDown(*args)

def OnMouseDown(mx, my):
    global down
    down = True
    tx, ty = ikamap.Editor.MapToTile(mx, my)
    FixTile(tx, ty)

def _IsWater(t):
    if t in WATER:
        return 1
    else:
        return 0

def _IsGround(t):
    return t == 1

def _Check(m1, m2):
    for row1, row2 in zip(m1, m2):
        for e1, e2 in zip(row1, row2):
            if e1 != e2 and e2 != E:
                return False
    return True

def _Match(tiles):
    global map
    keys = map.keys()

    for k in keys:
        if _Check(tiles, k):
            return map[k]
    return None

def FixTile(x, y):
    lay = ikamap.Editor.curlayer

    tiles = mat33(*([_IsGround(ikamap.Map.GetTile(X, Y, lay)) for (X, Y) in (
        (x - 1, y - 1), (x, y - 1), (x + 1,y - 1),
        (x - 1, y    ), (x, y    ), (x + 1,y    ),
        (x - 1, y + 1), (x, y + 1), (x + 1,y + 1),
    )]))

    m = _Match(tiles)

    if m:
        ikamap.Map.SetTile(x, y, lay, m)
