
#include "mapview.h"

#include "executor.h"
#include "map.h"
#include "spriteset.h"
#include "chr.h"
#include "tileset.h"
#include "command.h"

#include "log.h"

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
    EVT_KEY_DOWN(MapView::OnKeyPress)
END_EVENT_TABLE()

MapView::MapView(Executor* executor, wxWindow* parent)
    : wxPanel(parent)
    , _executor(executor)

    , _tileSetState(executor)
    , _copyPasteState(executor)
    , _obstructionState(executor)
    , _entityState(executor)
    , _zoneEditState(executor)
    , _scriptState(executor)

    , _editState(&_tileSetState)

    , _xwin(0)
    , _ywin(0)
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
    const Map* map = _executor->GetMap();

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
    if (_executor->GetMap()->NumLayers() == 0)
        return;

    if (!_executor->IsLayerVisible(_executor->GetCurrentLayer()))
    {
        ::wxMessageBox("You shouldn't be able to have an active layer that is not\n"
                       "visible.  If you are reading this now, you are experiencing a bug.", "Internal error.",
                       wxOK, this);
        return;
    }

    if (event.MiddleDown())
    {
        _scrollX = event.GetX();
        _scrollY = event.GetY();
    }

    else if (_executor->GetMap()->NumLayers())
        _editState->OnMouseDown(event);
}

void MapView::OnMouseUp(wxMouseEvent& event)
{
    if (_executor->GetMap()->NumLayers() > 0)
        _editState->OnMouseUp(event);
}

void MapView::OnMouseMove(wxMouseEvent& event)
{
    if (_executor->GetCurrentLayer() < _executor->GetMap()->NumLayers())
    {
        int x = event.GetX();
        int y = event.GetY();
        ScreenToTile(x, y);
        _executor->SetStatusBar(va("(%i, %i)", x, y), 0);
    }

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

        _xwin = min<int>(_xwin, _executor->GetMap()->width - _video->LogicalWidth());    _xwin = max(0, _xwin);
        _ywin = min<int>(_ywin, _executor->GetMap()->height - _video->LogicalHeight());  _ywin = max(0, _ywin);

        UpdateScrollBars();
        Render();
        ShowPage();
    }
    else if (_executor->GetMap()->NumLayers())
        _editState->OnMouseMove(event);
}

void MapView::OnMouseWheel(wxMouseEvent& event)
{
    if (_executor->GetMap()->NumLayers())
        _editState->OnMouseWheel(event);
}

void MapView::OnKeyPress(wxKeyEvent& event)
{
    _editState->OnKeyPress(event);
}

void MapView::OnMapChange(const MapEvent& event)
{
    Render();
    ShowPage();
}

void MapView::OnCurLayerChange(uint index)
{
    Render();
    ShowPage();
}

