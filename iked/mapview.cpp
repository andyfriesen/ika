// Note to self: Tweak this so that it uses the new CGraphFrame zooming stuff, and not its present hacked crap.
// Further note: delete these notes afterwards.

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
#include <gl\glu.h>

#include <list>
#include "wx\laywin.h"
#include "wx\sashwin.h"
#include "wx\checklst.h"

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
    // wxSashLayoutWindow tweak that passes scroll events to its parent.
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


    enum
    {
        id_zoomin = 100,
        id_zoomout,
        id_zoomnormal,
        id_zoomin2x,
        id_zoomin4x,
        id_zoomout2x,
        id_zoomout4x,

        id_filesave,
        id_filesaveas,
        id_fileclose,

        id_mapentities,
        id_mapzones,
        id_newlayer,
        id_vsp,
        id_script
    };
};

//-------------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(CMapView, wxMDIChildFrame)
    EVT_MENU(id_zoomin, CMapView::OnZoomIn)
    EVT_MENU(id_zoomout, CMapView::OnZoomOut)
    EVT_MENU(id_zoomnormal, CMapView::OnZoomNormal)

    EVT_MENU(id_zoomin2x, CMapView::OnZoomIn2x)
    EVT_MENU(id_zoomin4x, CMapView::OnZoomIn4x)
    EVT_MENU(id_zoomout2x, CMapView::OnZoomOut2x)
    EVT_MENU(id_zoomout4x, CMapView::OnZoomOut4x)

    EVT_MENU(id_mapentities, CMapView::OnShowEntityEditor)
    EVT_MENU(id_mapzones, CMapView::OnShowZoneEditor)
    EVT_MENU(id_newlayer, CMapView::OnNewLayer)
    EVT_MENU(id_vsp, CMapView::OnShowVSP)
    EVT_MENU(id_script, CMapView::OnShowScript)

    EVT_MENU(id_filesave, CMapView::OnSave)
    EVT_MENU(id_filesaveas, CMapView::OnSaveAs)
    EVT_MENU(id_fileclose, CMapView::OnClose)

    //EVT_PAINT(CMapView::OnPaint)
    EVT_ERASE_BACKGROUND(CMapView::OnErase)
    EVT_SIZE(CMapView::OnSize)
    EVT_SCROLLWIN(CMapView::OnScroll)
    EVT_CLOSE(CMapView::OnClose)
    
    EVT_MOUSE_EVENTS(CMapView::HandleMouse)
END_EVENT_TABLE()

CMapView::CMapView(CMainWnd* parent, int width, int height, const string& tilesetname)
    : IDocView(parent, "")
    , pParentwnd(parent)
    , nZoom(16)
{
    pMap = new Map;
    pMap->Resize(width, height);
    pMap->SetVSPName(tilesetname);
    pMap->Zones().push_back(SMapZone());
    Init();
}

CMapView::CMapView(CMainWnd* parent, const string& name)
    : IDocView(parent, name)
    , pParentwnd(parent)
    , nZoom(16)
{
    pMap = pParentwnd->map.Load(name);    
    Init();
}

