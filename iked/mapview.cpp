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
#include "log.h"
#include "layervisibilitycontrol.h"
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
    CMapSash            wxSashLayoutWindow
        |                      |
    CMapFrame           wxCheckListBox
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

        id_movelayerup,
        id_movelayerdown,
        id_deletelayer,
        id_newlayer,
    };

    enum
    {
        lay_entity=-10,
        lay_zone,
        lay_obstruction,
        lay_render // stub.  You can't do anything with this except move it around through the rendering order
    };

    // wxSashLayoutWindow tweak that passes scroll and command events to its parent.
    class CMapSash : public wxSashLayoutWindow
    {
    public:
        CMapSash(wxWindow* parent, int id)
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

            ((CMapView*)GetParent())->OnScroll(wxScrollWinEvent(-1, amount, event.GetOrientation()));
        }

        void ScrollTo(wxScrollWinEvent& event, int pos)
        {
            int max = GetScrollRange(event.GetOrientation());
            int thumbsize = GetScrollThumb(event.GetOrientation());

            if (pos < 0) pos = 0;
            if (pos > max - thumbsize)
                pos = max - thumbsize;

            ((CMapView*)GetParent())->OnScroll(wxScrollWinEvent(-1, pos, event.GetOrientation()));
        }

        void ScrollTop(wxScrollWinEvent& event)         {   ScrollTo(event, 0);      }
        void ScrollBottom(wxScrollWinEvent& event)      {   ScrollTo(event, GetScrollRange(event.GetOrientation()));     }
        
        void ScrollLineUp(wxScrollWinEvent& event)      {   ScrollRel(event, -1);    }
        void ScrollLineDown(wxScrollWinEvent& event)    {   ScrollRel(event, +1);    }

        void ScrollPageUp(wxScrollWinEvent& event)      {   ScrollRel(event, -GetScrollThumb(event.GetOrientation()));   }
        void ScrollPageDown(wxScrollWinEvent& event)    {   ScrollRel(event, +GetScrollThumb(event.GetOrientation()));   }
    
        void OnScroll(wxScrollWinEvent& event)
        {
            ((CMapView*)GetParent())->OnScroll(event);
        }

        void OnMouseEvent(wxMouseEvent& event)
        {
            wxPostEvent(GetParent(), event);
        }

        void OnCommand(wxCommandEvent& event)
        {
            GetParent()->ProcessEvent(event);
        }

        DECLARE_EVENT_TABLE()
    };

    BEGIN_EVENT_TABLE(CMapSash, wxSashLayoutWindow)
        EVT_MOUSE_EVENTS(CMapSash::OnMouseEvent)

        EVT_SCROLLWIN_TOP(CMapSash::ScrollTop)
        EVT_SCROLLWIN_BOTTOM(CMapSash::ScrollBottom)
        EVT_SCROLLWIN_LINEUP(CMapSash::ScrollLineUp)
        EVT_SCROLLWIN_LINEDOWN(CMapSash::ScrollLineDown)
        EVT_SCROLLWIN_PAGEUP(CMapSash::ScrollPageUp)
        EVT_SCROLLWIN_PAGEDOWN(CMapSash::ScrollPageDown)
        EVT_SCROLLWIN_THUMBTRACK(CMapSash::OnScroll)
        EVT_SCROLLWIN_THUMBRELEASE(CMapSash::OnScroll)

        EVT_COMMAND_RANGE(id_filler, id_filler + 100, wxEVT_COMMAND_BUTTON_CLICKED, CMapSash::OnCommand)
    END_EVENT_TABLE()

    class CMapFrame : public CGraphFrame
    {
        DECLARE_EVENT_TABLE();
        
        CMapView* pMapview;
    public:
        CMapFrame(wxWindow* parent, CMapView* mapview)
            : CGraphFrame(parent)
            , pMapview(mapview)
        {}

        void OnPaint(wxPaintEvent& event)
        {
            wxPaintDC blah(this);
            pMapview->Paint();
        }
    };

    BEGIN_EVENT_TABLE(CMapFrame, CGraphFrame)
        EVT_PAINT(CMapFrame::OnPaint)
    END_EVENT_TABLE()

    class LayerToolBar : public wxPanel
    {
        CMapView* _mapView;
    public:
        LayerToolBar(wxWindow* parent, CMapView* mapview)
            : wxPanel(parent)
            , _mapView(mapview)
        {
            struct {
                const char* label;
                int id;
                const char* tooltip;
            } buttons[] =
            {
                { "^", id_movelayerup, "Move the layer up in the render order." },
                { "v", id_movelayerdown, "Move the layer down in the render order." },
                { "x", id_deletelayer, "Remove the layer from the map." },
                { "*", id_newlayer, "Create a new layer." }
            };

            wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);

            for (int i = 0; i < sizeof buttons / sizeof buttons[0]; i++)
            {
                wxButton* b = new wxButton(this, buttons[i].id, buttons[i].label, wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
                b->SetToolTip(buttons[i].tooltip);
                sizer->Add(b, 0, wxALL | wxEXPAND);
            }

            SetSizer(sizer);
            sizer->Fit(this);
        }

        // ARGH
#define MAKE_STUPID_RELAY_THING(name) void name(wxCommandEvent& event) { _mapView->name(event); }

        MAKE_STUPID_RELAY_THING(OnNewLayer)
        MAKE_STUPID_RELAY_THING(OnDeleteLayer)
        MAKE_STUPID_RELAY_THING(OnMoveLayerUp)
        MAKE_STUPID_RELAY_THING(OnMoveLayerDown)

#undef MAKE_STUPID_RELAY_THING

        DECLARE_EVENT_TABLE()
    };

    BEGIN_EVENT_TABLE(LayerToolBar, wxPanel)
        EVT_BUTTON(id_newlayer, LayerToolBar::OnNewLayer)
        EVT_BUTTON(id_deletelayer, LayerToolBar::OnDeleteLayer)
        EVT_BUTTON(id_movelayerdown, LayerToolBar::OnMoveLayerDown)
        EVT_BUTTON(id_movelayerup, LayerToolBar::OnMoveLayerUp)
    END_EVENT_TABLE()
}

