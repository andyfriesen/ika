
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


    void SetTile(int x, int y);
    Matrix<uint>& GetCurBrush() const;
    void SetCurBrush(Matrix<uint>& b);

private:
    int _oldX, _oldY;
    int _curX, _curY;
    int _offsetX, _offsetY;
    int _dragX, _dragY;
    bool _dragging;
    Rect _selection;

    CompositeCommand* _curGroup;
};

#endif