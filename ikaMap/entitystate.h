
#ifndef ENTITYSTATE_H
#define ENTITYSTATE_H

#include "editstate.h"
#include "map.h"

class EntityState : public EditState
{
private:
    uint _entLayerIndex;    // the layer that the current entity lives on
    uint _entIndex;         // the index of the current entity

public:
    EntityState(MainWindow* mw);

    virtual void OnMouseDown(wxMouseEvent& event);
    virtual void OnMouseUp(wxMouseEvent& event);
    virtual void OnMouseMove(wxMouseEvent& event);
    virtual void OnMouseWheel(wxMouseEvent& event);

    virtual void OnRenderCurrentLayer();
};

#endif