'''A simple tool to replace one tile with another.
Coded by Andy Friesen
7 April 2004

To use the script, click the "Activate" button to the left, and
specify the tile to search for, and what to replace it with.

Replacement is only done on the currently active layer.

If either field is empty or an invalid integer, nothing is done.
'''

import ikamap

def OnActivated():
    result = ikamap.ShowDialog(
        1, 'Replace tiles',
        (
            ('Replace', 'edit'),
            ('With', 'edit')
        )
    )

    replace = with = None
    try:
        replace = int(result['Replace'])
        with = int(result['With'])
    except ValueError:
        return

    lay = ikamap.Editor.curlayer
    width, height = ikamap.Map.GetLayerSize(lay)

    for y in xrange(height):
        for x in xrange(width):
            if ikamap.Map.GetTile(x, y, lay) == replace:
                ikamap.Map.SetTile(x, y, lay, with)