// Editor states
namespace MapEditState
{
    void TileSetState::LayerEdit(wxMouseEvent& e)
    {
        int x = e.GetPosition().x;
        int y = e.GetPosition().y;
        This->ScreenToTile(x, y, This->_curLayer);

        if (x == oldx && y == oldy)
            return;

        oldx = x;
        oldy = y;

        if (e.ControlDown())
        {
            if (e.LeftDown())
                This->_editState = new CopyState(This, x, y);
            else if (e.RightDown())
                This->_editState = new PasteState(This, x, y);
            return;
        }

        if (e.ShiftDown())
        {
            This->pTileset->SetCurTile(This->_map->GetTile(x, y, This->_curLayer));
            CTileSetView* tsv = (CTileSetView*)This->pParent->FindWindow(This->pTileset);
            if (tsv)
                tsv->Render();
        }
        else
        {
            This->_map->SetTile(x, y, This->_curLayer, This->pTileset->CurTile());
            This->Render();
            This->pGraph->ShowPage();
        }

        UpdateStatBar(e, x, y);
    }

    void TileSetState::UpdateStatBar(wxMouseEvent& e, int x, int y)
    {
        wxStatusBar* s = This->pParent->GetStatusBar();
        s->SetStatusText("", 0);
        s->SetStatusText(va("(%i, %i)", x, y), 1);
    }

    void TileSetState::OnMouseDown(wxMouseEvent& e)
    {
        LayerEdit(e);
    }

    void TileSetState::OnMouseUp(wxMouseEvent& e){}

    void TileSetState::OnMouseMove(wxMouseEvent& e)
    {
        if (e.LeftIsDown())
            LayerEdit(e);
    }

