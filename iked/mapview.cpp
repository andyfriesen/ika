#if 0
#include <list>
#include <wx/wx.h>
#include <wx/laywin.h>
#include <wx/sashwin.h>
#include <wx/checklst.h>
#include <wx/filename.h>

#include "mapview.h"
#include "main.h"
#include "graph.h"
#include "tileset.h"
#include "spriteset.h"
#include "common/log.h"
#include "layervisibilitycontrol.h"
#include "layerdlg.h"
#include "entityeditor.h"
#include "zoneeditor.h"
#include "tilesetview.h"
#include "vsp.h"
#include "clipboard.h"

#include <gl\glu.h>

/*

    blegh, didn't want this to get complicated.

    wxMDIChildFrame
        |----------------------|
        |                      |
    MapSash            wxSashLayoutWindow
        |                      |
    MapFrame           wxCheckListBox
    (Map rendering)     (Layer visibility)

*/

namespace
{
    enum
    {
        id_filler = 100,

        // Command IDs
        id_zoomin,
        id_zoomout,
        id_zoomnormal,
        id_zoomin2x,
        id_zoomin4x,
        id_zoomout2x,
        id_zoomout4x,

        id_filesave,
        id_filesaveas,
        id_fileclose,

        id_goprevtile,
        id_gonexttile,
        id_mapentities,
        id_mapzones,
        id_tileset,
        id_script,

        id_layermode,
        id_tilemode,
        id_entitymode,
        id_zonemode,
        id_waypointmode,

        id_movelayerup,
        id_movelayerdown,
        id_deletelayer,
        id_newlayer,
    };

    // wxSashLayoutWindow tweak that passes scroll, command, and key events to its parent.
    class MapSash : public wxSashLayoutWindow
    {
    public:
        MapSash(wxWindow* parent, int id)
            : wxSashLayoutWindow(parent, id)
        {}

        void ScrollRel(wxScrollWinEvent& event, int amount)
        {
            int max = GetScrollRange(event.GetOrientation());
            int thumbsize = GetScrollThumb(event.GetOrientation());

            amount+= GetScrollPos(event.GetOrientation());
            if (amount < 0) amount = 0;
            if (amount > max - thumbsize)
                amount = max - thumbsize;

            ((MapView*)GetParent())->OnScroll(wxScrollWinEvent(-1, amount, event.GetOrientation()));
        }

        void ScrollTo(wxScrollWinEvent& event, int pos)
        {
            int max = GetScrollRange(event.GetOrientation());
            int thumbsize = GetScrollThumb(event.GetOrientation());

            if (pos < 0) pos = 0;
            if (pos > max - thumbsize)
                pos = max - thumbsize;

            ((MapView*)GetParent())->OnScroll(wxScrollWinEvent(-1, pos, event.GetOrientation()));
        }

        void ScrollTop(wxScrollWinEvent& event)         {   ScrollTo(event, 0);      }
        void ScrollBottom(wxScrollWinEvent& event)      {   ScrollTo(event, GetScrollRange(event.GetOrientation()));     }
        
        void ScrollLineUp(wxScrollWinEvent& event)      {   ScrollRel(event, -1);    }
        void ScrollLineDown(wxScrollWinEvent& event)    {   ScrollRel(event, +1);    }

        void ScrollPageUp(wxScrollWinEvent& event)      {   ScrollRel(event, -GetScrollThumb(event.GetOrientation()));   }
        void ScrollPageDown(wxScrollWinEvent& event)    {   ScrollRel(event, +GetScrollThumb(event.GetOrientation()));   }
    
        void OnScroll(wxScrollWinEvent& event)
        {
            ((MapView*)GetParent())->OnScroll(event);
        }

        void OnMouseEvent(wxMouseEvent& event)
        {
            wxPostEvent(GetParent(), event);
        }

        void OnCommand(wxCommandEvent& event)
        {
            GetParent()->ProcessEvent(event);
        }

        void OnKeyEvent(wxKeyEvent& event)
        {
            GetParent()->ProcessEvent(event);
        }

        DECLARE_EVENT_TABLE()
    };

    BEGIN_EVENT_TABLE(MapSash, wxSashLayoutWindow)
        EVT_MOUSE_EVENTS(MapSash::OnMouseEvent)

        EVT_SCROLLWIN_TOP(MapSash::ScrollTop)
        EVT_SCROLLWIN_BOTTOM(MapSash::ScrollBottom)
        EVT_SCROLLWIN_LINEUP(MapSash::ScrollLineUp)
        EVT_SCROLLWIN_LINEDOWN(MapSash::ScrollLineDown)
        EVT_SCROLLWIN_PAGEUP(MapSash::ScrollPageUp)
        EVT_SCROLLWIN_PAGEDOWN(MapSash::ScrollPageDown)
        EVT_SCROLLWIN_THUMBTRACK(MapSash::OnScroll)
        EVT_SCROLLWIN_THUMBRELEASE(MapSash::OnScroll)

        EVT_KEY_DOWN(MapSash::OnKeyEvent)

        EVT_COMMAND_RANGE(id_filler, id_filler + 100, wxEVT_COMMAND_BUTTON_CLICKED, MapSash::OnCommand)
    END_EVENT_TABLE()

    class MapFrame : public CGraphFrame
    {
        DECLARE_EVENT_TABLE();
        
        MapView* pMapview;
    public:
        MapFrame(wxWindow* parent, MapView* mapview)
            : CGraphFrame(parent)
            , pMapview(mapview)
        {}

        void OnPaint(wxPaintEvent& event)
        {
            wxPaintDC blah(this);
            pMapview->Paint();
        }
        void OnKeyEvent(wxKeyEvent& event)
        {
            pMapview->ProcessEvent(event);
        }
    };

    BEGIN_EVENT_TABLE(MapFrame, CGraphFrame)
        EVT_PAINT(MapFrame::OnPaint)
        EVT_KEY_DOWN(MapFrame::OnKeyEvent)
    END_EVENT_TABLE()

    /// Simple little tool palette class that I can reuse many times.
    class ToolPalette : public wxPanel
    {
        MapView* _mapView;
    public:
        struct ToolButton
        {
            const char* label;
            int id;
            const char* tooltip;
        };

        /**
         * A pointer and count is used here so that I can conveniently use C style initializer lists, instead
         * of constructing a vector. (gay)
         */
        ToolPalette(wxWindow* parent, MapView* mapview, const ToolButton* buttons, int count)
            : wxPanel(parent)
            , _mapView(mapview)
        {
            wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);

