
#ifndef ENTITYSTATE_H
#define ENTITYSTATE_H

#include "editstate.h"
#include "common/map.h"

class EntityState : public EditState
{
private:
    uint _entLayerIndex;    // the layer that the current entity lives on
    uint _entIndex;         // the index of the current entity

public:
    EntityState(Executor* e);

    virtual void OnMouseDown(wxMouseEvent& event);
    virtual void OnMouseUp(wxMouseEvent& event);
    virtual void OnMouseMove(wxMouseEvent& event);
    virtual void OnMouseWheel(wxMouseEvent& event);
    virtual void OnKeyPress(wxKeyEvent& event);

    virtual void OnRenderCurrentLayer();
    virtual void OnSwitchLayers(uint oldLayer, uint newLayer);
};

#endif