    void TileSetState::OnMouseWheel(wxMouseEvent& event)
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

    //--

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
        This->_map->Copy(mc, _selection, This->_curLayer);
        Clipboard::Set(mc);
        CMapView* mv = This;
        This->_editState = new TileSetState(This);

        mv->Render();
        mv->pGraph->ShowPage();
    }

    void CopyState::OnMouseMove(wxMouseEvent& event)
    {
        if (!event.ControlDown())
        {
            CMapView* mv = This;
            This->_editState = new TileSetState(This);
            mv->Render();
            mv->pGraph->ShowPage();
            return;
        }

        int x = event.GetPosition().x;
        int y = event.GetPosition().y;
        This->ScreenToTile(x, y, This->_curLayer);

        _selection.right = x;
        _selection.bottom = y;

        if (event.RightDown() || !event.ControlDown())         // Right mouse button, or control key released - cancel
            This->_editState = new TileSetState(This);

        This->Render();
        This->pGraph->ShowPage();
    }

    void CopyState::OnMouseWheel(wxMouseEvent& e){}

    void CopyState::OnRender()
    {
        This->RenderSelectionRect(_selection, RGBA(255, 255, 255));
    }

    PasteState::PasteState(CMapView* t, int x, int y)
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
            This->_map->Paste(Clipboard::GetMap(), _selection.left, _selection.top, This->_curLayer);
            CMapView* mv = This;
            This->_editState = new TileSetState(This);
            mv->Render();
            mv->pGraph->ShowPage();
            return;
        }
    }

    void PasteState::OnMouseMove(wxMouseEvent& event)
    {
        if (!event.ControlDown())
        {
            CMapView* mv = This;
            This->_editState = new TileSetState(This);
            mv->Render();
            mv->pGraph->ShowPage();
            return;
        }

        int x = event.GetPosition().x;
        int y = event.GetPosition().y;
        This->ScreenToTile(x, y, This->_curLayer);

        const MapClip& mc = Clipboard::GetMap();
        _selection.left = x;
        _selection.top = y;
        _selection.right = x + mc.Width() - 1;
        _selection.bottom = y + mc.Height() - 1;

        This->Render();
        This->pGraph->ShowPage();
    }

    void PasteState::OnMouseWheel(wxMouseEvent& e)
    {
    }

    void PasteState::OnRender()
    {
        This->RenderSelectionRect(_selection, RGBA(255, 0, 0));
    }
}

//-------------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(CMapView, wxMDIChildFrame)
    EVT_MENU(id_zoomin, CMapView::OnZoomIn)
    EVT_MENU(id_zoomout, CMapView::OnZoomOut)
    EVT_MENU(id_zoomnormal, CMapView::OnZoomNormal)

    EVT_MENU(id_zoomin2x, CMapView::OnZoomIn2x)
    EVT_MENU(id_zoomin4x, CMapView::OnZoomIn4x)
    EVT_MENU(id_zoomout2x, CMapView::OnZoomOut2x)
    EVT_MENU(id_zoomout4x, CMapView::OnZoomOut4x)

    EVT_MENU(id_goprevtile, CMapView::GoPrevTile)
    EVT_MENU(id_gonexttile, CMapView::GoNextTile)
    EVT_MENU(id_mapentities, CMapView::OnShowEntityEditor)
    EVT_MENU(id_mapzones, CMapView::OnShowZoneEditor)
    EVT_MENU(id_newlayer, CMapView::OnNewLayer)
    EVT_MENU(id_tileset, CMapView::OnShowVSP)
    EVT_MENU(id_script, CMapView::OnShowScript)

    EVT_MENU(id_filesave, CMapView::OnSave)
    EVT_MENU(id_filesaveas, CMapView::OnSaveAs)
    EVT_MENU(id_fileclose, CMapView::OnClose)

    EVT_ERASE_BACKGROUND(CMapView::OnErase)
    EVT_SIZE(CMapView::OnSize)
    EVT_SCROLLWIN(CMapView::OnScroll)
    EVT_CLOSE(CMapView::OnClose)
    
    EVT_MOUSE_EVENTS(CMapView::HandleMouse)