void CMapView::InitAccelerators()
{
    vector < wxAcceleratorEntry> accel = pParent->CreateBasicAcceleratorTable();

    int p = accel.size();
    accel.resize(accel.size()+4);

    accel[p++].Set(wxACCEL_CTRL, (int)'S', id_filesave);
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
    filemenu->Insert(2, new wxMenuItem(filemenu, id_filesave, "&Save", "Save the map to disk."));
    filemenu->Insert(3, new wxMenuItem(filemenu, id_filesaveas, "Save &As", "Save the map under a new filename."));
    filemenu->Insert(4, new wxMenuItem(filemenu, id_fileclose, "&Close", "Close the map view."));
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

    mapmenu->Append(id_vsp, "&VSP");
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

    pLayerlist = new CLayerVisibilityControl(pLeftbar, -1, this);

    // Right side -- Map view
    pRightbar = new CMapSash(this, -1);
    pRightbar->SetAlignment(wxLAYOUT_RIGHT);

    pGraph = new CMapFrame(pRightbar, this);

    string sTilesetname = Path::Directory(name) + pMap->GetVSPName();   // get the absolute path to the map, and add it to the tileset filename
    pTileset = pParentwnd->vsp.Load(sTilesetname);                      // load the VSP
    if (!pTileset)
    {
        wxMessageBox(va("Unable to load tileset %s", sTilesetname.c_str()), "Error", wxOK | wxCENTER, this);
        Close();
        return;
    }

    // Load CHRs

    for (int i = 0; i < pMap->NumEnts(); i++)
    {
        // Get the absolute path.  I'm paranoid.
        string sFilename = Path::Directory(name) + pMap->GetEntity(i).sCHRname;
        pSprite.push_back(pParentwnd->spriteset.Load(sFilename));
    }

    // --

    pRightbar->SetScrollbar(wxVERTICAL, 0, w, pMap->Height()* pTileset->Height());
    pRightbar->SetScrollbar(wxHORIZONTAL, 0, h, pMap->Width()* pTileset->Width());
    xwin = ywin = 0;

    UpdateLayerList();
    InitAccelerators();
    InitMenu();

    nCurlayer = 0;
    //csrmode = mode_select;
    csrmode = mode_normal;
    _selection = Rect(5, 2, 8, 12);

    pEntityeditor = new CEntityEditor(this, pMap);
    _zoneeditor = new ZoneEditor(this, pMap);

    Show();
}

void CMapView::Paint()
{
    if (!pTileset || !pMap)
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

    if (pMap)
        UpdateScrollbars();
}

void CMapView::OnScroll(wxScrollWinEvent& event)
{
    if (!pMap)
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
    pParentwnd->map.Release(pMap);
    Log::Write("Releasing tileset.");
    pParentwnd->vsp.Release(pTileset);

    Log::Write("Releasing spritesets.");
    for (std::vector < CSpriteSet*>::iterator i = pSprite.begin(); i!= pSprite.end(); i++)
        pParentwnd->spriteset.Release(*i);
    
    pSprite.clear();
    pMap = 0;
    pTileset = 0;

    Log::Write("Destroying....");

    Destroy();
}

void CMapView::OnSave(wxCommandEvent& event)
{
    if (name.length())
        pMap->Save(name.c_str());
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

    name = dlg.GetPath().c_str();
    SetTitle(name.c_str());

    OnSave(event);
}


const void* CMapView::GetResource() const
{
    return pMap;
}

void CMapView::Zoom(int nZoomscale)
{
    nZoom = pGraph->Zoom() - nZoomscale;

    if (nZoom < 1) nZoom = 1;
    if (nZoom > 255) nZoom = 255;

    pGraph->Zoom(nZoom);

    UpdateScrollbars();
    Render();   pGraph->ShowPage();
}

void CMapView::OnZoomIn(wxCommandEvent& event)    { Zoom(1);  }
void CMapView::OnZoomOut(wxCommandEvent& event)   { Zoom(-1); }
void CMapView::OnZoomIn2x(wxCommandEvent& event)  { Zoom(2);  }
void CMapView::OnZoomOut2x(wxCommandEvent& event) { Zoom(-2); }
void CMapView::OnZoomIn4x(wxCommandEvent& event)  { Zoom(4);  }
void CMapView::OnZoomOut4x(wxCommandEvent& event) { Zoom(-4); }
void CMapView::OnZoomNormal(wxCommandEvent& event){ Zoom(nZoom - 16); }  // >:D

void CMapView::OnShowEntityEditor(wxCommandEvent& event)
{
    pEntityeditor->Show(true);
}

void CMapView::OnShowZoneEditor(wxCommandEvent&)
{
    _zoneeditor->Show(true);
}

void CMapView::OnShowVSP(wxCommandEvent& event)
{
    pParent->Open(pTileset->GetVSP().Name());
}

void CMapView::OnNewLayer(wxCommandEvent& event)
{
    if (pMap->NumLayers() < 10)
    {
        pMap->AddLayer(10);
        std::string s = pMap->GetRString();
        pMap->SetRString(s + (char)('0' + pMap->NumLayers()));
        UpdateLayerList();
    }
}

