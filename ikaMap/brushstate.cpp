
#include "brushstate.h"
#include "executor.h"
#include "mapview.h"
#include "command.h"

BrushState::BrushState(Executor* e)
    : EditState(e)
    , _oldX(-1)
    , _oldY(-1)
{}

void BrushState::OnMouseDown(wxMouseEvent& event) {
    int x = event.m_x;
    int y = event.m_y;
    GetMapView()->ScreenToTile(x, y);

    HandleCommand(new PasteBrushCommand(x, y, GetCurLayerIndex(), GetExecutor()->GetCurrentBrush()));
}

void BrushState::OnMouseUp(wxMouseEvent& event) {
}

void BrushState::OnMouseMove(wxMouseEvent& event) {
    int x = event.m_x;
    int y = event.m_y;

    GetMapView()->ScreenToTile(x, y);
    if (x == _oldX && y == _oldY) {
        return;
    }

    _oldX = x;
    _oldY = y;
    GetMapView()->Refresh();
}

void BrushState::OnRenderCurrentLayer() {
    if (_oldX == -1 || _oldY == -1) {
        return;
    }

    GetMapView()->RenderBrush(_oldX, _oldY);
}
