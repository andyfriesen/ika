
#ifndef TILESETSTATE_H
#define TILESETSTATE_H

#include "types.h"
#include "editstate.h"

class TileSetState : public EditState
{
public:
    TileSetState(MainWindow* mw);

    virtual void OnMouseDown(wxMouseEvent& event);
    virtual void OnMouseUp(wxMouseEvent& event);
    virtual void OnMouseMove(wxMouseEvent& event);
    virtual void OnMouseWheel(wxMouseEvent& event);

    // lowest common denominator, since more than one of the above methods will be setting tiles.
    // x and y are still in client coordinates.  _curTile is the tile that is set.
    void SetTile(int x, int y);

    uint GetCurTile() const;
    void SetCurTile(uint t);

private:
    int _oldX, _oldY;
};

#endif