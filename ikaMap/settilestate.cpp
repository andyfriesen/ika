
#include "settilestate.h"
#include "wxinc.h"
#include "executor.h"
#include "mapview.h"
#include "tilesetview.h"
#include "common/map.h"
#include "tileset.h"
#include "command.h"
#include "common/utility.h"
#include "video.h"
#include "wxinc.h"

#include "common/log.h"

#undef USE_GROUP_JUJU

TilesetState::TilesetState(Executor* e)
    : EditState(e)
    , _oldX(-1)
    , _oldY(-1)
    , _curX(0)
    , _curY(0)
    , _dragX(0)
    , _dragY(0)
    , _dragging(false)
    , _offsetX(0)
    , _offsetY(0)
    , _curGroup(0)
{}

void TilesetState::OnRender()

{

    if (_dragging)
    {

        Tileset* ts = GetExecutor()->GetTileset();

        uint width = ts->Width();

        uint height = ts->Height();



        GetMapView()->GetVideo()->DrawSelectRect(
            _selection.left * width - GetMapView()->GetXWin(),
            _selection.top * height - GetMapView()->GetYWin(),
            (_selection.right - _selection.left) * width,
            (_selection.bottom - _selection.top) * height,
            RGBA(255, 255, 255, 127));

    }

    else

    {

        GetMapView()->RenderBrush(_curX, _curY);

    }

}



void TilesetState::OnRenderCurrentLayer()
{
    /*
    wxPoint mousePos = GetMapView()->ScreenToClient(::wxGetMousePosition());
    GetMapView()->ScreenToTile(mousePos.x, mousePos.y);	

    int w = GetTileset()->Width();
    int h = GetTileset()->Height();	
    GetMapView()->GetVideo()->Rect(mousePos.x*w, mousePos.y*h, w, h, RGBA(255, 192, 192, 255));
    */
}

void TilesetState::OnMouseDown(wxMouseEvent& event)
{
    if (event.RightDown() && !_dragging)
    {
        _selection.left = event.GetX();
        _selection.top = event.GetY();
        GetMapView()->ScreenToTile(_selection.left, _selection.top);
        _selection.right = _selection.left;
        _selection.bottom = _selection.top;


        _dragging = true;



        // Set base tile in tilesetview.
        int index = GetCurLayer()->tiles(_selection.left, _selection.top);

        GetTilesetView()->SetSelectionTile(index);

        GetTilesetView()->UpdateBrush();

        //SetCurTile(GetCurLayer()->tiles(x, y));
    }
    else if (event.LeftDown())
    {
        SetTile(event.m_x, event.m_y);
    }
}

void TilesetState::OnMouseUp(wxMouseEvent& event)
{
	_oldX = _oldY = -1;
    if (_dragging && !event.RightIsDown())
    {
        Map::Layer* layer = GetCurLayer();

        _dragging = false;

        _selection.Normalize();
        _curX = _selection.left;
        _curY = _selection.top;


        if (_selection.Width() > 1 || _selection.Height() > 1)
        {
            Matrix<uint>& brush = GetExecutor()->GetCurrentBrush();
            brush.Resize(_selection.Width(), _selection.Height());

            for (uint y = 0; y < brush.Height(); y++)
            {
                const uint sourceY = y + _selection.top;
                uint sourceX = _selection.left;

                for (uint x = 0; x < brush.Width(); x++)
                {
                    brush(x, y) = layer->tiles(sourceX, sourceY);
                    sourceX++;
                }
            }

            //Matrix<uint> tempMat(_tiles.Width(), _tiles.Height());
            //HandleCommand(new PasteTilesCommand(_selX, _selY, GetCurLayerIndex(), tempMat));

            //_clipboard = _tiles;    // save this for duplication goodness
            GetExecutor()->SetCurrentBrush(brush);
        }
        // If there's no selection rect, we just un-select.
        // An unfortunate side effect of the way things are set up is that you cannot copy/paste a single tile.
        // BOO FUCKING HOO.  Use the painter for that. ;)

        GetMapView()->Refresh();
    }
        // odd that the mouse button is able to go up without first going down
#if defined(USE_GROUP_JUJU)
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
#endif
}