void CMapView::OnShowScript(wxCommandEvent& event)
{
    pParent->Open(Path::ReplaceExtension(name, "py"));
}

//------------------------------------------------------------

void CMapView::ScreenToMap(int& x, int& y)
{
    SMapLayerInfo l;
    pMap->GetLayerInfo(l, nCurlayer);

    x+=(xwin * l.pmulx / l.pdivx);
    y+=(ywin * l.pmuly / l.pdivy);
}

void CMapView::LayerEdit(wxMouseEvent& event)
{
    if (
       !(event.LeftIsDown() || event.RightIsDown()) ||
       !(event.Dragging()   || event.ButtonDown()) ) return;

    int x = event.GetPosition().x;
    int y = event.GetPosition().y;
    ScreenToMap(x, y);

    if (x == oldx && y == oldy) return;
    oldx = x; oldy = y;

    SMapLayerInfo l;
    pMap->GetLayerInfo(l, nCurlayer);

    int tilex =(x * l.pmulx / l.pdivx) / pTileset->Width();
    int tiley =(y * l.pmuly / l.pdivy) / pTileset->Height();

    switch (nCurlayer)
    {
    case lay_zone:      break;
    case lay_entity:    break;
    case lay_obstruction:
        {
            bool set = event.LeftIsDown() && !event.RightIsDown();      // Boolean logic is so neat.
            pMap->SetObs(tilex, tiley, set);
        }
        break;

    default:
        if (event.ShiftDown())
            pTileset->SetCurTile(pMap->GetTile(tilex, tiley, nCurlayer));
        else
            pMap->SetTile(tilex, tiley, nCurlayer, pTileset->CurTile());
        break;
    }

    Render();
    pGraph->ShowPage();
}

void CMapView::HandleMouse(wxMouseEvent& event)
{
    if (event.GetWheelRotation())
    {
        HandleMouseWheel(event);
        return;
    }
    
    switch (csrmode)
    {
    case mode_normal:
        LayerEdit(event);
        break;

    case mode_select:
        {
            _selection.right = event.GetPosition().x;
            _selection.bottom = event.GetPosition().y;
            ScreenToMap(_selection.right, _selection.bottom);
            _selection.right /= pTileset->Width();
            _selection.bottom /= pTileset->Height();
            Render();
            pGraph->ShowPage();
            break;
        }

    default:
        wxASSERT(0);
        break;
    }
}

void CMapView::HandleMouseWheel(wxMouseEvent& event)
{
    const int delta = event.GetWheelRotation();

    if (event.ControlDown())
    {
        // zoom!
        if (delta > 0)  Zoom(1);
        else            Zoom(-1);
    }
    else
    {
        // Change the current tile

        int t = pTileset->CurTile() + pTileset->Count();    // add the tile count so that we don't go negative
        if (delta > 0)  t++;
        else            t--;
        pTileset->SetCurTile(t % pTileset->Count());        // then drop it.  This is just a cheap way to do wrap - around.

        // And redraw the tileset window, if one is open.
        CTileSetView* tsv = (CTileSetView*)pParent->FindWindow(pTileset);
        if (tsv)    tsv->Render();
    }
}

void CMapView::OnLayerChange(int lay)
{
    nCurlayer = lay;
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

    int maxx = pMap->Width() * pTileset->Width();
    int maxy = pMap->Height() * pTileset->Height();

    // clip the viewport
    if (xwin + w > maxx) xwin = maxx - w;
    if (ywin + h > maxy) ywin = maxy - h;
    if (xwin < 0) xwin = 0;
    if (ywin < 0) ywin = 0;

    pRightbar->SetScrollbar(wxHORIZONTAL, xwin, w, pMap->Width() * pTileset->Width()  );
    pRightbar->SetScrollbar(wxVERTICAL, ywin, h,   pMap->Height()* pTileset->Height() );
}

