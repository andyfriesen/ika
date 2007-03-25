
#ifndef NULLSTATE_H
#define NULLSTATE_H

#include "editstate.h"

struct NullState : EditState {
    NullState(Executor* e)
        : EditState(e, "NullState")
    {}

    std::string GetName();

    virtual void OnMouseDown(wxMouseEvent&) {}
    virtual void OnMouseUp(wxMouseEvent&) {}
    virtual void OnMouseMove(wxMouseEvent&) {}

    virtual void OnBeginState() {}
    virtual void OnEndState() {}

    virtual void OnMouseWheel(wxMouseEvent&) {}
    virtual void OnRender() {}
    virtual void OnRenderCurrentLayer() {}
    virtual void OnTilesetViewRender() {}
    virtual void OnSwitchLayers(unsigned int oldLayer, unsigned int newLayer) {}
    virtual void OnKeyPress(wxKeyEvent&) {}
};

#endif
