
#include "mapview.h"

#include "mainwindow.h"
#include "map.h"
#include "chr.h"
#include "tileset.h"
#include "command.h"

BEGIN_EVENT_TABLE(MapView, wxPanel)
    EVT_SIZE(MapView::OnSize)
    EVT_SCROLLWIN(MapView::OnScroll)
    EVT_ERASE_BACKGROUND(MapView::OnEraseBackground)
    EVT_PAINT(MapView::OnPaint)

    EVT_LEFT_DOWN(MapView::OnMouseDown)
    EVT_RIGHT_DOWN(MapView::OnMouseDown)
    EVT_MIDDLE_DOWN(MapView::OnMouseDown)
    EVT_LEFT_UP(MapView::OnMouseUp)
    EVT_RIGHT_UP(MapView::OnMouseUp)
    EVT_LEFT_DCLICK(MapView::OnMouseDown)
    EVT_MOTION(MapView::OnMouseMove)
    EVT_MOUSEWHEEL(MapView::OnMouseWheel)
END_EVENT_TABLE()

MapView::MapView(MainWindow* mw, wxWindow* parent)
    : wxPanel(parent)
    , _mainWnd(mw)
    , _tileSetState(mw)
    , _copyPasteState(mw)
    , _obstructionState(mw)
    , _entityState(mw)
    , _editState(&_tileSetState)

    , _xwin(0)
    , _ywin(0)
    , _curLayer(0)
{
    _video = new VideoFrame(this);
    _video->SetClearColour(RGBA(128, 128, 128));
}

MapView::~MapView()
{}

void MapView::OnPaint(wxPaintEvent& event)
{
    wxPaintDC dc(this);

    Render();
    ShowPage();
}

void MapView::OnSize(wxSizeEvent& event)
{
    _video->SetSize(GetClientSize());
    UpdateScrollBars();
}

void MapView::OnScroll(wxScrollWinEvent& event)
{
    struct Local
    {
        static void HandleEvent(int& val, int min, int max, int page, int pos, wxEventType et)
        {
            // Can't switch/case this. :\
            // Luckily, my regex-fu is developed enough that I could convert this
            // from a switch/case to what you see in about 2 seconds. :D
                 if (et == wxEVT_SCROLLWIN_TOP)          { val = min;       }
            else if (et == wxEVT_SCROLLWIN_BOTTOM)       { val = max;       }
            else if (et == wxEVT_SCROLLWIN_LINEUP)       { val--;           }
            else if (et == wxEVT_SCROLLWIN_LINEDOWN)     { val++;           }
            else if (et == wxEVT_SCROLLWIN_PAGEUP)       { val -= page;     }
            else if (et == wxEVT_SCROLLWIN_PAGEDOWN)     { val += page;     }
            else if (et == wxEVT_SCROLLWIN_THUMBTRACK)   { val = pos;       }
            else if (et == wxEVT_SCROLLWIN_THUMBRELEASE) { val = pos;       }
            
            val = clamp(val, min, max);
        }
    };
    const Map* map = _mainWnd->GetMap();

    if (event.GetOrientation() == wxHORIZONTAL)
        Local::HandleEvent(_xwin, 0, map->width,  _video->LogicalWidth(),  event.GetPosition(), event.GetEventType());
    else
        Local::HandleEvent(_ywin, 0, map->height, _video->LogicalHeight(), event.GetPosition(), event.GetEventType());
    UpdateScrollBars();
    Render();
    ShowPage();
}

void MapView::OnMouseDown(wxMouseEvent& event)
{
    if (event.MiddleDown())
    {
        _scrollX = event.GetX();
        _scrollY = event.GetY();
    }

    else if (_mainWnd->GetMap()->NumLayers())
        _editState->OnMouseDown(event);
}

void MapView::OnMouseUp(wxMouseEvent& event)
{
    if (_mainWnd->GetMap()->NumLayers())
        _editState->OnMouseUp(event);
}