void MapView::Render()
{
    Map* map = _executor->GetMap();
    int curLayer = _executor->GetCurrentLayer();

    _video->SetCurrent();
    _video->Clear();

    for (uint i = 0; i < map->NumLayers(); i++)
    {
        if (_executor->IsLayerVisible(i))
        {
            Map::Layer* lay = map->GetLayer(i);

            // TODO: Parallax, and possibly wrapping.
            RenderLayer(lay, _xwin - lay->x, _ywin - lay->y);
            RenderEntities(lay, _xwin - lay->x, _ywin - lay->y);

            if (i == curLayer) _editState->OnRenderCurrentLayer();
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
    TileSet* ts = _executor->GetTileSet();
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
    TileSet* ts = _executor->GetTileSet();

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

    if ((uint)(firstX + lenX) > lay->Width())  lenX = lay->Width()  - firstX;
    if ((uint)(firstY + lenY) > lay->Height()) lenY = lay->Height() - firstY;

    for (int y = 0; y < lenY; y++)
    {
        for (int x = 0; x < lenX; x++)
        {
            if (lay->obstructions(x + firstX, y + firstY))
                _video->RectFill(x * tileX - adjustX, y * tileY - adjustY, tileX, tileY, RGBA(128, 128, 128, 128));
        }
    }
}

void MapView::UpdateScrollBars()
{
    const Map* map = _executor->GetMap();

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
    x += _xwin;
    y += _ywin;
}

void MapView::ScreenToLayer(int& x, int& y) const
{
    ScreenToLayer(x, y, _executor->GetCurrentLayer());
}

void MapView::ScreenToLayer(int& x, int& y, uint layer) const
{
    wxASSERT(layer < _executor->GetMap()->NumLayers());

    ScreenToMap(x, y);

    Map::Layer* lay = _executor->GetMap()->GetLayer(layer);

    x -= lay->x;
    y -= lay->y;
}

void MapView::ScreenToTile(int& x, int& y) const
{
    ScreenToTile(x, y, _executor->GetCurrentLayer());
}

void MapView::ScreenToTile(int& x, int& y, uint layer) const
{
    wxASSERT(_executor->GetMap() != 0);
    wxASSERT(_executor->GetTileSet() != 0);
    wxASSERT(layer < _executor->GetMap()->NumLayers());

    ScreenToLayer(x, y, layer);

    x /= _executor->GetTileSet()->Width();
    y /= _executor->GetTileSet()->Height();
}

void MapView::TileToScreen(int& x, int& y) const
{
    TileToScreen(x, y, _executor->GetCurrentLayer());
}

void MapView::TileToScreen(int& x, int& y, uint layer) const
{
    wxASSERT(_executor->GetMap());
    wxASSERT(_executor->GetTileSet());
    wxASSERT(layer < _executor->GetMap()->NumLayers());

    Map::Layer* lay = _executor->GetMap()->GetLayer(layer);

    x = (x * _executor->GetTileSet()->Width()) + lay->x - _xwin;
    y = (y * _executor->GetTileSet()->Height()) + lay->y - _ywin;
}

void MapView::MapToTile(int& x, int& y) const
{
    MapToTile(x, y, _executor->GetCurrentLayer());
}

void MapView::MapToTile(int& x, int& y, uint layer) const
{
    wxASSERT(_executor->GetMap());
    wxASSERT(_executor->GetTileSet());
    wxASSERT(layer < _executor->GetMap()->NumLayers());

    Map::Layer* lay = _executor->GetMap()->GetLayer(layer);

    x = (x + lay->x) * lay->parallax.mulx / lay->parallax.divx;
    y = (y + lay->y) * lay->parallax.muly / lay->parallax.divy;
    x /= _executor->GetTileSet()->Width();
    y /= _executor->GetTileSet()->Height();
}

void MapView::TileToMap(int& x, int& y) const
{
    TileToMap(x, y, _executor->GetCurrentLayer());
}

void MapView::TileToMap(int& x, int& y, uint layer) const
{
    wxASSERT(_executor->GetMap());
    wxASSERT(_executor->GetTileSet());
    wxASSERT(layer < _executor->GetMap()->NumLayers());

    Map::Layer* lay = _executor->GetMap()->GetLayer(layer);

    x = (x * lay->parallax.divx / lay->parallax.mulx) - lay->x;
    y = (y * lay->parallax.divy / lay->parallax.muly) - lay->y;
}

uint MapView::EntityAt(int x, int y, uint layer)
{
    wxASSERT(layer < _executor->GetMap()->NumLayers());

    std::vector<Map::Entity>& ents = _executor->GetMap()->GetLayer(layer)->entities;

    for (uint i = 0; i < ents.size(); i++)
    {
        Map::Entity& ent = ents[i];

        /*if (ent.x > x)  continue;
        if (ent.y > y)  continue;*/

        int hotx = 0;
        int hoty = 0;
        int width = 16;
        int height = 16;

        SpriteSet* ss = GetEntitySpriteSet(&ents[i]);
        if (ss)
        {
            hotx =   ss->GetCHR()->HotX();
            hoty =   ss->GetCHR()->HotY();
            width =  ss->Width();
            height = ss->Height();
        }

        if (ent.x - hotx > x)           continue;
        if (ent.y - hoty > y)           continue;

        if (ent.x - hotx + width  < x)  continue;
        if (ent.y - hoty + height < y)  continue;

        return i;
    }

    return -1;
}

uint MapView::ZoneAt(int x, int y, uint layer)
{
    wxASSERT(layer < _executor->GetMap()->NumLayers());

    std::vector<Map::Layer::Zone>& zones = _executor->GetMap()->GetLayer(layer)->zones;

    for (uint i = 0; i < zones.size(); i++)
    {
        Map::Layer::Zone& zone = zones[i];

        if (zone.position.left   <= x &&
            zone.position.top    <= y &&
            zone.position.right  >= x &&
            zone.position.bottom >= y)
            return i;
    }
    return -1;
}

SpriteSet* MapView::GetEntitySpriteSet(Map::Entity* ent) const
{
    return _executor->GetSpriteSet(ent->spriteName);
}

VideoFrame* MapView::GetVideo() const
{
    return _video;
}

void MapView::SetEditState(EditState* newState)
{
    _editState->OnEndState();
    _editState = newState;
    _editState->OnBeginState();

    Render();
    ShowPage();
}

void MapView::Cock()
{
    SetEditState(&_tileSetState);
}

void MapView::SetCopyPasteState()
{
    SetEditState(&_copyPasteState);
}

void MapView::SetObstructionState()
{
    SetEditState(&_obstructionState);
}
void MapView::SetEntityState()
{
    SetEditState(&_entityState);
}

void MapView::SetZoneState()
{
    SetEditState(&_zoneEditState);
}
void MapView::SetScriptTool(Script* script)
{
    _scriptState.SetScript(script);

    SetEditState(&_scriptState);
}