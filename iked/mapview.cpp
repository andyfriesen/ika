#include "mapview.h"
#include "main.h"
#include "graph.h"
#include "tileset.h"
#include "log.h"
#include "layervisibilitycontrol.h"
#include <gl\glu.h>

#include <wx\laywin.h>
#include <wx\sashwin.h>
#include <wx\checklst.h>

/*

    blegh, didn't want this to get complicated.

    wxMDIChildFrame
        |----------------------|
        |                      |
    CMapSash            wxSashLayoutWindow
        |                      |
    CGraphFrame         wxCheckListBox
    (Map rendering)     (Layer visibility)

*/

namespace
{
    // wxSashLayoutWindow tweak that passes scroll events to its parent.
    class CMapSash : public wxSashLayoutWindow
    {
    public:
        CMapSash(wxWindow* parent, int id)
            : wxSashLayoutWindow(parent,id)
        {}

        void ScrollRel(wxScrollWinEvent& event,int amount)
        {
            int max=GetScrollRange(event.GetOrientation());
            int thumbsize=GetScrollThumb(event.GetOrientation());

            amount+=GetScrollPos(event.GetOrientation());
            if (amount<0) amount=0;
            if (amount>max-thumbsize)
                amount=max-thumbsize;

            ((CMapView*)GetParent())->OnScroll(wxScrollWinEvent(-1,amount,event.GetOrientation()));
        }

        void ScrollTo(wxScrollWinEvent& event,int pos)
        {
            int max=GetScrollRange(event.GetOrientation());
            int thumbsize=GetScrollThumb(event.GetOrientation());

            if (pos<0) pos=0;
            if (pos>max-thumbsize)
                pos=max-thumbsize;

            ((CMapView*)GetParent())->OnScroll(wxScrollWinEvent(-1,pos,event.GetOrientation()));
        }

        void ScrollTop(wxScrollWinEvent& event)         {   ScrollTo(event,0);      }
        void ScrollBottom(wxScrollWinEvent& event)      {   ScrollTo(event,GetScrollRange(event.GetOrientation()));     }
        
        void ScrollLineUp(wxScrollWinEvent& event)      {   ScrollRel(event,-1);    }
        void ScrollLineDown(wxScrollWinEvent& event)    {   ScrollRel(event,+1);    }

        void ScrollPageUp(wxScrollWinEvent& event)      {   ScrollRel(event,-GetScrollThumb(event.GetOrientation()));   }
        void ScrollPageDown(wxScrollWinEvent& event)    {   ScrollRel(event,+GetScrollThumb(event.GetOrientation()));   }
    
        void OnScroll(wxScrollWinEvent& event)
        {
            ((CMapView*)GetParent())->OnScroll(event);
           
        }

        void OnMouseEvent(wxMouseEvent& event)
        {
            wxPostEvent(GetParent(),event);
        }

        DECLARE_EVENT_TABLE()
    };

    BEGIN_EVENT_TABLE(CMapSash,wxSashLayoutWindow)
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

    const int nZoomscale=16;
};

//-------------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(CMapView,wxMDIChildFrame)
    EVT_MENU(CMapView::id_zoomin,CMapView::OnZoomIn)
    EVT_MENU(CMapView::id_zoomout,CMapView::OnZoomOut)
    EVT_MENU(CMapView::id_zoomnormal,CMapView::OnZoomNormal)

    EVT_MENU(CMapView::id_zoomin2x,CMapView::OnZoomIn2x)
    EVT_MENU(CMapView::id_zoomin4x,CMapView::OnZoomIn4x)
    EVT_MENU(CMapView::id_zoomout2x,CMapView::OnZoomOut2x)
    EVT_MENU(CMapView::id_zoomout4x,CMapView::OnZoomOut4x)

    EVT_MENU(CMapView::id_filesave,CMapView::OnSave)
    EVT_MENU(CMapView::id_filesaveas,CMapView::OnSaveAs)
    EVT_MENU(CMapView::id_fileclose,CMapView::OnClose)

    EVT_PAINT(CMapView::OnPaint)
    EVT_ERASE_BACKGROUND(CMapView::OnErase)
    EVT_SIZE(CMapView::OnSize)
    EVT_SCROLLWIN(CMapView::OnScroll)
    EVT_CLOSE(CMapView::OnClose)
    
    EVT_MOUSE_EVENTS(CMapView::HandleMouse)
