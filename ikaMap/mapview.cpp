
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
    EVT_MIDDLE_DOWN(MapView::OnMouseDown)
    EVT_LEFT_UP(MapView::OnMouseUp)
    EVT_MOTION(MapView::OnMouseMove)
    EVT_MOUSEWHEEL(MapView::OnMouseWheel)


    
END_EVENT_TABLE()

MapView::MapView(MainWindow* mw, wxWindow* parent)
    : wxPanel(parent)
    , _mainWnd(mw)
    , _tileSetState(mw)
    , _editState(&_tileSetState)

    , _xwin(0)
    , _ywin(0)
    , _curLayer(0)
{
    _video = new VideoFrame(this);
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
        }
    }

    _editState->OnRender();
}

void MapView::ShowPage()
{
    _video->ShowPage();
}

void MapView::RenderLayer(Map::Layer* lay, int xoffset, int yoffset)
{
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
    }
}

void MapView::RenderEntities(Map::Layer* lay, int xoffset, int yoffset)
{
    for (std::vector<Map::Layer::Entity>::iterator iter = lay->entities.begin(); iter != lay->entities.end(); iter++)
    {
        SpriteSet* ss = 0;
        // default position/size if the sprite cannot be found
        int hotx = 0;
        int hoty = 0;
        int hotw = 16;
        int hoth = 16;
        int width = 16;
        int height = 16;

        if (_mainWnd->GetMap()->entities.count(iter->bluePrint))
        {
            Map::Entity* bluePrint = &_mainWnd->GetMap()->entities[iter->bluePrint];
            ss = _mainWnd->GetSprite(bluePrint->spriteName);
        }

        if (ss != 0)
        {
            hotx = ss->GetCHR()->HotX();
            hoty = ss->GetCHR()->HotY();
            hotw = ss->GetCHR()->HotW();
            hoth = ss->GetCHR()->HotH();
            width = ss->Width();
            height = ss->Height();
        }

        // Too far down or to the right.  Don't need to draw
        if (iter->x + hotx - width  - _xwin > _video->LogicalWidth() ||
            iter->y + hoty - height - _ywin > _video->LogicalHeight())
            continue;

        // too far to the left or top.  Don't need to draw.
        if (iter->x + hotx - _xwin < 0 ||
            iter->y + hoty - _ywin < 0)
            continue;

        // Finally draw the goddamned thing.
        if (ss)
            _video->Blit(ss->GetImage(0), iter->x - hotx - _xwin, iter->y - hoty - _ywin);
        else
            _video->RectFill(iter->x - _xwin, iter->y - _ywin, 16, 16, RGBA(255, 255, 255, 128));
    }
}

void MapView::UpdateScrollBars()
{
    const Map* map = _mainWnd->GetMap();

    SetScrollbar(wxHORIZONTAL, _xwin, _video->LogicalWidth(),  map->width);
    SetScrollbar(wxVERTICAL,   _ywin, _video->LogicalHeight(), map->height);
}

void MapView::ScreenToTile(int& x, int& y)
{
    ScreenToTile(x, y, _curLayer);
}

void MapView::ScreenToTile(int& x, int& y, uint layer)
{
    wxASSERT(_mainWnd->GetMap() != 0);
    wxASSERT(_mainWnd->GetTileSet() != 0);
    wxASSERT(layer < _mainWnd->GetMap()->NumLayers());

    Map::Layer* lay = &_mainWnd->GetMap()->GetLayer(layer);

    x = (x + _xwin - lay->x) / _mainWnd->GetTileSet()->Width();
    y = (y + _ywin - lay->y) / _mainWnd->GetTileSet()->Height();
}

void MapView::TileToScreen(int& x, int& y)
{
    TileToScreen(x, y, _curLayer);
}

void MapView::TileToScreen(int& x, int& y, uint layer)
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

    _curLayer = i;
}