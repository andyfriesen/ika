
#include "TileSetView.h"
#include "main.h"

#include "ImageView.h"

#include "wx/event.h"

namespace
{
    enum
    {
        id_deletetile = 100,
        id_inserttile,
        id_copytile,
        id_pasteinto,
        id_pasteover,
        id_insertandpaste,
        id_edittile,
        id_zoomin,
        id_zoomin2x,
        id_zoomin4x,
        id_zoomout,
        id_zoomout2x,
        id_zoomout4x,
        id_zoomnormal
    };

    class CTileSetFrame : public CGraphFrame
    {
        DECLARE_EVENT_TABLE()
        
        CTileSetView* pTilesetview;
    public:
        CTileSetFrame(wxWindow* parent, CTileSetView* tilesetview)
            : CGraphFrame(parent)
            , pTilesetview(tilesetview)
        {}

        void OnPaint(wxPaintEvent& event)
        {
            wxPaintDC blah(this);

            pTilesetview->Paint();
        }
    };

    BEGIN_EVENT_TABLE(CTileSetFrame, CGraphFrame)
        EVT_PAINT(CTileSetFrame::OnPaint)
    END_EVENT_TABLE();
};

BEGIN_EVENT_TABLE(CTileSetView,IDocView)
    EVT_SCROLLWIN(CTileSetView::OnScroll)
    EVT_SIZE(CTileSetView::OnSize)
    EVT_CLOSE(CTileSetView::OnClose)
    EVT_ERASE_BACKGROUND(CTileSetView::OnEraseBackground)

    EVT_LEFT_DOWN(CTileSetView::OnLeftClick)
    EVT_RIGHT_DOWN(CTileSetView::OnRightClick)
    EVT_MOUSEWHEEL(CTileSetView::OnMouseWheel)

    EVT_MENU(id_edittile,CTileSetView::OnEditTile)

    EVT_MENU(id_zoomnormal, CTileSetView::OnZoomNormal)
    EVT_MENU(id_zoomin, CTileSetView::OnZoomIn)
    EVT_MENU(id_zoomout, CTileSetView::OnZoomOut)

    EVT_MENU(id_zoomin2x, CTileSetView::OnZoomIn2x)
    EVT_MENU(id_zoomin4x, CTileSetView::OnZoomIn4x)
    EVT_MENU(id_zoomout2x, CTileSetView::OnZoomOut2x)
    EVT_MENU(id_zoomout4x, CTileSetView::OnZoomOut4x)

END_EVENT_TABLE()

CTileSetView::CTileSetView(CMainWnd* parentwnd,const string& fname)
    : IDocView(parentwnd,fname)
    , pParent(parentwnd)
    , ywin(0)
    , zoom(16)
{
    pGraph=new CTileSetFrame(this, this); //new CGraphFrame(this);
    pGraph->SetSize(GetClientSize());

    pTileset=pParent->vsp.Load(fname);

    wxMenuBar* menubar = pParent->CreateBasicMenu();
    wxMenu* viewmenu = new wxMenu;
    viewmenu->Append(id_zoomnormal, "Zoom %&100", "");
    viewmenu->Append(id_zoomin, "Zoom &In\t+", "");
    viewmenu->Append(id_zoomout, "Zoom &Out\t-", "");

    viewmenu->Append(id_zoomin2x, "Zoom In 2x", "");
    viewmenu->Append(id_zoomout2x, "Zoom Out 2x", "");

    viewmenu->Append(id_zoomin4x, "Zoom In 4x", "");
    viewmenu->Append(id_zoomout4x, "Zoom Out 4x", "");

    menubar->Append(viewmenu, "&View");
    SetMenuBar(menubar);

    pContextmenu=new wxMenu();
    pContextmenu->Append(id_deletetile,"Delete");
    pContextmenu->Append(id_inserttile,"Insert");
    pContextmenu->Append(id_copytile,"Copy");
    pContextmenu->Append(id_pasteinto,"Paste into");
    pContextmenu->Append(id_pasteover,"Paste over");
    pContextmenu->Append(id_insertandpaste,"Insert and paste");
    pContextmenu->AppendSeparator();
    pContextmenu->Append(id_edittile,"Edit");
}

CTileSetView::~CTileSetView()
{
    delete pContextmenu;

    pParent->vsp.Release(pTileset);
    pTileset = 0;
}

// --------------------------------- events ---------------------------------

void CTileSetView::OnSave(wxCommandEvent& event)
{
    pTileset->Save(name.c_str());
}

void CTileSetView::Paint()
{
    if (!pTileset)
        return; // blech

    Render();
}

void CTileSetView::OnSize(wxSizeEvent& event)
{
    pGraph->SetSize(GetClientSize());

    UpdateScrollbar();
}

