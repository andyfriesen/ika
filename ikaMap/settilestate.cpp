
#include "settilestate.h"
#include "wxinc.h"
#include "executor.h"
#include "mapview.h"
#include "tilesetview.h"
#include "map.h"
#include "tileset.h"
#include "command.h"
#include "misc.h"
#include "video.h"
#include "wxinc.h"

#include "log.h"

TileSetState::TileSetState(Executor* e)
    : EditState(e)
    , _oldX(-1)
    , _oldY(-1)
    , _curGroup(0)
{}

void TileSetState::OnRenderCurrentLayer()
{
    /*
    wxPoint mousePos = GetMapView()->ScreenToClient(::wxGetMousePosition());
    GetMapView()->ScreenToTile(mousePos.x, mousePos.y);	

    int w = GetTileSet()->Width();
    int h = GetTileSet()->Height();	
    GetMapView()->GetVideo()->Rect(mousePos.x*w, mousePos.y*h, w, h, RGBA(255, 192, 192, 255));
    */
}

void TileSetState::OnMouseDown(wxMouseEvent& event)
{
    if (event.ShiftDown())
    {
        int x = event.m_x;
        int y = event.m_y;
        GetMapView()->ScreenToTile(x, y);

        SetCurTile(GetCurLayer()->tiles(x, y));
    }
    else
    {
        SetTile(event.m_x, event.m_y);
    }
}

void TileSetState::OnMouseUp(wxMouseEvent& event)
{
	_oldX = _oldY = -1;
        // odd that the mouse button is able to go up without first going down
        if (_curGroup)
        {
            if (_curGroup->GetCount() == 1)
            {
                GetExecutor()->AddCommand(_curGroup->GetIndex(0));
                delete _curGroup;
            }
            else
                GetExecutor()->AddCommand(_curGroup);
        }
        _curGroup = 0;
}

void TileSetState::OnMouseMove(wxMouseEvent& event)
{
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

    if (GetCurLayer()->tiles(x, y) == GetCurTile()) return; // don't flood the undo buffer with commands that don't actually do anything

    // Create a new group if we need to
    if (!_curGroup)
        _curGroup = new CompositeCommand();

    Command* cmd = new SetTileCommand(x, y, GetCurLayerIndex(), GetCurTile());
    //HandleCommand(c);
    // naughty.  execute the command, but don't put it on the undo stack
    // when the mouse button is released, we add the list of commands all in one go.
    cmd->Do(GetExecutor());
    _curGroup->Append(cmd);
}

uint TileSetState::GetCurTile() const
{
    return GetExecutor()->GetCurrentTile();
}

void TileSetState::SetCurTile(uint t)
{
    GetExecutor()->SetCurrentTile(t);
}