END_EVENT_TABLE()

CMapView::CMapView(CMainWnd* parent, int width, int height, const string& tilesetname)
    : IDocView(parent, "")
    , pParentwnd(parent)
    , _curZone(0)
{
    _map = new Map;
    _map->Resize(width, height);
    _map->SetVSPName(tilesetname);
    Init();
}

CMapView::CMapView(CMainWnd* parent, const string& name)
    : IDocView(parent, name)
    , pParentwnd(parent)
    , _curZone(0)
{
    _map = pParentwnd->map.Load(name);    
    Init();
}

void CMapView::InitAccelerators()
{
    vector<wxAcceleratorEntry> accel = pParent->CreateBasicAcceleratorTable();

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

void CMapView::InitMenu()
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

void CMapView::Init()
{
    int w, h;
    GetClientSize(&w, &h);

    // Left side -- layer properties
    pLeftbar = new wxSashLayoutWindow(this, -1);
    pLeftbar->SetAlignment(wxLAYOUT_LEFT);
    pLeftbar->SetOrientation(wxLAYOUT_VERTICAL);
    pLeftbar->SetDefaultSize(wxSize(100, 100));
    pLeftbar->SetSashVisible(wxSASH_RIGHT, true);

    wxPanel* panel = new wxPanel(pLeftbar);

    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

        mainSizer->Add(new LayerToolBar(panel, this), 0, wxALL);
        pLayerlist = new CLayerVisibilityControl(panel, -1, this);
        mainSizer->Add(pLayerlist, 1, wxALL | wxEXPAND);

    panel->SetSizer(mainSizer);

    // Right side -- Map view
    pRightbar = new CMapSash(this, -1);
    pRightbar->SetAlignment(wxLAYOUT_RIGHT);

    _editState = new MapEditState::TileSetState(this);

    pGraph = new CMapFrame(pRightbar, this);

    wxString mapPath(wxFileName(name.c_str()).GetPath());
    
    // If it's not an absolute path, then it is assumed to be relative to the map file.
    wxFileName tileSetName(_map->GetVSPName().c_str());
    if (!tileSetName.IsAbsolute() && !mapPath.empty())
        tileSetName.InsertDir(0, mapPath);

    pTileset = pParentwnd->vsp.Load(tileSetName.GetFullPath().c_str()); // load the VSP
    if (!pTileset)
    {
        wxMessageBox(va("Unable to load tileset %s", tileSetName.GetFullPath().c_str()), "Error", wxOK | wxCENTER, this);
        Close();
        return;
    }

    // Load CHRs

    for (int i = 0; i < _map->NumEnts(); i++)
    {
        // If it's not an absolute path, then it is assumed to be relative to the map file.
        wxFileName spriteName(_map->GetEntity(i).sCHRname.c_str());
        if (!spriteName.IsAbsolute() && !mapPath.empty())
            spriteName.InsertDir(0, mapPath);

        pSprite.push_back(pParentwnd->spriteset.Load(spriteName.GetFullPath().c_str()));
    }

    // --

    pRightbar->SetScrollbar(wxVERTICAL,   0, w, _map->Height() * pTileset->Height());
    pRightbar->SetScrollbar(wxHORIZONTAL, 0, h, _map->Width()  * pTileset->Width());
    xwin = ywin = 0;

    UpdateLayerList();
    InitAccelerators();
    InitMenu();

    _curLayer = 0;
    _entityEditor = new CEntityEditor(this, _map);
    _zoneEditor   = new ZoneEditor(this, _map);

    Show();
    SetFocus();
}

void CMapView::Paint()
{
    if (!pTileset || !_map)
        return; // .................................................................. really retarded.  wx likes to call this between deallocating my stuff, and actually destroying the frame. ;P

    pGraph->SetCurrent();
    pGraph->Clear();    

    Render();

    pGraph->ShowPage();
}

void CMapView::OnSize(wxSizeEvent& event)
{
    wxLayoutAlgorithm layout;
    layout.LayoutWindow(this, pRightbar);

    if (_map)
        UpdateScrollbars();
}

void CMapView::OnScroll(wxScrollWinEvent& event)
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
    pGraph->ShowPage();
}

