#ifndef SCRIPTSTATE_H
#define SCRIPTSTATE_H

/**
 * When this state is active, we use a python script to decide what happens.
 * YOU WANT FLEXIBILITY.  HERE'S YOUR GODDAMN FLEXIBILITY :D
 */

#include "editstate.h"

struct Script;

struct ScriptState : public EditState
{
private:
    Script* _script;

public:
    ScriptState(MainWindow* mainWnd);

    void SetScript(Script* script);
    
    virtual void OnMouseDown(wxMouseEvent& event);
    virtual void OnMouseUp(wxMouseEvent& event);
    virtual void OnMouseMove(wxMouseEvent& event);
  
    virtual void OnBeginState();
    virtual void OnEndState();

    virtual void OnMouseWheel(wxMouseEvent& event);
    virtual void OnRender();
    virtual void OnRenderCurrentLayer();
    virtual void OnSwitchLayers(uint oldLayer, uint newLayer);
};

#endif