void TilesetState::OnMouseMove(wxMouseEvent& event)
{
    Map::Layer* layer = GetCurLayer();
    uint width = layer->Width() - 1;
    uint height = layer->Height() - 1;
    if (_dragging)
    {
        _selection.right = event.m_x;
        _selection.bottom = event.m_y;
        GetMapView()->ScreenToTile(_selection.right, _selection.bottom);
        if (_selection.right > width) _selection.right = width;
        if (_selection.bottom > height) _selection.bottom = height;
        if (_selection.right >= _selection.left)    _selection.right++;
        if (_selection.bottom >= _selection.top)    _selection.bottom++;
        if (_selection.right)
        
        GetMapView()->Refresh();
    }
    else
    {
        int x = event.m_x;
        int y = event.m_y;
        GetMapView()->ScreenToTile(x, y);
        if (x <= width && y <= height) 
        {
            _curX = x;
            _curY = y;
            GetMapView()->Refresh();
        }
    /*
    Map::Layer* layer = GetCurLayer();
    Matrix<uint>& brush = GetExecutor()->GetCurrentBrush();
    int w = GetTileset()->Width();
    int h = GetTileset()->Height();	
    uint new_tx = event.m_x / GetTileset()->Width();
    if (new_tx < 0) new_tx = 0;
    int max_tx = layer->Width() - brush.Width();
    if (max_tx < 0) max_tx = 0;
    if (new_tx > max_tx) new_tx = max_tx;
    uint new_ty = event.m_y / GetTileset()->Height();
    if (new_ty < 0) new_ty = 0;
    int max_ty = layer->Height() - brush.Height();
    if (max_ty < 0) max_ty = 0;
    if (new_ty > max_ty) new_ty = max_ty;
    if (new_tx != _tx || new_ty != _ty) {
        _tx = new_tx;
        _ty = new_ty;
        GetMapView()->Refresh();
    }
    */
    }

    if (event.LeftIsDown() && !event.RightIsDown())
        SetTile(event.m_x, event.m_y);
    else if (event.RightIsDown())
    {
    }
}

void TilesetState::OnMouseWheel(wxMouseEvent& event)
{
    /*
    int i = event.GetWheelRotation() / event.GetWheelDelta();
    uint curTile = GetCurTile();

    // _curTile is unsigned, so we need to check for wraparound before actually doing the math.
    // So we just add the tile count, then subtract it afterwards, if needed
    curTile += GetTileset()->Count();
    curTile += i;

    // If we've gone over, wrap-around.
    while (curTile >= GetTileset()->Count()) 
        curTile -= GetTileset()->Count();

    SetCurTile(curTile);
    */
}

void TilesetState::SetTile(int x, int y)
{
    GetMapView()->ScreenToTile(x, y);

    if (x == _oldX && y == _oldY)   return;
    if (x < 0 || y < 0)             return;
    if ((uint)x >= GetCurLayer()->Width() ||
        (uint)y >= GetCurLayer()->Height())
        return;

    _oldX = x; _oldY = y;

    //if (GetCurLayer()->tiles(x, y) == GetCurTile()) return; // don't flood the undo buffer with commands that don't actually do anything


    Command* cmd = new SetBrushCommand(x, y, GetCurLayerIndex(), GetCurBrush());
#if defined(USE_GROUP_JUJU)
    // Create a new group if we need to
    if (!_curGroup)
        _curGroup = new CompositeCommand();

    // naughty.  execute the command, but don't put it on the undo stack
    // when the mouse button is released, we add the list of commands all in one go.
    cmd->Do(GetExecutor());
    _curGroup->Append(cmd);
#else
    HandleCommand(cmd);
#endif
}
Matrix<uint>& TilesetState::GetCurBrush() const
{
    return GetExecutor()->GetCurrentBrush();
}
void TilesetState::SetCurBrush(Matrix<uint>& brush)
{
    Log::Write("Set current brush");
    GetExecutor()->SetCurrentBrush(brush);
}