void CMapView::OnClose()
{
    Log::Write("CMapView::Close");
    Log::Write("Releasing Map.");
    pParentwnd->map.Release(_map);
    Log::Write("Releasing tileset.");
    pParentwnd->vsp.Release(pTileset);

    Log::Write("Releasing spritesets.");
    for (std::vector<CSpriteSet*>::iterator i = pSprite.begin(); i!= pSprite.end(); i++)
        pParentwnd->spriteset.Release(*i);
    
    pSprite.clear();
    _map = 0;
    pTileset = 0;

    Log::Write("Destroying....");

    Destroy();
}

void CMapView::OnSave(wxCommandEvent& event)
{
    if (name.length())
    {
        // More path junk.  Absolute paths need to be converted to relative paths, or the game won't run in any other directory.
        wxFileName tileSetName(_map->GetVSPName().c_str());
        if (tileSetName.IsAbsolute())
            tileSetName.MakeRelativeTo(wxFileName(name.c_str(), "", "").GetPath());

        _map->SetVSPName(tileSetName.GetFullPath().c_str());

        _map->Save(name.c_str());
    }
    else
        OnSaveAs(event);
}

void CMapView::OnSaveAs(wxCommandEvent& event)
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

    if (!Path::Compare(name, oldname))
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


const void* CMapView::GetResource() const
{
    return _map;
}

void CMapView::GoPrevTile(wxEvent&)
{
    switch (_curLayer)
    {
    case lay_obstruction:
    case lay_entity:
    case lay_render:
        break;

    case lay_zone:
        _curZone = (_curZone - 1 + _map->NumZones()) % _map->NumZones();
        break;
    default:
        {
            int t = pTileset->CurTile() + 1;
            pTileset->SetCurTile(t % pTileset->Count());
            // And redraw the tileset window, if one is open.
            CTileSetView* tsv = (CTileSetView*)pParent->FindWindow(pTileset);
            if (tsv)    tsv->Render();
        }
    }
}

void CMapView::GoNextTile(wxEvent&)
{
    switch (_curLayer)
    {
    case lay_obstruction:
    case lay_entity:
    case lay_render:
        break;

    case lay_zone:
        _curZone = (_curZone + 1) % _map->NumZones();
        break;
    default:
        {
            int t = pTileset->CurTile() + pTileset->Count() - 1;    // This is just a cheap way to do wrap - around.
            pTileset->SetCurTile(t % pTileset->Count());        
            // And redraw the tileset window, if one is open.
            CTileSetView* tsv = (CTileSetView*)pParent->FindWindow(pTileset);
            if (tsv)    tsv->Render();
        }
    }
}

void CMapView::Zoom(int nZoomscale)
{
    int zoom = pGraph->Zoom() - nZoomscale;

    if (zoom < 1) zoom = 1;
    if (zoom > 255) zoom = 255;

    pGraph->Zoom(zoom);

    UpdateScrollbars();
    Render();   pGraph->ShowPage();
}

void CMapView::OnZoomIn(wxCommandEvent&)    { Zoom(1);  }
void CMapView::OnZoomOut(wxCommandEvent&)   { Zoom(-1); }
void CMapView::OnZoomIn2x(wxCommandEvent&)  { Zoom(2);  }
void CMapView::OnZoomOut2x(wxCommandEvent&) { Zoom(-2); }
void CMapView::OnZoomIn4x(wxCommandEvent&)  { Zoom(4);  }
void CMapView::OnZoomOut4x(wxCommandEvent&) { Zoom(-4); }
void CMapView::OnZoomNormal(wxCommandEvent&){ Zoom(pGraph->Zoom() - 16); }  // >:D