            for (int i = 0; i < count; i++)
            {
                wxButton* b = new wxButton(this, buttons[i].id, buttons[i].label, wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
                if (buttons[i].tooltip)
                    b->SetToolTip(buttons[i].tooltip);
                sizer->Add(b, 0, wxALL | wxEXPAND);
            }

            SetSizer(sizer);
            sizer->Fit(this);
        }

        void OnCommand(wxCommandEvent& event)
        {
            // Relay all command events to the map view.
            _mapView->ProcessEvent(event);
        }

        DECLARE_EVENT_TABLE()
    };

    BEGIN_EVENT_TABLE(ToolPalette, wxPanel)
        EVT_COMMAND_RANGE(id_filler, id_filler + 100, wxEVT_COMMAND_BUTTON_CLICKED, ToolPalette::OnCommand)
    END_EVENT_TABLE()
}

// Editor states
namespace MapEditState
{
    Map::Layer* IEditState::CurLayer() const { return This->_curLayer; }
    CMainWnd* IEditState::Parent() const { return This->pParent; }
    CTileSet* IEditState::TileSet() const { return This->_tileSet; }
    Point IEditState::CameraPos() const
    {
        return Point(This->xwin, This->ywin);
    }

    Map* IEditState::Map() const
    {
        return This->_map;
    }

    std::map<std::string, CSpriteSet*>& IEditState::SpriteSets() const
    {
        return This->_sprites;
    }
    void IEditState::Render() { This->Render(); }
    
    // Base class for edit modes that involve dragging a rectangle around.
    class RectangleDragState : public IEditState
    {
    protected:
        virtual Rect GetRect() const = 0;
        virtual void MoveRect(const Rect& r) = 0;

        int dragx, dragy;   // the point at which dragging began
        Rect initial;       // the position of the rect before we started dragging it around
        Rect current;       // current dragged position

    public:
        RectangleDragState(MapView* t)
            : IEditState(t)
            , dragx(0)
            , dragy(0)
        {}

        virtual void OnMouseDown(wxMouseEvent& e)
        {
            dragx = e.GetPosition().x;
            dragy = e.GetPosition().y;

            current = initial = GetRect();
        }

        virtual void OnMouseUp(wxMouseEvent& e)
        {
            MoveRect(current);
        }

        virtual void OnMouseMove(wxMouseEvent& e)
        {
            if (!e.LeftIsDown())
                return;

            int deltax = e.GetPosition().x - dragx;
            int deltay = e.GetPosition().y - dragy;

            current.left   = initial.left + deltax;
            current.top    = initial.top + deltay;
            current.right  = current.left + initial.Width();
            current.bottom = current.top + initial.Height();
        }

        virtual void OnMouseWheel(wxMouseEvent& e){}

        // override this too, if you want to do more than just draw a box.
        virtual void OnRender()
        {
            This->RenderSelectionRect(current, RGBA(255, 255, 255));
        }
    };

    class TileSetState : public IEditState
    {
        int oldx, oldy;

        void LayerEdit(wxMouseEvent& e)
        {
            if (!CurLayer())   // no layer to edit
                return;

            int x = e.GetPosition().x;
            int y = e.GetPosition().y;
            This->ScreenToTile(x, y, CurLayer());

            if (x == oldx && y == oldy)
                return;

            UpdateStatBar(e, x, y);

            oldx = x;
            oldy = y;

            /*if (e.ControlDown())
            {
                if (e.LeftDown())
                    This->_editState = new CopyState(This, x, y);
                else if (e.RightDown())
                    This->_editState = new PasteState(This, x, y);
            }
            else*/
            if (e.ShiftDown())
            {
                // Grab the current tile from the map
                TileSet()->SetCurTile(CurLayer()->tiles(x, y));

                // Find the tileset window, if it is open
                CTileSetView* tsv = (CTileSetView*)Parent()->FindWindow(TileSet());
                // Tell it to refresh if it is.
                if (tsv)
                    tsv->Render();
            }
            else
            {
                CurLayer()->tiles(x, y) = TileSet()->CurTile();
                Render();
            }

        }

        void UpdateStatBar(wxMouseEvent& e, int x, int y)
        {
            wxStatusBar* s = Parent()->GetStatusBar();
            s->SetStatusText(CurLayer()->label.c_str(), 0);
            s->SetStatusText(va("(%i, %i)", x, y), 1);
        }

    public:
        TileSetState(MapView* t) 
            : IEditState(t)
            , oldx(0)
            , oldy(0)
        {}

        virtual void OnMouseDown(wxMouseEvent& e)
        {
            oldx = oldy = -1; // fool LayerEdit() into thinking that the cursor has in fact moved.
            LayerEdit(e);
        }

        virtual void OnMouseUp(wxMouseEvent& e){}

        virtual void OnMouseMove(wxMouseEvent& e)
        {
            if (e.LeftIsDown() || e.RightIsDown())
                LayerEdit(e);
        }

        virtual void OnMouseWheel(wxMouseEvent& event)
        {
            const int delta = event.GetWheelRotation() > 0 ? 1 : -1;

            if (event.ControlDown())
            {
                // zoom!
                if (delta > 0)  This->Zoom(1);
                else            This->Zoom(-1);
            }
            else
            {
                if (delta > 0)  This->GoPrevTile(event);
                else            This->GoNextTile(event);
            }
        }
    };

    class LayerMoveState : public RectangleDragState
    {
        bool dragging;

    protected:
        virtual Rect GetRect() const
        {
            if (dragging) 
                return Rect(
                    CurLayer()->x, 
                    CurLayer()->y, 
                    CurLayer()->x + CurLayer()->Width()  * TileSet()->Width(), 
                    CurLayer()->y + CurLayer()->Height() * TileSet()->Height());
            else
                return Rect();
        }

        virtual void MoveRect(const Rect& r)
        {
            if (dragging)
            {
                // for now, ignore width and height
                CurLayer()->x = r.left;
                CurLayer()->y = r.top;
            }
        }

    public:
        LayerMoveState(MapView* t)
            : RectangleDragState(t)
            , dragging(false)
        {}

