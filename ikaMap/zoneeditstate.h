#ifndef ZONEEDITSTATE_H
#define ZONEEDITSTATE_H

#include "editstate.h"

class ZoneEditState : public EditState
{
    uint _curZoneIndex;

    bool _dragging;     // true if dragging a rect for a new zone
    Rect _selection;

public:
    ZoneEditState(MainWindow* mw);

    virtual void OnMouseDown(wxMouseEvent& event);
    virtual void OnMouseUp(wxMouseEvent& event);
    virtual void OnMouseMove(wxMouseEvent& event);

    virtual void OnRenderCurrentLayer();
    virtual void OnSwitchLayers(uint oldLayer, uint newLayer);
};

#endif