void CMapView::OnShowEntityEditor(wxCommandEvent&)
{
    _entityEditor->Show(true);
}

void CMapView::OnShowZoneEditor(wxCommandEvent&)
{
    _zoneEditor->Show(true);
}

void CMapView::OnShowVSP(wxCommandEvent&)
{
    pParent->Open(pTileset->GetVSP().Name());
}

void CMapView::OnShowScript(wxCommandEvent&)
{
    pParent->Open(Path::ReplaceExtension(name, "py"));
}

static char LayerNumberToRenderChar(int i)
{
    switch (i)
    {
    case lay_entity: return 'E';
    case lay_render: return 'R';
    default:
        if (i > 0)
            return "1234567890"[i];
        else
            return 0;
    }
}

void CMapView::OnMoveLayerUp(wxCommandEvent&)
{
    char c = LayerNumberToRenderChar(_curLayer);
    if (!c) return;

    std::string s = _map->GetRString();

    int pos = s.find(c);
    if (pos != std::string::npos && pos > 0)
    {
        s[pos] = s[pos - 1];
        s[pos - 1] = c;
        _map->SetRString(s);
        UpdateLayerList();
    }
}

void CMapView::OnMoveLayerDown(wxCommandEvent&)
{
    char c = LayerNumberToRenderChar(_curLayer);
    if (!c) return;

    std::string s = _map->GetRString();
    
    uint pos = s.find(c);
    if (pos != std::string::npos && pos < s.length() - 1)
    {
        s[pos] = s[pos + 1];
        s[pos + 1] = c;
        _map->SetRString(s);
        UpdateLayerList();
    }
}

void CMapView::OnDeleteLayer(wxCommandEvent&)
{
    int result = wxMessageBox("Are you sure you want to delete this layer?\nThis cannot be undone!", "You sure?", wxYES_NO | wxCENTRE, this);
    if (result == wxYES)
    {
        // Find any reference to the layer in the render string, and annihilate it.
        char c = "1234567890"[_curLayer]; // The character in the render string that represents this layer.

        std::string s = _map->GetRString();
        uint i = 0;
        while (i < s.length())
        {
            if (s[i] == c)
            {
                s.erase(s.begin() + i);
                continue;
            }
            
            if (s[i] > c && s[i] < '9')
                s[i]--;
            i++;
        }
        
        _map->SetRString(s);

        _map->DeleteLayer(_curLayer);
        UpdateLayerList();
    }
}

void CMapView::OnNewLayer(wxCommandEvent&)
{
    if (_map->NumLayers() < 10)
    {
        _map->AddLayer(10);
        std::string s = _map->GetRString();
        _map->SetRString(s + (char)('0' + _map->NumLayers()));
        UpdateLayerList();
    }
    else
        wxMessageBox("ika can only handle 10 layers right now.", "Error", wxOK | wxCENTER, this);
}

//------------------------------------------------------------

void CMapView::ScreenToMap(int& x, int& y)
{
    SMapLayerInfo l;
    _map->GetLayerInfo(l, _curLayer);

    x+=(xwin * l.pmulx / l.pdivx);
    y+=(ywin * l.pmuly / l.pdivy);
}

void CMapView::MapToTile(int& x, int& y)
{
    x /= pTileset->Width();
    y /= pTileset->Height();
}

void CMapView::ScreenToTile(int& x, int& y)
{
    ScreenToMap(x, y);
    MapToTile(x, y);
}

void CMapView::ScreenToTile(int& x, int& y, int layidx)
{
    ScreenToMap(x, y);

    SMapLayerInfo lay;
    _map->GetLayerInfo(lay, layidx);

    x = x * lay.pmulx / lay.pdivx;
    y = y * lay.pmuly / lay.pdivy;

    MapToTile(x, y);
}