        virtual void OnMouseDown(wxMouseEvent& e)
        {
            Map::Layer* curLayer = 0;
            dragging = false; // shouldn't be necessary, but I'm paranoid.

            for (uint i = 0; i < Map()->NumLayers(); i++)
            {
                Map::Layer& lay = Map()->GetLayer(i);
                if (e.m_x >= lay.x && e.m_x <= lay.x + lay.Width()  * TileSet()->Width() &&
                    e.m_y >= lay.y && e.m_y <= lay.y + lay.Height() * TileSet()->Height())
                    curLayer = &lay;
            }

            if (curLayer)
            {
                dragging = true;
                This->SetCurrentLayer(curLayer);
            }

            RectangleDragState::OnMouseDown(e);
        }

        virtual void OnMouseMove(wxMouseEvent& e)
        {
            if (dragging)
            {
                RectangleDragState::OnMouseMove(e);

                MoveRect(current);
                Render();
            }
        }

        virtual void OnMouseUp(wxMouseEvent& e)
        {
            if (dragging)
            {
                RectangleDragState::OnMouseUp(e);   // super::OnMouseUp(e);
                Render();
                dragging = false;
            }
        }
        virtual void OnRender()
        {
            // One problem with this is that it pays no attention to depth.
            // I've decided to leave it as it is, simply because this allows
            // the user to see layers that would otherwise be obscured.
            Rect r;
            for (uint i = 0; i < Map()->NumLayers(); i++)
            {
                Map::Layer& lay = Map()->GetLayer(i);

                r.left = lay.x;       r.top = lay.y;
                r.right  = r.left + lay.Width() * TileSet()->Width();
                r.bottom = r.top + lay.Height() * TileSet()->Height();

                This->RenderSelectionRect(r, 
                    (&lay == CurLayer()) ?
                        RGBA(255, 255, 255)
                    :
                        RGBA(128, 128, 128));
           }
        }
        virtual void OnDoubleClick(wxMouseEvent& e)
        {
            Map::Layer* curLayer = 0;
            for (uint i = 0; i < Map()->NumLayers(); i++)
            {
                Map::Layer& lay = Map()->GetLayer(i);
                if (e.m_x >= lay.x && e.m_x <= lay.x + lay.Width()  * TileSet()->Width() &&
                    e.m_y >= lay.y && e.m_y <= lay.y + lay.Height() * TileSet()->Height())
                    curLayer = &lay;
            }

            if (curLayer)
            {
                LayerDlg dlg(This, curLayer);
                int result = dlg.ShowModal();

                if (result == wxID_OK)
                {
                    This->UpdateLayerList();
                    Render();
                }
            }
        }
    };


    class EntityState : public RectangleDragState
    {
        bool dragging;
        Map::Layer::Entity* curEnt;
        EntityEditor* entEditor;
        int width, height; // convenience.  Holds the width/height of the entity's hotspot.

        Rect EntRect(Map::Layer::Entity* e)
        {
            assert(e);
            int w = 16, h = 16;

            if (Map()->entities.count(e->bluePrint))
            {
                Map::Entity& ent = Map()->entities[e->bluePrint];
                CSpriteSet* sprite = SpriteSets()[ent.spriteName];

                if (sprite)
                {
                    w = sprite->Width();
                    h = sprite->Height();
                }
            }

            return Rect(e->x, e->y, e->x + w, e->y + h);
        }

        Map::Layer::Entity* FindEnt(int x, int y)
        {
            Map::Layer& lay = *CurLayer();

            for (uint i = 0; i < lay.entities.size(); i++)
            {
                Map::Layer::Entity& e = lay.entities[i];
                Rect r = EntRect(&e);

                if (x >= r.left && x <= r.right && 
                    y >= r.top  && y <= r.bottom)
                    return &e;
            }

            return 0;
        }

    protected:
        virtual Rect GetRect() const
        {
            if (curEnt)
                return Rect(curEnt->x, curEnt->y, curEnt->x + width, curEnt->y + height);
            else
                return Rect();
        }
        
        virtual void MoveRect(const Rect& r)
        {
            if (curEnt)
            {
                curEnt->x = r.left;
                curEnt->y = r.top;
            }
        }

    public:
        EntityState(MapView* t)
            : RectangleDragState(t)
            , dragging(false)
            , curEnt(0)
            , entEditor(new EntityEditor(t, Map()))
        {}

        ~EntityState()
        {
            delete entEditor;
        }

        virtual void OnMouseMove(wxMouseEvent& e)
        {
            if (dragging && e.LeftIsDown())
            {
                //int x = e.m_x - CurLayer()->x;
                //int y = e.m_y - CurLayer()->y;
                RectangleDragState::OnMouseMove(e);
                MoveRect(current);//Rect(x, y, x + width, y + height));
                Render();
            }
        }

        virtual void OnMouseDown(wxMouseEvent& e)
        {
            int x = e.m_x + CameraPos().x - CurLayer()->x;
            int y = e.m_y + CameraPos().y - CurLayer()->y;
            curEnt = FindEnt(x, y);

            if (!curEnt)
            {
                int result = wxMessageBox("Create a new entity here?", "Blah!", wxYES_NO, This);
                if (result == wxYES)
                {
                    Map::Layer::Entity ent;
                    Map::Entity bluePrint;

                    ent.x = x;
                    ent.y = y;

                    // get an unused blueprint name.
                    for (int i = 0; ent.bluePrint.empty() || Map()->entities.count(ent.bluePrint) != 0; i++)
                        ent.bluePrint = va("bluePrint%i", i);

                    // get an unused entity name.
                    for (int i = 0;; i++)
                    {
loopBack:
                        std::string s = va("ent%i", i);
                        for (uint j = 0; j < CurLayer()->entities.size(); j++)
                            if (CurLayer()->entities[j].label == s)
                            {
                                i++;
                                goto loopBack; // name already taken.  Pick another
                            }
                        // else
                        {
                            ent.label = s;
                            break;
                        }
                    }

                    bluePrint.label = ent.bluePrint;

                    Map()->entities[ent.bluePrint] = bluePrint;
                    CurLayer()->entities.push_back(ent);
                }
            }
            else
            {
                Rect r = EntRect(curEnt);
                width = r.Width();
                height = r.Height();
                dragging = true;
                RectangleDragState::OnMouseDown(e);
            }
            
            Render();
        }

        virtual void OnMouseUp(wxMouseEvent& e)
        {
            RectangleDragState::OnMouseUp(e);
            dragging = false;
            Render();
        }

