
#include "obstructionstate.h"

#include "wxinc.h"
#include "mapview.h"
#include "command.h"

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

    /*
     * yay for boolean logic.
     * Left == set.
     * Right == unset.
     * Both == unset.
     * Neither == execution will never get here anyway.
     */
    u8 set = event.LeftIsDown() && !event.RightIsDown();

    _oldX = x;
    _oldY = y;

    // If we're not actually going to change anything, then do not send the command.
    if (GetCurLayer()->obstructions(x, y) != set)
    {
        HandleCommand(new SetObstructionCommand(x, y, GetCurLayerIndex(),
            event.LeftIsDown() && !event.RightIsDown()));
    }
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
    Map::Layer* l = GetCurLayer();
    MapView* mv = GetMapView();

    if (l)
        mv->RenderObstructions(GetCurLayer(), mv->GetXWin() - l->x, mv->GetYWin() - l->y);
}