void MapView::OnMouseMove(wxMouseEvent& event)
{
    if (event.MiddleIsDown())
    {
        // Middlemouse + drag scrolls the map around.
        // Hold down the Ctrl key to scroll really really fast.
        int dx = _scrollX - event.GetX();
        int dy = _scrollY - event.GetY();
        _scrollX = event.GetX();
        _scrollY = event.GetY();

        _xwin += dx * (event.ControlDown() ? 4 : 1);
        _ywin += dy * (event.ControlDown() ? 4 : 1);

        _xwin = min<int>(_xwin, _mainWnd->GetMap()->width - _video->LogicalWidth());    _xwin = max(0, _xwin);
        _ywin = min<int>(_ywin, _mainWnd->GetMap()->height - _video->LogicalHeight());  _ywin = max(0, _ywin);

        UpdateScrollBars();
        Render();
        ShowPage();
    }
    else if (_mainWnd->GetMap()->NumLayers())
        _editState->OnMouseMove(event);
}

void MapView::OnMouseWheel(wxMouseEvent& event)
{
    if (_mainWnd->GetMap()->NumLayers())
        _editState->OnMouseWheel(event);
}

void MapView::Render()
{
    Map* map = _mainWnd->GetMap();

    _video->SetCurrent();
    _video->Clear();

    for (uint i = 0; i < map->NumLayers(); i++)
    {
        if (_mainWnd->IsLayerVisible(i))
        {
            Map::Layer& lay = map->GetLayer(i);

            // TODO: Parallax, and possibly wrapping.
            RenderLayer(&lay, _xwin - lay.x, _ywin - lay.y);
            RenderEntities(&lay, _xwin - lay.x, _ywin - lay.y);

            if (i == _curLayer) _editState->OnRenderCurrentLayer();
        }
    }

    _editState->OnRender();
}

void MapView::ShowPage()
{
    _video->ShowPage();
}

void MapView::RenderLayer(const Matrix<uint>& tiles, int xoffset, int yoffset)
{
    TileSet* ts = _mainWnd->GetTileSet();
    if (!ts->Count())
        return;

    int width  = _video->LogicalWidth();
    int height = _video->LogicalHeight();

    int tileX = ts->Width();
    int tileY = ts->Height();

    int firstX = xoffset / tileX;
    int firstY = yoffset / tileY;
    
    int lenX = width  / tileX + 2;
    int lenY = height / tileY + 2;

    int adjustX = xoffset % tileX;
    int adjustY = yoffset % tileY;

    if (firstX < 0)  
    {
        lenX -= -firstX;
        adjustX += firstX * tileX;
        firstX = 0;
    }
    if (firstY < 0)
    {
        lenY -= -firstY;
        adjustY += firstY * tileY;
        firstY = 0;
    }

    if ((uint)(firstX + lenX) > tiles.Width())  lenX = tiles.Width()  - firstX;
    if ((uint)(firstY + lenY) > tiles.Height()) lenY = tiles.Height() - firstY;

    for (int y = 0; y < lenY; y++)
    {
        for (int x = 0; x < lenX; x++)
        {
            int t = tiles(x + firstX, y + firstY);

            _video->Blit(
                ts->GetImage(t),
                x * tileX - adjustX, y * tileY - adjustY,
                true);
        }
    }
}