        virtual void OnRender()
        {
            for (uint i = 0; i < CurLayer()->entities.size(); i++)
            {
                int w = 16, h = 16;

                const Map::Layer::Entity& e = CurLayer()->entities[i];
                
                if (Map()->entities.count(e.bluePrint))
                {
                    const Map::Entity& ent = Map()->entities[e.bluePrint];
                    const CSpriteSet* sprite = SpriteSets()[ent.spriteName];

                    if (sprite)
                    {
                        w = sprite->Width();
                        h = sprite->Height();
                    }
                }

                This->RenderSelectionRect(Rect(e.x, e.y, e.x + w, e.y + h),
                    (curEnt == &CurLayer()->entities[i]) ?
                        RGBA(255, 255, 255)
                    :
                        RGBA(128, 128, 128));
            }
        }
        virtual void OnDoubleClick(wxMouseEvent& e)
        {
            entEditor->Show(curEnt);
        }
        virtual void OnKeyPress(wxKeyEvent& e)
        {
            if (!curEnt)
                return;

            if (e.KeyCode() == WXK_DELETE)
            {
                if (wxMessageBox("You cannot undo this.\nReally delete this entity?", "Confirm", wxYES_NO, This))
                {
                    Map()->entities.erase(curEnt->bluePrint);
                    CurLayer()->entities.erase(curEnt);
                    curEnt = 0;
                    Render();
                }
            }
        }
    };
    //--
/*
    void CopyState::OnMouseDown(wxMouseEvent& e)
    {
        // O_o.  This shouldn't happen
    }
    
    void CopyState::OnMouseUp(wxMouseEvent& e)
    {
        _selection.right++;
        _selection.bottom++;
        _selection.Normalize();

        MapClip mc;
        This->_map->Copy(mc, _selection, CurLayer());
        Clipboard::Set(mc);
        MapView* mv = This;
        This->_editState = new TileSetState(This);

        mv->Render();
    }

    void CopyState::OnMouseMove(wxMouseEvent& event)
    {
        if (!event.ControlDown())
        {
            MapView* mv = This;
            This->_editState = new TileSetState(This);
            mv->Render();
            return;
        }

        int x = event.GetPosition().x;
        int y = event.GetPosition().y;
        This->ScreenToTile(x, y, CurLayer());

        _selection.right = x;
        _selection.bottom = y;

        if (event.RightDown() || !event.ControlDown())         // Right mouse button, or control key released - cancel
            This->_editState = new TileSetState(This);

        This->Render();
    }

    void CopyState::OnMouseWheel(wxMouseEvent& e){}

    void CopyState::OnRender()
    {
        This->RenderSelectionRect(_selection, RGBA(255, 255, 255));
    }

    PasteState::PasteState(MapView* t, int x, int y)
        : IEditState(t)
    {
        const MapClip& mc = Clipboard::GetMap();
        _selection = Rect(x, y, x + mc.Width(), y + mc.Height());
    }

    void PasteState::OnMouseDown(wxMouseEvent& event)
    {
        if (event.LeftDown())
            This->_editState = new TileSetState(This);
    }

    void PasteState::OnMouseUp(wxMouseEvent& event)
    {
        if (event.RightUp())
        {
            This->_map->Paste(Clipboard::GetMap(), _selection.left, _selection.top, CurLayer());
            MapView* mv = This;
            This->_editState = new TileSetState(This);
            mv->Render();
            return;
        }
    }

    void PasteState::OnMouseMove(wxMouseEvent& event)
    {
        if (!event.ControlDown())
        {
            MapView* mv = This;
            This->_editState = new TileSetState(This);
            mv->Render();
            return;
        }

        int x = event.GetPosition().x;
        int y = event.GetPosition().y;
        This->ScreenToTile(x, y, CurLayer());

        const MapClip& mc = Clipboard::GetMap();
        _selection.left = x;
        _selection.top = y;
        _selection.right = x + mc.Width() - 1;
        _selection.bottom = y + mc.Height() - 1;

        This->Render();
    }

    void PasteState::OnMouseWheel(wxMouseEvent& e)
    {
    }

    void PasteState::OnRender()
    {
        This->RenderSelectionRect(_selection, RGBA(255, 0, 0));
    }*/
}

//-------------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(MapView, wxMDIChildFrame)
    EVT_MENU(id_zoomin, MapView::OnZoomIn)
    EVT_MENU(id_zoomout, MapView::OnZoomOut)
    EVT_MENU(id_zoomnormal, MapView::OnZoomNormal)

    EVT_MENU(id_zoomin2x, MapView::OnZoomIn2x)
    EVT_MENU(id_zoomin4x, MapView::OnZoomIn4x)
    EVT_MENU(id_zoomout2x, MapView::OnZoomOut2x)
    EVT_MENU(id_zoomout4x, MapView::OnZoomOut4x)

    EVT_MENU(id_goprevtile, MapView::GoPrevTile)
    EVT_MENU(id_gonexttile, MapView::GoNextTile)
    EVT_MENU(id_mapentities, MapView::OnShowEntityEditor)
    EVT_MENU(id_mapzones, MapView::OnShowZoneEditor)
    EVT_MENU(id_newlayer, MapView::OnNewLayer)
    EVT_MENU(id_tileset, MapView::OnShowVSP)
    EVT_MENU(id_script, MapView::OnShowScript)

    EVT_MENU(id_filesave, MapView::OnSave)
    EVT_MENU(id_filesaveas, MapView::OnSaveAs)
    EVT_MENU(id_fileclose, MapView::OnClose)

    EVT_ERASE_BACKGROUND(MapView::OnErase)
    EVT_SIZE(MapView::OnSize)
    EVT_SCROLLWIN(MapView::OnScroll)
    EVT_CLOSE(MapView::OnClose)

    EVT_KEY_DOWN(MapView::OnKeyDown)

    EVT_BUTTON(id_layermode, MapView::OnLayerMode)
    EVT_BUTTON(id_tilemode,  MapView::OnTileMode)
    EVT_BUTTON(id_entitymode, MapView::OnEntityMode)
    EVT_BUTTON(id_zonemode,   MapView::OnZoneMode)

    EVT_BUTTON(id_movelayerup, MapView::OnMoveLayerUp)
    EVT_BUTTON(id_movelayerdown, MapView::OnMoveLayerDown)
    EVT_BUTTON(id_newlayer, MapView::OnNewLayer)
    EVT_BUTTON(id_deletelayer, MapView::OnDeleteLayer)
    
    EVT_MOUSE_EVENTS(MapView::HandleMouse)