END_EVENT_TABLE()

CMapView::CMapView(CMainWnd* parent,const string& name)
:   IDocView(parent,name),

    pParentwnd(parent),
    nZoom(32)
{
    int w,h;
    GetClientSize(&w,&h);

    // Left side -- layer properties
    pLeftbar=new wxSashLayoutWindow(this,-1);
    pLeftbar->SetAlignment(wxLAYOUT_LEFT);
    pLeftbar->SetOrientation(wxLAYOUT_VERTICAL);
    pLeftbar->SetDefaultSize(wxSize(100,100));
    pLeftbar->SetSashVisible(wxSASH_RIGHT,true);

    pLayerlist=new CLayerVisibilityControl(pLeftbar,-1,this);

    // Right side -- Map view
    pRightbar=new CMapSash(this,-1);
    pRightbar->SetAlignment(wxLAYOUT_RIGHT);

    pGraph=new CGraphFrame(pRightbar);

    // Get resources
    pMap=pParentwnd->map.Load(name);                                    // load the map
        
    string sTilesetname = Path::Directory(name) + pMap->GetVSPName();   // get the absolute path to the map, and add it to the tileset filename
    pTileset=pParentwnd->vsp.Load(sTilesetname);                        // load the VSP

    pRightbar->SetScrollbar(wxVERTICAL,0,w,pMap->Height()*pTileset->Height());
    pRightbar->SetScrollbar(wxHORIZONTAL,0,h,pMap->Width()*pTileset->Width());
    xwin=ywin=0;

    InitLayerVisibilityControl();
    InitAccelerators();
    InitMenu();

    nCurlayer=0;
    csrmode=mode_normal;

    Show();
}

void CMapView::InitLayerVisibilityControl()
{
    // Fill up the layer info bar
    const string& s=pMap->GetRString();
    
    for (int idx=0; idx<s.length(); idx++)
    {
        char c=s[idx];
        if (c>='0' && c<='9')
        {
            c= c=='0' ? 10 : c-'1';

            SMapLayerInfo lay;
            pMap->GetLayerInfo(lay,c);
            pLayerlist->AppendItem(va("Layer %i",c),c);
            pLayerlist->Check(pLayerlist->Number()-1);
        }
        else if (c=='E')
            pLayerlist->AppendItem("Entities",lay_entity);
//        else if (c=='R')
//            pLayerlist->AppendItem("HookRetrace",-1);  // we don't do anything when the hookretrace "layer" is selected.
    }

    pLayerlist->AppendItem("Zones",lay_zone);
    pLayerlist->AppendItem("Obstructions",lay_obstruction);
}

void CMapView::InitAccelerators()
{
    vector<wxAcceleratorEntry> accel=pParent->CreateBasicAcceleratorTable();

    int p=accel.size();
    accel.resize(accel.size()+4);

    accel[p++].Set(wxACCEL_CTRL,(int)'S',id_filesave);
    accel[p++].Set(0,(int)'+',id_zoomin);
    accel[p++].Set(0,(int)'-',id_zoomout);
    accel[p++].Set(0,(int)'=',id_zoomnormal);

    wxAcceleratorTable table(p,&*accel.begin());
    SetAcceleratorTable(table);
}