void CMapView::HandleMouse(wxMouseEvent& event)
{
    if (event.GetWheelRotation())                   _editState->OnMouseWheel(event);
    else if (event.LeftDown() || event.RightDown()) _editState->OnMouseDown(event);
    else if (event.LeftUp()   || event.RightUp())   _editState->OnMouseUp(event);
    else if (event.Moving())                        _editState->OnMouseMove(event);
}

void CMapView::HandleMouseWheel(wxMouseEvent& event)
{
    const int delta = event.GetWheelRotation();

}

void CMapView::OnLayerChange(int lay)
{
    _curLayer = lay;
    if (nLayertoggle[lay] == hidden)
    {
        nLayertoggle[lay] = visible;
        Render();
        pGraph->ShowPage();
        pLayerlist->CheckItem(lay);
    }
}

void CMapView::OnLayerToggleVisibility(int lay, int newstate)
{
    nLayertoggle[lay] = newstate;
    Render();
    pGraph->ShowPage();
}

void CMapView::UpdateScrollbars()
{
    int w = pGraph->LogicalWidth();
    int h = pGraph->LogicalHeight();

    int maxx = _map->Width() * pTileset->Width();
    int maxy = _map->Height() * pTileset->Height();

    // clip the viewport
    if (xwin + w > maxx) xwin = maxx - w;
    if (ywin + h > maxy) ywin = maxy - h;
    if (xwin < 0) xwin = 0;
    if (ywin < 0) ywin = 0;

    pRightbar->SetScrollbar(wxHORIZONTAL, xwin, w, _map->Width() * pTileset->Width()  );
    pRightbar->SetScrollbar(wxVERTICAL, ywin, h,   _map->Height()* pTileset->Height() );
}

void CMapView::UpdateLayerList()
{
    // Fill up the layer info bar
    const string& s = _map->GetRString();

    pLayerlist->Clear();
    
    for (uint idx = 0; idx < s.length(); idx++)
    {
        char c = s[idx];
        if (c >='0' && c <='9')
        {
            if (c == '0')
                c = 10;
            else
                c -= '1';

            SMapLayerInfo lay;
            _map->GetLayerInfo(lay, c);
            pLayerlist->AppendItem(va("Layer %i", c + 1), c);
            pLayerlist->Check(pLayerlist->Number() - 1);

            nLayertoggle[c]= visible;
        }
        else if (c == 'E')
        {
            pLayerlist->AppendItem("Entities", lay_entity);
            pLayerlist->Check(pLayerlist->Number() - 1);
            nLayertoggle[lay_entity] = visible;
        }
        else if (c == 'R')
            pLayerlist->AppendItem("Render Hook", lay_render);
//        else if (c =='R')
//            pLayerlist->AppendItem("HookRetrace", -1);  // we don't do anything when the hookretrace "layer" is selected.
    }

    pLayerlist->AppendItem("Zones", lay_zone);
    pLayerlist->AppendItem("Obstructions", lay_obstruction);

    pLayerlist->SelectItem(_curLayer);
}

// ------------------------------ Rendering -------------------------

void CMapView::Render()
{
    const string& r = _map->GetRString();

    pGraph->SetCurrent();
    pGraph->Clear();

    for (uint i = 0; i < r.length(); i++)
    {
        if (r[i] >= '0' && r[i] <= '9')
        {
            int l = r[i] - '1';
            if (r[i] == '0')
                l = 10;

            if (l >= 0 && l < _map->NumLayers() && nLayertoggle[l] != hidden)
                RenderLayer(l);
        }
        else if (r[i]=='E')
            RenderEntities();
    }

    if (nLayertoggle[lay_obstruction])
        RenderInfoLayer(lay_obstruction);
    if (nLayertoggle[lay_zone])
        RenderInfoLayer(lay_zone);
 
    _editState->OnRender();
}

typedef std::pair<SMapEntity*, CSpriteSet*> EntSpritePair;
typedef std::list<EntSpritePair> EntRenderList;

