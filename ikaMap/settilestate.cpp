
#include "settilestate.h"
#include "wxinc.h"
#include "mainwindow.h"
#include "mapview.h"
#include "tilesetview.h"
#include "map.h"
#include "tileset.h"
#include "command.h"
#include "misc.h"

TileSetState::TileSetState(MainWindow* mw)
    : EditState(mw)
    , _oldX(-1)
    , _oldY(-1)
{}

void TileSetState::OnMouseDown(wxMouseEvent& event)
{
    if (event.ShiftDown())
    {
        int x = event.m_x;
        int y = event.m_y;
        GetMapView()->ScreenToTile(x, y);

        SetCurTile(GetCurLayer()->tiles(x, y));
        GetTileSetView()->Render();
        GetTileSetView()->ShowPage();
    }
    else
        SetTile(event.m_x, event.m_y);
}

void TileSetState::OnMouseUp(wxMouseEvent& event)
{
    _oldX = _oldY = -1;
}

void TileSetState::OnMouseMove(wxMouseEvent& event)
{
    int x = event.GetX();
    int y = event.GetY();
    GetMapView()->ScreenToTile(x, y);
    GetMainWindow()->GetStatusBar()->SetStatusText(va("(%i, %i)", x, y));

    if (event.LeftIsDown() && !event.ShiftDown())
        SetTile(event.m_x, event.m_y);
}

void TileSetState::OnMouseWheel(wxMouseEvent& event)
{
    int i = event.GetWheelRotation() / event.GetWheelDelta();
    uint curTile = GetCurTile();

    // _curTile is unsigned, so we need to check for wraparound before actually doing the math.
    // So we just add the tile count, then subtract it afterwards, if needed
    curTile += GetTileSet()->Count();
    curTile += i;

    // If we've gone over, wrap-around.
    while (curTile >= GetTileSet()->Count()) 
        curTile -= GetTileSet()->Count();

    SetCurTile(curTile);
}

void TileSetState::SetTile(int x, int y)
{
    GetMapView()->ScreenToTile(x, y);

    if (x == _oldX && y == _oldY)   return;
    if (x < 0 || y < 0)             return;
    if ((uint)x >= GetCurLayer()->Width() ||
        (uint)y >= GetCurLayer()->Height())
        return;

    _oldX = x; _oldY = y;

    HandleCommand(new SetTileCommand(x, y, GetCurLayerIndex(), GetCurTile()));
}

uint TileSetState::GetCurTile() const
{
    return GetTileSetView()->GetCurTile();
}

void TileSetState::SetCurTile(uint t)
{
    GetTileSetView()->SetCurTile(t);
}