void CMapView::InitMenu()
{
    wxMenuBar* menubar=pParent->CreateBasicMenu();

    wxMenu* filemenu=menubar->Remove(0);
    filemenu->Insert(2,new wxMenuItem(filemenu,id_filesave,"&Save","Save the map to disk."));
    filemenu->Insert(3,new wxMenuItem(filemenu,id_filesaveas,"Save &As","Save the map under a new filename."));
    filemenu->Insert(4,new wxMenuItem(filemenu,id_fileclose,"&Close","Close the map view."));
    menubar->Append(filemenu,"&File");

    wxMenu* viewmenu=new wxMenu;
    
    viewmenu->Append(id_zoomnormal,"Zoom %&100","");
    viewmenu->AppendSeparator();
    
    viewmenu->Append(id_zoomin,"Zoom &In\t+","");
    viewmenu->Append(id_zoomout,"Zoom &Out\t-","");

    viewmenu->Append(id_zoomin2x,"Zoom In 2x","");
    viewmenu->Append(id_zoomout2x,"Zoom Out 2x","");

    viewmenu->Append(id_zoomin4x,"Zoom In 4x","");
    viewmenu->Append(id_zoomout4x,"Zoom Out 4x","");

    menubar->Append(viewmenu,"&View");

    SetMenuBar(menubar);
}

void CMapView::OnPaint()
{
    wxPaintDC paintdc(this);

    pGraph->SetCurrent();
    pGraph->Clear();    

    Render();

    pGraph->ShowPage();
}

void CMapView::OnSize(wxSizeEvent& event)
{
    wxLayoutAlgorithm layout;
    layout.LayoutWindow(this,pRightbar);

    UpdateScrollbars();
}

void CMapView::OnScroll(wxScrollWinEvent& event)
{
    switch (event.GetOrientation())
    {
    case wxHORIZONTAL:  xwin=event.GetPosition();   break;
    case wxVERTICAL:    ywin=event.GetPosition();   break;
    }

    UpdateScrollbars();

    Render();
    pGraph->ShowPage();
}

void CMapView::OnClose()
{
    pParentwnd->map.Release(pMap);
    pParentwnd->vsp.Release(pTileset);
    Destroy();
}