END_EVENT_TABLE()

MapView::MapView(CMainWnd* parent, int width, int height, const std::string& tilesetname)
    : IDocView(parent, "")
    , pParentwnd(parent)
    , _curZone(0)
{
    _map = new Map;
    _map->width = width;
    _map->height = height;
    _map->tileSetName = tilesetname;
    Init();
}

MapView::MapView(CMainWnd* parent, const std::string& name)
    : IDocView(parent, name)
    , pParentwnd(parent)
    , _curZone(0)
{
    _map = pParentwnd->map.Load(name);    
    Init();
}

void MapView::InitAccelerators()
{
    std::vector<wxAcceleratorEntry> accel = pParent->CreateBasicAcceleratorTable();

    int p = accel.size();
    accel.resize(accel.size()+6);

    accel[p++].Set(wxACCEL_CTRL, (int)'S', id_filesave);
    accel[p++].Set(0, (int)'[', id_gonexttile);
    accel[p++].Set(0, (int)']', id_goprevtile);
    accel[p++].Set(0, (int)'+', id_zoomin);
    accel[p++].Set(0, (int)'-', id_zoomout);
    accel[p++].Set(0, (int)'=', id_zoomnormal);

    wxAcceleratorTable table(p, &* accel.begin());
    SetAcceleratorTable(table);
}

void MapView::InitMenu()
{
    wxMenuBar* menubar = pParent->CreateBasicMenu();

    wxMenu* filemenu = menubar->Remove(0);
    filemenu->InsertSeparator(2);
    filemenu->Insert(3, new wxMenuItem(filemenu, id_filesave, "&Save", "Save the map to disk."));
    filemenu->Insert(4, new wxMenuItem(filemenu, id_filesaveas, "Save &As", "Save the map under a new filename."));
    filemenu->Insert(5, new wxMenuItem(filemenu, id_fileclose, "&Close", "Close the map view."));
    menubar->Append(filemenu, "&File");

    //--

    wxMenu* viewmenu = new wxMenu;
    
    viewmenu->Append(id_zoomnormal, "Zoom %&100", "");
    viewmenu->AppendSeparator();
    
    viewmenu->Append(id_zoomin, "Zoom &In\t+", "");
    viewmenu->Append(id_zoomout, "Zoom &Out\t-", "");

    viewmenu->Append(id_zoomin2x, "Zoom In 2x", "");
    viewmenu->Append(id_zoomout2x, "Zoom Out 2x", "");

    viewmenu->Append(id_zoomin4x, "Zoom In 4x", "");
    viewmenu->Append(id_zoomout4x, "Zoom Out 4x", "");

    menubar->Append(viewmenu, "&View");

    //--

    wxMenu* mapmenu = new wxMenu;
    mapmenu->Append(id_goprevtile, "&Previous Tile\t[", "");
    mapmenu->Append(id_gonexttile, "&Next Tile\t]", "");
    mapmenu->AppendSeparator();

    mapmenu->Append(id_tileset, "&Tileset");
    mapmenu->Append(id_script, "&Script");
    mapmenu->Append(id_newlayer, "New &Layer");
    mapmenu->Append(id_mapentities, "&Entities...");
    mapmenu->Append(id_mapzones, "&Zones...");

    menubar->Append(mapmenu, "&Map");

    //--

    SetMenuBar(menubar);
}

void MapView::Init()
{
    if (!_map)
    {
        wxMessageBox("Unable to load map", "Error", wxOK | wxCENTER, this);
        Close();
        return;
    }

    int w, h;
    GetClientSize(&w, &h);

    SetIcon(wxIcon("mapicon", wxBITMAP_TYPE_ICO_RESOURCE));

    // Left side -- layer properties
    pLeftbar = new wxSashLayoutWindow(this, -1);
    pLeftbar->SetAlignment(wxLAYOUT_LEFT);
    pLeftbar->SetOrientation(wxLAYOUT_VERTICAL);
    pLeftbar->SetDefaultSize(wxSize(100, 100));
    pLeftbar->SetSashVisible(wxSASH_RIGHT, true);

    wxPanel* panel = new wxPanel(pLeftbar);

    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    const ToolPalette::ToolButton modeButtons[] =
    {
        {   "L",    id_layermode,   "Edit layers."      },
        {   "T",    id_tilemode,    "Paint tiles."      },
        {   "E",    id_entitymode,  "Edit entities."    },
        {   "Z",    id_zonemode,    "Edit zones."       },
        {   "W",    id_waypointmode,"Edit waypoints."   },
    };
    const int numModeButtons = sizeof modeButtons / sizeof modeButtons[0];

    const ToolPalette::ToolButton layerButtons[] =
    {
        { "^", id_movelayerup,      "Move the layer up in the render order."    },
        { "v", id_movelayerdown,    "Move the layer down in the render order."  },
        { "x", id_deletelayer,      "Remove the layer from the map."            },
        { "*", id_newlayer,         "Create a new layer."                       }
    };
    const int numLayerButtons = sizeof layerButtons / sizeof layerButtons[0];

    // Tool buttons that sit above the layer list
    mainSizer->Add(new ToolPalette(panel, this, modeButtons, numModeButtons), 0, wxALL);
    mainSizer->Add(new ToolPalette(panel, this, layerButtons, numLayerButtons), 0, wxALL);
    // The layer list itself.
    _layerList = new CLayerVisibilityControl(panel, -1, this);
    mainSizer->Add(_layerList, 1, wxALL | wxEXPAND);

    panel->SetSizer(mainSizer);

    // Right side -- Map view
    pRightbar = new MapSash(this, -1);
    pRightbar->SetAlignment(wxLAYOUT_RIGHT);

    _editState = new MapEditState::TileSetState(this);
    //_editState = new MapEditState::LayerMoveState(this);

    _graph = new MapFrame(pRightbar, this);

    wxString mapPath(wxFileName(name.c_str()).GetPath());
    
    // If it's not an absolute path, then it is assumed to be relative to the map file.
    wxFileName tileSetName(_map->tileSetName.c_str());
    if (!tileSetName.IsAbsolute() && !mapPath.empty())
        tileSetName.InsertDir(0, mapPath);

    _tileSet = pParentwnd->vsp.Load(tileSetName.GetFullPath().c_str()); // load the VSP
    if (!_tileSet)
    {
        wxMessageBox(va("Unable to load tileset %s", tileSetName.GetFullPath().c_str()), "Error", wxOK | wxCENTER, this);
        Close();
        return;
    }

    // Load CHRs

    for (Map::EntityMap::iterator iter = _map->entities.begin(); iter != _map->entities.end(); iter++)
    {
        std::string s(iter->second.spriteName);

        // If it's not an absolute path, then it is assumed to be relative to the map file.
        wxFileName spriteName(s.c_str());
        if (!spriteName.IsAbsolute() && !mapPath.empty())
            spriteName.InsertDir(0, mapPath);

        try
        {
            _sprites[s] = pParentwnd->spriteset.Load(spriteName.GetFullPath().Strip().c_str());
        }
        catch (std::runtime_error err)
        {
            Log::Write(err.what());
            _sprites[s] = 0;
        }
    }

    // --

    pRightbar->SetScrollbar(wxVERTICAL,   0, w, _map->height * _tileSet->Height());
    pRightbar->SetScrollbar(wxHORIZONTAL, 0, h, _map->width  * _tileSet->Width());
    xwin = ywin = 0;

    UpdateLayerList();
    InitAccelerators();
    InitMenu();

    _curLayer = _map->NumLayers() > 0 ? &_map->GetLayer(0) : 0;
    //_entityEditor = new EntityEditor(this, _map);
    //_zoneEditor   = new ZoneEditor(this, _map);

    Show();
    SetFocus();
}