void CMapView::RenderEntities()
{
    if (nLayertoggle[lay_entity] == hidden)
        return;

    EntRenderList entstodraw;
    int x2, y2;
    pGraph->GetClientSize(&x2, &y2);
    x2 += xwin;
    y2 += ywin;

    // 1) figure out which entities to draw
    for (int i = 0; i < _map->NumEnts(); i++)
    {
        SMapEntity& e = _map->GetEntity(i);

        if (e.x < xwin || e.y < ywin)   continue;
        if (e.x > x2   || e.y > y2)     continue;

        entstodraw.push_back(EntSpritePair(&e, pSprite[i]) );
    }

    // 2) y sort


    // 3) render!
    for (EntRenderList::iterator j = entstodraw.begin(); j!= entstodraw.end(); j++)
    {
        SMapEntity* pEnt = j->first;
        CSpriteSet* pSpriteset = j->second;

        if (pSpriteset)                                                                             // spritesets that couldn't be loaded are null pointers
            pGraph->Blit(pSpriteset->GetImage(0), pEnt->x - xwin, pEnt->y - ywin, true);
        else
            pGraph->RectFill(pEnt->x, pEnt->y, pTileset->Width(), pTileset->Height(), RGBA(0, 0, 0, 128)); // no spriteset found, draw a gray square
    }
}

// AGH I HATE THIS TODO: figure out a nice way to consolidate all this into a single function. (templates seem like a good idea)

void CMapView::RenderInfoLayer(int lay)
{
    int nWidth = pGraph->LogicalWidth();
    int nHeight = pGraph->LogicalHeight();

    int xw = xwin;
    int yw = ywin;

    int tx = pTileset->Width();
    int ty = pTileset->Height();

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
                    pGraph->RectFill(
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
                    pGraph->RectFill(
                        x * tx - nAdjx, y * ty - nAdjy,
                        tx, ty,
                        c);
            }
        }
    }

    glColor4f(1, 1, 1, 1);
}

void CMapView::RenderLayer(int lay)
{
    int nWidth = pGraph->LogicalWidth();
    int nHeight = pGraph->LogicalHeight();

    SMapLayerInfo l;
    _map->GetLayerInfo(l, lay);

    int xw = xwin * l.pmulx / l.pdivx;
    int yw = ywin * l.pmuly / l.pdivy;

    int tx = pTileset->Width();
    int ty = pTileset->Height();

    int nFirstx = xw / tx;
    int nFirsty = yw / ty;
    
    int nLenx = nWidth / tx + 2;
    int nLeny = nHeight / ty + 2;

    if (nFirstx + nLenx > _map->Width())  nLenx = _map->Width() - nFirstx;
    if (nFirsty + nLeny > _map->Height()) nLeny = _map->Height() - nFirsty;

    int nAdjx = (xw % tx);
    int nAdjy = (yw % ty);

    for (int y = 0; y <nLeny; y++)
    {
        for (int x = 0; x <nLenx; x++)
        {
            int t = _map->GetTile(x + nFirstx, y + nFirsty, lay);
            
            if (lay == 0 || t != 0)
                pGraph->Blit(
                    pTileset->GetImage(t),
                    x * tx - nAdjx, y * ty - nAdjy,
                    //tx, ty,
                    true);
        }
    }
}

void CMapView::RenderSelectionRect(Rect r, RGBA colour)
{
    /* 
     * All 4 sides of the rect are inclusive, so we just swap so that the lesser coords are top and left,
     * then multiply by the tile size, and add the tile size to the right and bottom edges, so the rect
     * draws properly.  Then we draw.
     */

    if (r.left > r.right)
    {
        swap(r.left, r.right);
    }
    if (r.top > r.bottom)
    {
        swap(r.bottom, r.top);
    }

    r.top *= pTileset->Height();
    r.left *= pTileset->Width();
    r.bottom = (r.bottom + 1) * pTileset->Height();
    r.right = (r.right + 1) * pTileset->Width();

    pGraph->Rect(r.left - xwin, r.top - ywin, r.Width(), r.Height(), colour);
}