void CMapView::OnSave(wxCommandEvent& event)
{
    if (sName.length())
    {
        pMap->Save(sName.c_str());
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

    int result=dlg.ShowModal();
    if (result==wxID_CANCEL)
        return;

    sName=dlg.GetFilename().c_str();
    SetTitle(sName.c_str());

    OnSave(event);
}


void CMapView::Zoom(const int& nZoomscale)
{
    int nTest=nZoom;
    nTest+=nZoomscale;

    if (nTest>32)   nTest=32;
    if (nTest<=0)   nTest=1;

    nZoom=nTest;
    UpdateScrollbars();
    Render();   pGraph->ShowPage();
}


void CMapView::OnZoomIn(wxCommandEvent& event)    { Zoom(1);  }
void CMapView::OnZoomOut(wxCommandEvent& event)   { Zoom(-1); }
void CMapView::OnZoomIn2x(wxCommandEvent& event)  { Zoom(2);  }
void CMapView::OnZoomOut2x(wxCommandEvent& event) { Zoom(-2); }
void CMapView::OnZoomIn4x(wxCommandEvent& event)  { Zoom(4);  }
void CMapView::OnZoomOut4x(wxCommandEvent& event) { Zoom(-4); }

void CMapView::OnZoomNormal(wxCommandEvent& event)
{
    nZoom=nZoomscale;
    UpdateScrollbars();
    Render();   pGraph->ShowPage();
}

//------------------------------------------------------------

void CMapView::ScreenToMap(int& x,int& y)
{
    SMapLayerInfo l;
    pMap->GetLayerInfo(l,nCurlayer);

    x=x*nZoomscale/nZoom;
    y=y*nZoomscale/nZoom;

    x+=(xwin*l.pmulx/l.pdivx);
    y+=(ywin*l.pmuly/l.pdivy);
}

void CMapView::HandleLayerEdit(wxMouseEvent& event)
{
    if (!event.LeftIsDown()) return;

    int x=event.GetPosition().x;
    int y=event.GetPosition().y;
    ScreenToMap(x,y);

    if (x==oldx && y==oldy) return;
    oldx=x; oldy=y;

    SMapLayerInfo l;
    pMap->GetLayerInfo(l,nCurlayer);

    int tilex=(x*l.pmulx/l.pdivx) / pTileset->Width();
    int tiley=(y*l.pmuly/l.pdivy) / pTileset->Height();

    switch (csrmode)
    {
    case mode_normal:
        pMap->SetTile(tilex,tiley,nCurlayer,pTileset->CurTile());
        break;
    }

    Render();
    pGraph->ShowPage();
}

void CMapView::HandleMouse(wxMouseEvent& event)
{
    switch (csrmode)
    {
    case lay_entity:
    case lay_zone:
    case lay_obstruction:
        // NYI
        break;
    default:
        HandleLayerEdit(event);
        break;
    }
}

void CMapView::OnLayerChange(int lay)
{
    nCurlayer=lay;
    if (nLayertoggle[lay]==hidden)
    {
        nLayertoggle[lay]=visible;
        Render();
        pGraph->ShowPage();
        pLayerlist->CheckItem(lay);
    }
}

void CMapView::OnLayerToggleVisibility(int lay,int newstate)
{
    nLayertoggle[lay]=newstate;
    Render();
    pGraph->ShowPage();
}

void CMapView::UpdateScrollbars()
{
    int w,h;
    pGraph->GetClientSize(&w,&h);

    // not using *= here for a reason.  Integer math, remember. ;)
    w=w*nZoomscale/nZoom;
    h=h*nZoomscale/nZoom;

    int a=nZoomscale;

    int maxx=pMap->Width()*pTileset->Width();
    int maxy=pMap->Height()*pTileset->Height();

    // clip the viewport
    if (xwin+w>maxx) xwin=maxx-w;
    if (ywin+h>maxy) ywin=maxy-h;
    if (xwin<0) xwin=0;
    if (ywin<0) ywin=0;

    pRightbar->SetScrollbar(wxHORIZONTAL,xwin,w, pMap->Width() *pTileset->Width()  );
    pRightbar->SetScrollbar(wxVERTICAL,ywin,h,   pMap->Height()*pTileset->Height() );
}

// ------------------------------ Rendering -------------------------

void CMapView::Render()
{
    const string& r=pMap->GetRString();

    pGraph->SetCurrent();
    pGraph->Clear();

    for (int i=0; i<r.length(); i++)
    {
        if (r[i]>='0' && r[i]<='9')
        {
            int l=r[i]-'1';
            if (r[i]=='0') l=10;
            
            if (l>=0 && l<pMap->NumLayers() && nLayertoggle[l]!=hidden)
                RenderLayer(l);
        }
    }
}

void CMapView::RenderLayer(int lay)
{
    int nWidth,nHeight;

    pGraph->GetClientSize(&nWidth,&nHeight);

    nWidth=nWidth*nZoomscale/nZoom;
    nHeight=nHeight*nZoomscale/nZoom;

    SMapLayerInfo l;
    pMap->GetLayerInfo(l,lay);

    int xw=xwin*l.pmulx/l.pdivx;
    int yw=ywin*l.pmuly/l.pdivy;

    int tx=pTileset->Width();
    int ty=pTileset->Height();

    int nFirstx=xw/tx;
    int nFirsty=yw/ty;
    
    int nLenx=nWidth/tx+2;
    int nLeny=nHeight/ty+2;

    if (nFirstx+nLenx>pMap->Width())  nLenx=pMap->Width()-nFirstx;
    if (nFirsty+nLeny>pMap->Height()) nLeny=pMap->Height()-nFirsty;

    int nAdjx=(xw%tx)*nZoom/nZoomscale;
    int nAdjy=(yw%ty)*nZoom/nZoomscale;

    tx=tx*nZoom/nZoomscale;
    ty=ty*nZoom/nZoomscale;

    for (int y=0; y<nLeny; y++)
    {
        for (int x=0; x<nLenx; x++)
        {
            int t=pMap->GetTile(x+nFirstx, y+nFirsty, lay);
            
            if (lay==0 || t!=0)
                pGraph->ScaleBlit(
                    pTileset->GetImage(t),
                    x*tx-nAdjx, y*ty-nAdjy,
                    tx,ty,
                    true);
        }
    }
}