void MapView::Paint()
{
    if (!_tileSet || !_map)
        return; // Frick'n gay.  wx likes to call this between deallocating my stuff, and actually destroying the frame. ;P

    _graph->SetCurrent();
    _graph->Clear();    

    Render();
}

void MapView::OnSize(wxSizeEvent& event)
{
    wxLayoutAlgorithm layout;
    layout.LayoutWindow(this, pRightbar);

    if (_map && _tileSet) // gaaaay.
        UpdateScrollbars();
}

void MapView::OnScroll(wxScrollWinEvent& event)
{
    if (!_map)
        return;

    switch (event.GetOrientation())
    {
    case wxHORIZONTAL:  xwin = event.GetPosition();   break;
    case wxVERTICAL:    ywin = event.GetPosition();   break;
    }

    UpdateScrollbars();

    Render();
}

void MapView::OnClose()
{
    Log::Write("MapView::Close");
    Log::Write("Releasing Map.");
    pParentwnd->map.Release(_map);
    Log::Write("Releasing tileset.");
    pParentwnd->vsp.Release(_tileSet);

    Log::Write("Releasing spritesets.");
    for (SpriteMap::iterator i = _sprites.begin(); i!= _sprites.end(); i++)
        pParentwnd->spriteset.Release(i->second);
    
    _sprites.clear();
    _map = 0;
    _tileSet = 0;

    Log::Write("Destroying map window....");

    Destroy();
}

void MapView::OnKeyDown(wxKeyEvent& event)
{
    _editState->OnKeyPress(event);
}

void MapView::OnSave(wxCommandEvent& event)
{
    if (name.length())
    {
        // More path junk.  Absolute paths need to be converted to relative paths, or the game won't run in any other directory.
        wxFileName tileSetName(_map->tileSetName.c_str());
        if (tileSetName.IsAbsolute())
            tileSetName.MakeRelativeTo(wxFileName(name.c_str(), "", "").GetPath());

        _map->tileSetName = tileSetName.GetFullPath().c_str();

        _map->Save(name);
    }
    else
        OnSaveAs(event);
}

void MapView::OnSaveAs(wxCommandEvent& event)
{
    wxFileDialog dlg(
        this,
        "Open File",
        "",
        "",
        "ika maps (*.map)|*.map|"
        "All files (*.*)|*.*",
        wxSAVE | wxOVERWRITE_PROMPT
        );

    int result = dlg.ShowModal();
    if (result == wxID_CANCEL)
        return;

    std::string oldname(name);

    name = dlg.GetPath().c_str();
    SetTitle(name.c_str());

    OnSave(event);

    if (!Path::equals(name, oldname))
    {
        /* This bit is important.
         * iked refcounts everything based on filename.
         * we need to do this to make sure that the integrity is maintained.
         */
        _map->Load(oldname.c_str());        // If anybody else is looking at this map, they'll get the right one. (unlikely in the case of maps)
        if (!pParent->map.Release(_map))
            delete _map;                    // We let go of it. (one way or the other)

        _map = pParent->map.Load(name);     // Then we get the one we just saved.
    }
}


const void* MapView::GetResource() const
{
    return _map;
}

void MapView::SetCurrentLayer(Map::Layer* lay)
{
    assert(lay);
    _layerList->SelectLayer(lay);
    _curLayer = lay;
}

void MapView::SetCurrentLayer(const std::string& layerName)
{
    for (uint i = 0; i < _map->NumLayers(); i++)
    {
        if (_map->GetLayer(i).label == layerName)
        {
            SetCurrentLayer(&_map->GetLayer(i));
            return;
        }
    }

    // No such layer exists.  Do nothing.
}

void MapView::GoPrevTile(wxEvent&)
{
    int t = _tileSet->CurTile() + 1;                                    // increment
    _tileSet->SetCurTile(t % _tileSet->Count());                        // modulo for wraparound

    CTileSetView* tsv = (CTileSetView*)pParent->FindWindow(_tileSet);   // And redraw the tileset window, if it is open.
    if (tsv)    tsv->Render();
}

void MapView::GoNextTile(wxEvent&)
{
    int t = _tileSet->CurTile() + _tileSet->Count() - 1;                // Decrement.  Add the total number of tiles so...
    _tileSet->SetCurTile(t % _tileSet->Count());                        // modulo can once again perform cheapass wraparound
    
    CTileSetView* tsv = (CTileSetView*)pParent->FindWindow(_tileSet);   // And redraw the tileset window, if it is open.
    if (tsv)    tsv->Render();
}

void MapView::Zoom(int nZoomscale)
{
    int zoom = _graph->Zoom() - nZoomscale;

    if (zoom < 1) zoom = 1;
    if (zoom > 255) zoom = 255;

    _graph->Zoom(zoom);

    UpdateScrollbars();
    Render();
}

