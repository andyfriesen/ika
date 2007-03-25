
#ifndef TILESETSTATE_H
#define TILESETSTATE_H

#include "common/utility.h"
#include "editstate.h"

struct CompositeCommand;

struct TilesetState : EditState
{
public:
    TilesetState(Executor* e);

    virtual void OnMouseDown(wxMouseEvent& event);
    virtual void OnMouseUp(wxMouseEvent& event);
    virtual void OnMouseMove(wxMouseEvent& event);
    virtual void OnMouseWheel(wxMouseEvent& event);
    virtual void OnRender();

    virtual void OnRenderCurrentLayer();
    virtual void OnTilesetViewRender();


    void SetTile(int x, int y);
    uint GetCurTile() const;
    void SetCurTile(uint tile);

private:
    int _oldX, _oldY;
    int _curX, _curY;

    CompositeCommand* _curGroup;
};

#endif