void MapView::RenderLayer(Map::Layer* lay, int xoffset, int yoffset)
{
    // TODO: wrapping
    xoffset = xoffset * lay->parallax.mulx / lay->parallax.divx;
    yoffset = yoffset * lay->parallax.muly / lay->parallax.divy;

    RenderLayer(lay->tiles, xoffset, yoffset);

    /*TileSet* ts = _mainWnd->GetTileSet();
    if (!ts->Count())
        return;

    int width  = _video->LogicalWidth();
    int height = _video->LogicalHeight();

    int tileX = ts->Width();
    int tileY = ts->Height();

    int firstX = xoffset / tileX;
    int firstY = yoffset / tileY;
    
    int lenX = width  / tileX + 2;
    int lenY = height / tileY + 2;

    int adjustX = xoffset % tileX;
    int adjustY = yoffset % tileY;

    if (firstX + lenX > lay->Width())  lenX = lay->Width()  - firstX;
    if (firstY + lenY > lay->Height()) lenY = lay->Height() - firstY;

    if (firstX < 0)  
    {
        lenX -= -firstX;
        adjustX += firstX * tileX;
        firstX = 0;
    }
    if (firstY < 0)
    {
        lenY -= -firstY;
        adjustY += firstY * tileY;
        firstY = 0;
    }

    for (int y = 0; y < lenY; y++)
    {
        for (int x = 0; x < lenX; x++)
        {
            int t = lay->tiles(x + firstX, y + firstY);

            _video->Blit(
                ts->GetImage(t),
                x * tileX - adjustX, y * tileY - adjustY,
                true);
        }
    }*/
}

void MapView::RenderEntities(Map::Layer* lay, int xoffset, int yoffset)
{
    for (std::vector<Map::Entity>::iterator iter = lay->entities.begin(); iter != lay->entities.end(); iter++)
    {
        // default position/size if the sprite cannot be found
        int hotx = 0;
        int hoty = 0;
        int hotw = 16;
        int hoth = 16;
        int width = 16;
        int height = 16;

        SpriteSet* ss = GetEntitySpriteSet(&*iter);
        if (ss != 0)
        {
            hotx = ss->GetCHR()->HotX();
            hoty = ss->GetCHR()->HotY();
            hotw = ss->GetCHR()->HotW();
            hoth = ss->GetCHR()->HotH();
            width = ss->Width();
            height = ss->Height();
        }

        int x = iter->x - hotx - _xwin + lay->x;
        int y = iter->y - hoty - _ywin + lay->y;

        // Too far down or to the right.  Don't need to draw
        if (x > _video->LogicalWidth() ||
            y > _video->LogicalHeight())
            continue;

        // too far to the left or top.  Don't need to draw.
        if (x < -width ||
            y < -height)
            continue;

        // Finally draw the goddamned thing.
        if (ss)
            _video->Blit(ss->GetImage(0), x , y);
        else
            _video->RectFill(x, y, width, height, RGBA(255, 255, 255, 128));
    }
}

void MapView::RenderObstructions(Map::Layer* lay, int xoffset, int yoffset)
{
    // Draw a gray square over obstructed things.
    TileSet* ts = _mainWnd->GetTileSet();

    int width  = _video->LogicalWidth();
    int height = _video->LogicalHeight();

    int tileX = ts->Width();
    int tileY = ts->Height();

    int firstX = xoffset / tileX;
    int firstY = yoffset / tileY;
    
    int lenX = width  / tileX + 2;
    int lenY = height / tileY + 3;

    int adjustX = (xoffset % tileX);
    int adjustY = (yoffset % tileY);

    if (firstX < 0)  
    {
        lenX -= -firstX;
        adjustX += firstX * tileX;
        firstX = 0;
    }
    if (firstY < 0)
    {
        lenY -= -firstY;
        adjustY += firstY * tileY;
        firstY = 0;
    }

    if ((uint)(firstX + lenX > lay->Width()))  lenX = lay->Width()  - firstX;
    if ((uint)(firstY + lenY > lay->Height())) lenY = lay->Height() - firstY;

    for (int y = 0; y < lenY; y++)
    {
        for (int x = 0; x < lenX; x++)
        {
            if (lay->obstructions(x + firstX, y + firstY))
                _video->RectFill(x * tileX - adjustX, y * tileY - adjustY, tileX, tileY, RGBA(0, 0, 0, 128));
        }
    }
}

void MapView::UpdateScrollBars()
{
    const Map* map = _mainWnd->GetMap();

    SetScrollbar(wxHORIZONTAL, _xwin, _video->LogicalWidth(),  map->width);
    SetScrollbar(wxVERTICAL,   _ywin, _video->LogicalHeight(), map->height);
}