void MapView::OnZoomIn(wxCommandEvent&)    { Zoom( 1); }
void MapView::OnZoomOut(wxCommandEvent&)   { Zoom(-1); }
void MapView::OnZoomIn2x(wxCommandEvent&)  { Zoom( 2); }
void MapView::OnZoomOut2x(wxCommandEvent&) { Zoom(-2); }
void MapView::OnZoomIn4x(wxCommandEvent&)  { Zoom( 4); }
void MapView::OnZoomOut4x(wxCommandEvent&) { Zoom(-4); }
void MapView::OnZoomNormal(wxCommandEvent&){ Zoom(_graph->Zoom() - 16); }  // >:D

void MapView::OnShowEntityEditor(wxCommandEvent&)
{
    //_entityEditor->Show(true);
}

void MapView::OnShowZoneEditor(wxCommandEvent&)
{
    //_zoneEditor->Show(true);
}

void MapView::OnShowVSP(wxCommandEvent&)
{
    pParent->Open(_tileSet->GetVSP().Name());
}

void MapView::OnShowScript(wxCommandEvent&)
{
    pParent->Open(Path::replaceExtension(name, "py"));
}

void MapView::OnLayerMode(wxCommandEvent&)
{
    _editState = new MapEditState::LayerMoveState(this);
    Render();
}

void MapView::OnTileMode(wxCommandEvent&)
{
    _editState = new MapEditState::TileSetState(this);
    Render();
}

void MapView::OnEntityMode(wxCommandEvent&)
{
    _editState = new MapEditState::EntityState(this);
    Render();
}

void MapView::OnZoneMode(wxCommandEvent&)
{
}

void MapView::OnMoveLayerUp(wxCommandEvent&)
{
    uint index = _map->NumLayers(); // always too big
    for (uint i = 0; i < _map->NumLayers(); i++)
    {
        if (&_map->GetLayer(i) == _curLayer)
        {
            index = i;
            break;
        }
    }
    assert(index < _map->NumLayers()); // too big == layer not found.  Very bad.

    if (index > 0)
    {
        _map->SwapLayers(index, index - 1);
        UpdateLayerList();
        Render();
    }
}

void MapView::OnMoveLayerDown(wxCommandEvent&)
{
    uint index = _map->NumLayers(); // always too big
    for (uint i = 0; i < _map->NumLayers(); i++)
    {
        if (&_map->GetLayer(i) == _curLayer)
        {
            index = i;
            break;
        }
    }
    assert(index < _map->NumLayers()); // too big == layer not found.  Very bad.

    if (index < _map->NumLayers() - 1)
    {
        _map->SwapLayers(index, index + 1);
        UpdateLayerList();
        Render();
    }
}

void MapView::OnDeleteLayer(wxCommandEvent&)
{
    if (!_curLayer) // no layer selected
        return;

    int result = wxMessageBox("Are you sure you want to delete this layer?\nThis cannot be undone!", "You sure?", wxYES_NO | wxCENTRE, this);
    if (result == wxYES)
    {
        _map->DestroyLayer(*_curLayer);
        UpdateLayerList();
        Render();
    }
}

void MapView::OnNewLayer(wxCommandEvent&)
{
    Map::Layer& lay = _map->AddLayer("New Layer", 100, 100);
    _curLayer = &lay; // is this the correct behaviour?
    UpdateLayerList();
    UpdateScrollbars();
    Render();
}

//------------------------------------------------------------

void MapView::MapToTile(int& x, int& y)
{
    x /= _tileSet->Width();
    y /= _tileSet->Height();
}

void MapView::ScreenToTile(int& x, int& y)
{
    ScreenToTile(x, y, _curLayer);
}

void MapView::ScreenToTile(int& x, int& y, Map::Layer* layer)
{
    assert(layer);

    x += xwin - layer->x;
    y += ywin - layer->y;

    x = x * layer->parallax.mulx / layer->parallax.divx;
    y = y * layer->parallax.muly / layer->parallax.divy;

    MapToTile(x, y);
}

void MapView::HandleMouse(wxMouseEvent& event)
{
    if (event.ButtonDClick())                       _editState->OnDoubleClick(event);
    else if (event.GetWheelRotation())              _editState->OnMouseWheel(event);
    else if (event.LeftDown() || event.RightDown()) _editState->OnMouseDown(event);
    else if (event.LeftUp()   || event.RightUp())   _editState->OnMouseUp(event);
    else if (event.Moving())                        _editState->OnMouseMove(event);
}

void MapView::HandleMouseWheel(wxMouseEvent& event)
{
    const int delta = event.GetWheelRotation();

}

void MapView::OnLayerChange(Map::Layer* lay)
{
    _curLayer = lay;
    //if (nLayertoggle[lay] == hidden)
    {
        //nLayertoggle[lay] = visible;
        Render();
        //_layerList->CheckLayer(lay);
    }
}

void MapView::OnLayerToggleVisibility(Map::Layer* lay, int newstate)
{
    return;
    //nLayertoggle[lay] = newstate;
    Render();
}

void MapView::UpdateScrollbars()
{
    int w = _graph->LogicalWidth();
    int h = _graph->LogicalHeight();

    // temp
    _map->width = _map->height = 0;
    for (uint i = 0; i < _map->NumLayers(); i++)
    {
        _map->width = max(_map->width, _map->GetLayer(i).Width());
        _map->height = max(_map->height, _map->GetLayer(i).Height());
    }
    // end temp

    int maxx = _map->width * _tileSet->Width();
    int maxy = _map->height * _tileSet->Height();

    // clip the viewport
    if (xwin + w > maxx) xwin = maxx - w;
    if (ywin + h > maxy) ywin = maxy - h;
    if (xwin < 0) xwin = 0;
    if (ywin < 0) ywin = 0;

    pRightbar->SetScrollbar(wxHORIZONTAL, xwin, w, _map->width * _tileSet->Width()  );
    pRightbar->SetScrollbar(wxVERTICAL, ywin, h,   _map->height* _tileSet->Height() );
}

void MapView::UpdateLayerList()
{
    // Fill up the layer info bar

    _layerList->Clear();

    for (uint i = 0; i < _map->NumLayers(); i++)
    {
        _layerList->AppendLayer(&_map->GetLayer(i));
    }

    _layerList->SelectLayer(_curLayer);
}

// ------------------------------ Rendering -------------------------

