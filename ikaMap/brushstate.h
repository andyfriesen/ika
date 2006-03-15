
#ifndef BRUSHSTATE_H
#define BRUSHSTATE_H

#include "editstate.h"

struct Executor;

struct BrushState : EditState {
    BrushState(Executor* e);

    virtual void OnMouseDown(wxMouseEvent& event);
    virtual void OnMouseUp(wxMouseEvent& event);
    virtual void OnMouseMove(wxMouseEvent& event);

    virtual void OnRenderCurrentLayer();

private:
    int _oldX;
    int _oldY;
};

#endif
