
#include "scriptstate.h"
#include "script.h"
#include <cassert>

#include "mapview.h"

ScriptState::ScriptState(MainWindow* mainWnd)
    : EditState(mainWnd)
    , _script(0)
{
}

void ScriptState::SetScript(Script* script)
{
    // TODO: make sure that this isn't called while the script is still in use or something similarly bizzare.
    _script = script;
}

void ScriptState::OnMouseDown(wxMouseEvent& event)
{
    assert(_script);
    GetMapView()->ScreenToMap(event.m_x, event.m_y);
    _script->OnMouseDown(event.GetX(), event.GetY());
}

void ScriptState::OnMouseUp(wxMouseEvent& event)
{
    assert(_script);
    GetMapView()->ScreenToMap(event.m_x, event.m_y);
    _script->OnMouseUp(event.GetX(), event.GetY());
}

void ScriptState::OnMouseMove(wxMouseEvent& event)
{
    assert(_script);
    GetMapView()->ScreenToMap(event.m_x, event.m_y);
    _script->OnMouseMove(event.GetX(), event.GetY());
}

void ScriptState::OnBeginState()
{
    assert(_script);
    _script->OnBeginState();
}

void ScriptState::OnEndState()
{
    assert(_script);
    _script->OnEndState();
}

void ScriptState::OnMouseWheel(wxMouseEvent& event)
{
    assert(_script);
    GetMapView()->ScreenToMap(event.m_x, event.m_y);
    _script->OnMouseWheel(event.GetX(), event.GetY(), event.GetWheelRotation() / event.GetWheelDelta());
}

void ScriptState::OnRender()
{
    assert(_script);
    _script->OnRender();
}

void ScriptState::OnRenderCurrentLayer()
{
    assert(_script);
    _script->OnRenderCurrentLayer();
}

void ScriptState::OnSwitchLayers(uint oldLayer, uint newLayer)
{
    assert(_script);
    _script->OnSwitchLayers(oldLayer, newLayer);
}