void CMapView::UpdateLayerList()
{
    // Fill up the layer info bar
    const string& s = pMap->GetRString();

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
            pMap->GetLayerInfo(lay, c);
            pLayerlist->AppendItem(va("Layer %i", c + 1), c);
            pLayerlist->Check(pLayerlist->Number() - 1);

            nLayertoggle[c]= visible;
        }
        else if (c =='E')
        {
            pLayerlist->AppendItem("Entities", lay_entity);
            pLayerlist->Check(pLayerlist->Number() - 1);
            nLayertoggle[lay_entity] = visible;
        }
//        else if (c =='R')
//            pLayerlist->AppendItem("HookRetrace", -1);  // we don't do anything when the hookretrace "layer" is selected.
    }

    pLayerlist->AppendItem("Zones", lay_zone);
    pLayerlist->AppendItem("Obstructions", lay_obstruction);
}

// ------------------------------ Rendering -------------------------

void CMapView::Render()
{
    const string& r = pMap->GetRString();

    pGraph->SetCurrent();
    pGraph->Clear();

    for (uint i = 0; i < r.length(); i++)
    {
        if (r[i] >= '0' && r[i] <= '9')
        {
            int l = r[i] - '1';
            if (r[i] == '0')
                l = 10;

            if (l >= 0 && l < pMap->NumLayers() && nLayertoggle[l] != hidden)
                RenderLayer(l);
        }
        else if (r[i]=='E')
            RenderEntities();
    }

    if (nLayertoggle[lay_obstruction])
        RenderInfoLayer(lay_obstruction);
    if (nLayertoggle[lay_zone])
        RenderInfoLayer(lay_zone);
 
//    RenderSelectionRect();
}

typedef std::pair < SMapEntity*, CSpriteSet*> EntSpritePair;
typedef std::list < EntSpritePair> EntRenderList;

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
    for (int i = 0; i < pMap->NumEnts(); i++)
    {
        SMapEntity& e = pMap->GetEntity(i);

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

    if (nFirstx + nLenx > pMap->Width())  nLenx = pMap->Width()-nFirstx;
    if (nFirsty + nLeny > pMap->Height()) nLeny = pMap->Height()-nFirsty;

    int nAdjx = xw % tx;
    int nAdjy = yw % ty;

    if (lay == lay_obstruction)
    {
        for (int y = 0; y < nLeny; y++)
        {
            for (int x = 0; x < nLenx; x++)
            {
                if (pMap->IsObs(x + nFirstx, y + nFirsty))
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
                int z = pMap->GetZone(x + nFirstx, y + nFirsty);

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
    pMap->GetLayerInfo(l, lay);

    int xw = xwin * l.pmulx / l.pdivx;
    int yw = ywin * l.pmuly / l.pdivy;

    int tx = pTileset->Width();
    int ty = pTileset->Height();

    int nFirstx = xw / tx;
    int nFirsty = yw / ty;
    
    int nLenx = nWidth / tx + 2;
    int nLeny = nHeight / ty + 2;

    if (nFirstx + nLenx > pMap->Width())  nLenx = pMap->Width() - nFirstx;
    if (nFirsty + nLeny > pMap->Height()) nLeny = pMap->Height() - nFirsty;

    int nAdjx = (xw % tx);
    int nAdjy = (yw % ty);

    for (int y = 0; y <nLeny; y++)
    {
        for (int x = 0; x <nLenx; x++)
        {
            int t = pMap->GetTile(x + nFirstx, y + nFirsty, lay);
            
            if (lay == 0 || t != 0)
                pGraph->Blit(
                    pTileset->GetImage(t),
                    x * tx - nAdjx, y * ty - nAdjy,
                    //tx, ty,
                    true);
        }
    }
}

void CMapView::RenderSelectionRect()
{
    /* 
     * All 4 sides of the rect are inclusive, so we just swap so that the lesser coords are top and left,
     * then multiply by the tile size, and add the tile size to the right and bottom edges, so the rect
     * draws properly.  Then we draw.
     */

    Rect r = _selection;
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

    pGraph->Rect(r.left - xwin, r.top - ywin, r.Width(), r.Height(), RGBA(255, 255, 255));
}