uint MapView::GetZoom() const
{
    return _video->GetZoom();
}

void MapView::SetZoom(uint z)
{
    _video->SetZoom(z);
}

void MapView::IncZoom(int amt)
{
    _video->IncZoom(amt);
}

void MapView::ScreenToMap(int& x, int& y) const
{
    ScreenToMap(x, y, _curLayer);
}

void MapView::ScreenToMap(int& x, int& y, uint layer) const
{
    wxASSERT(layer < _mainWnd->GetMap()->NumLayers());

    Map::Layer* lay = &_mainWnd->GetMap()->GetLayer(layer);

    x = x + _xwin - lay->x;
    y = y + _ywin - lay->y;
}

void MapView::ScreenToTile(int& x, int& y) const
{
    ScreenToTile(x, y, _curLayer);
}

void MapView::ScreenToTile(int& x, int& y, uint layer) const
{
    wxASSERT(_mainWnd->GetMap() != 0);
    wxASSERT(_mainWnd->GetTileSet() != 0);
    wxASSERT(layer < _mainWnd->GetMap()->NumLayers());

    ScreenToMap(x, y, layer);

    x /= _mainWnd->GetTileSet()->Width();
    y /= _mainWnd->GetTileSet()->Height();
}

void MapView::TileToScreen(int& x, int& y) const
{
    TileToScreen(x, y, _curLayer);
}

void MapView::TileToScreen(int& x, int& y, uint layer) const
{
    wxASSERT(_mainWnd->GetMap());
    wxASSERT(_mainWnd->GetTileSet());
    wxASSERT(layer < _mainWnd->GetMap()->NumLayers());

    Map::Layer* lay = &_mainWnd->GetMap()->GetLayer(layer);

    x = (x * _mainWnd->GetTileSet()->Width()) + lay->x - _xwin;
    y = (y * _mainWnd->GetTileSet()->Height()) + lay->y - _ywin;
}

void MapView::SetCurLayer(uint i)
{
    wxASSERT(i < _mainWnd->GetMap()->NumLayers());

    uint old = _curLayer;
    _curLayer = i;

    _editState->OnSwitchLayers(old, _curLayer);

}

uint MapView::EntityAt(int x, int y, uint layer)
{
    wxASSERT(layer < _mainWnd->GetMap()->NumLayers());

    std::vector<Map::Entity>& ents = _mainWnd->GetMap()->GetLayer(layer).entities;

    for (uint i = 0; i < ents.size(); i++)
    {
        Map::Entity& ent = ents[i];

        if (ent.x > x)  continue;
        if (ent.y > y)  continue;

        SpriteSet* ss = GetEntitySpriteSet(&ents[i]);
        int hotx = ss ? ss->GetCHR()->HotX() : 0;
        int hoty = ss ? ss->GetCHR()->HotY() : 0;
        int width = ss ? ss->Width() : 16;
        int height = ss ? ss->Height() : 16;

        if (ent.x - hotx + width  < x) continue;
        if (ent.y - hoty + height < y) continue;

        return i;
    }

    return -1;
}

SpriteSet* MapView::GetEntitySpriteSet(Map::Entity* ent) const
{
    return _mainWnd->GetSprite(ent->spriteName);
}

VideoFrame* MapView::GetVideo() const
{
    return _video;
}

void MapView::Cock()
{
    _editState->OnEndState();
    _editState = &_tileSetState;
    _editState->OnBeginState();

    Render();
    ShowPage();
}

void MapView::SetCopyPasteState()
{
    _editState->OnEndState();
    _editState = &_copyPasteState;
    _editState->OnBeginState();

    Render();
    ShowPage();
}

void MapView::SetObstructionState()
{
    _editState->OnEndState();
    _editState = &_obstructionState;
    _editState->OnBeginState();

    Render();
    ShowPage();
}
void MapView::SetEntityState()
{
    _editState->OnEndState();
    _editState = &_entityState;
    _editState->OnBeginState();

    Render();
    ShowPage();
}