void MapView::Render()
{
    _graph->SetCurrent();
    _graph->Clear();

    for (uint i = 0; i < _map->NumLayers(); i++)
    {
        Map::Layer& lay = _map->GetLayer(i);

        // TODO: hide layers
        
        // figure out where the layer would be rendered
        Map::Layer::ParallaxInfo& pi = lay.parallax;
        int xw = (xwin - lay.x) * pi.mulx / pi.divx;
        int yw = (ywin - lay.y) * pi.muly / pi.divy;

        RenderLayer(&lay, xw, yw);
        RenderEntities(&lay, xw, yw);
        // TODO: handle wrapping
        // TODO: handle rendering entities
        // TODO: handle rendering obstructions
        // TODO: handle rendering zones.
    }

    _editState->OnRender();
    _graph->ShowPage();
}

namespace
{
    struct DrawEntry
    {
        int x, y;
        CSpriteSet* sprite;
        DrawEntry(int _x, int _y, CSpriteSet* s)
            : x(_x)
            , y(_y)
            , sprite(s)
        {}
    };
}

void MapView::RenderEntities(Map::Layer* lay, int xoffset, int yoffset)
{

    typedef std::list<DrawEntry> DrawList;
    DrawList entsToDraw;
    
    int x2, y2;
    _graph->GetClientSize(&x2, &y2);
    x2 += xoffset;
    y2 += yoffset;

    // 1) figure out which entities to draw
    for (uint i = 0; i < lay->entities.size(); i++)
    {
        Map::Layer::Entity& e = lay->entities[i];

        if (e.x < xoffset || e.y < yoffset)   continue;
        if (e.x > x2   || e.y > y2)     continue;

        CSpriteSet* ss = 0;
        if (_map->entities.count(e.bluePrint))
        {
            Map::Entity& bluePrint = _map->entities[e.bluePrint];
            ss = _sprites[bluePrint.spriteName];
        }

        entsToDraw.push_back(DrawEntry(e.x, e.y, ss));
    }

    // 2) y sort
    // bleh.

    // 3) render!
    for (DrawList::iterator j = entsToDraw.begin(); j!= entsToDraw.end(); j++)
    {
        if (j->sprite)                                                                             // spritesets that couldn't be loaded are null pointers
            _graph->Blit(j->sprite->GetImage(0), j->x - xwin, j->y - ywin, true);
        else
            _graph->RectFill(j->x, j->y, _tileSet->Width(), _tileSet->Height(), RGBA(0, 0, 0, 128)); // no spriteset found, draw a gray square
    }
}

// AGH I HATE THIS TODO: figure out a nice way to consolidate all this into a single function. (templates seem like a good idea)

/*void MapView::RenderInfoLayer(int lay)
{
    int nWidth = _graph->LogicalWidth();
    int nHeight = _graph->LogicalHeight();

    int xw = xwin;
    int yw = ywin;

    int tx = _tileSet->Width();
    int ty = _tileSet->Height();

    int nFirstx = xw / tx;
    int nFirsty = yw / ty;
    
    int nLenx = nWidth / tx + 2;
    int nLeny = nHeight / ty + 2;

    if (nFirstx + nLenx > _map->Width())  nLenx = _map->Width()-nFirstx;
    if (nFirsty + nLeny > _map->Height()) nLeny = _map->Height()-nFirsty;

    int nAdjx = xw % tx;
    int nAdjy = yw % ty;

    if (lay == lay_obstruction)
    {
        for (int y = 0; y < nLeny; y++)
        {
            for (int x = 0; x < nLenx; x++)
            {
                if (_map->IsObs(x + nFirstx, y + nFirsty))
                    _graph->RectFill(
                        x * tx - nAdjx, y * ty - nAdjy,
                        tx, ty,
                        RGBA(0, 0, 0, 128));
            }
        }
    }
    else
    {
        for (int y = 0; y < nLeny; y++)
        {
            for (int x = 0; x < nLenx; x++)
            {
                int z = _map->GetZone(x + nFirstx, y + nFirsty);

                RGBA c(0, 0, (z * 4) & 255, 128);
            
                if (lay == 0 || z!= 0)
                    _graph->RectFill(
                        x * tx - nAdjx, y * ty - nAdjy,
                        tx, ty,
                        c);
            }
        }
    }

    glColor4f(1, 1, 1, 1);
}
*/
void MapView::RenderLayer(Map::Layer* lay, int xoffset, int yoffset)
{
    int nWidth = _graph->LogicalWidth();
    int nHeight = _graph->LogicalHeight();

    int xw = xoffset;
    int yw = yoffset;

    int tx = _tileSet->Width();
    int ty = _tileSet->Height();

    int nFirstx = xw / tx;
    int nFirsty = yw / ty;
    
    int nLenx = nWidth / tx + 2;
    int nLeny = nHeight / ty + 2;

    int nAdjx = (xw % tx);
    int nAdjy = (yw % ty);

    if (nFirstx + nLenx > lay->Width())  nLenx = lay->Width()  - nFirstx;
    if (nFirsty + nLeny > lay->Height()) nLeny = lay->Height() - nFirsty;

    if (nFirstx < 0)  
    {
        nLenx -= -nFirstx;
        nAdjx += nFirstx * tx;
        nFirstx = 0;
    }
    if (nFirsty < 0)
    {
        nLeny -= -nFirsty;
        nAdjy += nFirsty * ty;
        nFirsty = 0;
    }

    for (int y = 0; y < nLeny; y++)
    {
        for (int x = 0; x < nLenx; x++)
        {
            int t = lay->tiles(x+ nFirstx, y + nFirsty);

            _graph->Blit(
                _tileSet->GetImage(t),
                x * tx - nAdjx, y * ty - nAdjy,
                true);
        }
    }
}

void MapView::RenderSelectionRect(Rect r, RGBA colour)
{
    /* 
     * All 4 sides of the rect are inclusive, so we just swap so that the lesser coords are top and left,
     * then multiply by the tile size, and add the tile size to the right and bottom edges, so the rect
     * draws properly.  Then we draw.
     */

    /*if (r.left > r.right)
    {
        swap(r.left, r.right);
    }
    if (r.top > r.bottom)
    {
        swap(r.bottom, r.top);
    }

    r.top *= _tileSet->Height();
    r.left *= _tileSet->Width();
    r.bottom = (r.bottom + 1) * _tileSet->Height();
    r.right = (r.right + 1) * _tileSet->Width();*/

    _graph->Rect(r.left - xwin, r.top - ywin, r.Width(), r.Height(), colour);
}
#endif