
#include "obstructionstate.h"

#include "wxinc.h"
#include "mapview.h"

ObstructionState::ObstructionState(MainWindow* mw)
    : EditState(mw)
    , _oldX(-1)
    , _oldY(-1)
{}

void ObstructionState::OnMouseDown(wxMouseEvent& event)
{
    int x = event.GetX();
    int y = event.GetY();
    GetMapView()->ScreenToTile(x, y);

    if (x == _oldX && y == _oldY)
        return;

    _oldX = x;
    _oldY = y;

    /*
     * yay for boolean logic.
     * Left == set.
     * Right == unset.
     * Both == unset.
     * Neither == execution will never get here.
     */
    GetCurrentLayer()->obstructions(x, y) =
        event.LeftIsDown() && !event.RightIsDown();

    GetMapView()->Render();
    GetMapView()->ShowPage();
}

void ObstructionState::OnMouseUp(wxMouseEvent& event)
{
    _oldX = _oldY = -1;
}

void ObstructionState::OnMouseMove(wxMouseEvent& event)
{
    if (event.LeftIsDown() || event.RightIsDown())
        OnMouseDown(event);
}

void ObstructionState::OnMouseWheel(wxMouseEvent& event)
{
}

void ObstructionState::OnRender()
{
    Map::Layer* l = GetCurrentLayer();
    MapView* mv = GetMapView();

    mv->RenderObstructions(GetCurrentLayer(), mv->GetXWin() - l->x, mv->GetYWin() - l->y);
}
