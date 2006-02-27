import ikamap
import noisetool

X1 = Y1 = X2 = Y2 = -1



def OnMouseDown(x, y):
    global buttonDown, X1, Y1
    X1, Y1 = ika.Editor.MapToTile(x, y)


def OnMouseUp(x, y):
    global buttonDown, X1, Y1, X2, Y2
    X2, Y2 = ika.Editor.MapToTile(x, y)

    for x in range(min(X1, X2), max(X1, X2) + 1):
        for y in range(min(Y1, Y2), max(Y1, Y2) + 1):
            ika.Map.SetTile(x, y, ika.Editor.curlayer, noisetool.RandomTile(ika.Editor.curtile))



