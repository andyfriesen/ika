
#ifndef ENTITYSTATE_H
#define ENTITYSTATE_H

#include "editstate.h"
#include "map.h"

class EntityState : public EditState
{
private:
    Map::Layer::Entity* _curEnt;

public:
    EntityState(MainWindow* mw);

    virtual void OnMouseDown(wxMouseEvent& event);
    virtual void OnMouseUp(wxMouseEvent& event);
    virtual void OnMouseMove(wxMouseEvent& event);
    virtual void OnMouseWheel(wxMouseEvent& event);

    virtual void OnRender();
};

#endif