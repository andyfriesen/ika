
#ifndef OBSTRUCTIONSTATE_H
#define OBSTRUCTIONSTATE_H

#include "editstate.h"

class ObstructionState : public EditState
{
    int _oldX;
    int _oldY;
public:
    ObstructionState(MainWindow* mw);

    virtual void OnMouseDown(wxMouseEvent& event);
    virtual void OnMouseUp(wxMouseEvent& event);
    virtual void OnMouseMove(wxMouseEvent& event);
    virtual void OnMouseWheel(wxMouseEvent& event);
    virtual void OnRender();
};

#endif