void CTileSetView::OnScroll(wxScrollWinEvent& event)
{
    if (event.m_eventType == wxEVT_SCROLLWIN_TOP)           ywin=0;                     
    else if (event.m_eventType == wxEVT_SCROLLWIN_BOTTOM)   ywin=pTileset->Count();        // guaranteed to be too big, so that the usual scroll handler will clip it
    else if (event.m_eventType == wxEVT_SCROLLWIN_LINEUP)   ywin--;                     
    else if (event.m_eventType == wxEVT_SCROLLWIN_LINEDOWN) ywin++;                     
    else if (event.m_eventType == wxEVT_SCROLLWIN_PAGEUP)   ywin-=GetScrollThumb(wxVERTICAL);   
    else if (event.m_eventType == wxEVT_SCROLLWIN_PAGEDOWN) ywin+=GetScrollThumb(wxVERTICAL);   
    else                                                    ywin=event.GetPosition();

    UpdateScrollbar();
    Render();
}

void CTileSetView::OnClose(wxCommandEvent& event)
{
    Destroy();
}

const void* CTileSetView::GetResource() const
{
    return pTileset;
}

void CTileSetView::OnLeftClick(wxMouseEvent& event)
{
    int x,y;
    event.GetPosition(&x,&y);

    pTileset->SetCurTile(TileAt(x,y));
    Render();
}

void CTileSetView::OnRightClick(wxMouseEvent& event)
{
    PopupMenu(pContextmenu,event.GetPosition());
    nTile=TileAt(event.GetX(),event.GetY());
}

void CTileSetView::OnMouseWheel(wxMouseEvent& event)
{
    int i = event.m_wheelRotation / event.m_wheelDelta;
    pTileset->SetCurTile(pTileset->CurTile() + i);
    Render();
}

//---------------------------

void CTileSetView::Zoom(int factor)
{
    zoom -= factor;
    if (zoom < 1) zoom = 1;
    if (zoom > 255) zoom = 255;

    Render();
    pGraph->ShowPage();
    UpdateScrollbar();
}

void CTileSetView::OnEditTile(wxCommandEvent& event)
{
//    pParent->
    pParent->OpenDocument( new CImageView(pParent,&pTileset->Get(nTile)) );
}

void CTileSetView::OnZoomNormal(wxCommandEvent&)    {   Zoom(16 - zoom);  }
void CTileSetView::OnZoomIn(wxCommandEvent&)        {   Zoom(1);    }
void CTileSetView::OnZoomOut(wxCommandEvent&)       {   Zoom(-1);   }
void CTileSetView::OnZoomIn2x(wxCommandEvent&)      {   Zoom(2);    }
void CTileSetView::OnZoomOut2x(wxCommandEvent&)     {   Zoom(-2);   }
void CTileSetView::OnZoomIn4x(wxCommandEvent&)      {   Zoom(4);    }
void CTileSetView::OnZoomOut4x(wxCommandEvent&)     {   Zoom(-4);   }
//---------------------------

void CTileSetView::Render()
{
    int w,h;
    GetClientSize(&w,&h);

    const int tx=pTileset->Width() * 16 / zoom;
    const int ty=pTileset->Height() * 16 / zoom;

    int nTilewidth =w/tx;
    int nTileheight=(h/ty)+1;

    int nTile=ywin*nTilewidth;                  // first tile to draw

    pGraph->SetCurrent();
    pGraph->Clear();

    for (int y=0; y<nTileheight; y++)
    {
        for (int x=0; x<nTilewidth; x++)
        {
            pGraph->ScaleBlit(pTileset->GetImage(nTile),
                x * tx, y * ty,
                tx, ty,
                false);

            nTile++;

            if (nTile>pTileset->Count())
            {
                y=nTileheight;  // hack, so that we bomb out of the outer loop too.
                break;
            }
        }
    }

    int cx,cy;
    TilePos(pTileset->CurTile(),cx,cy);
    pGraph->Rect(cx-1,cy-1,tx+1,ty+1,RGBA(255,255,255));

    pGraph->ShowPage();
}

void CTileSetView::UpdateScrollbar()
{
    int w,h;
    GetClientSize(&w, &h);

    int nTilewidth  = w / pTileset->Width() * zoom / 16;
    int nTileheight = h / pTileset->Height() * zoom / 16;

    int nTotalheight = pTileset->Count() / nTilewidth;

    if (ywin > nTotalheight - nTileheight)  ywin = nTotalheight - nTileheight;
    if (ywin < 0)                           ywin = 0;

    SetScrollbar(wxVERTICAL, ywin, nTileheight, nTotalheight, true);
}

int CTileSetView::TileAt(int x,int y) const
{
    const int tx = pTileset->Width() * 16 / zoom;
    const int ty = pTileset->Height() * 16 / zoom;

    int nTilewidth = GetClientSize().GetWidth() / tx;

    x /= tx;      y /= ty;

    int t = (y + ywin) * nTilewidth + x;

    if (t > pTileset->Count()) return 0;
    return t;
}

// Returns the position at which the tile is drawn at
void CTileSetView::TilePos(int tileidx,int& x,int& y) const
{
    int nTilewidth = GetClientSize().GetWidth() / (pTileset->Width() * 16 / zoom);

    x = tileidx % nTilewidth;
    y = tileidx / nTilewidth - ywin;

    x *= pTileset->Width() * 16 / zoom;
    y *= pTileset->Height() * 16 / zoom;
}