
#include "TileSetView.h"
#include "main.h"

BEGIN_EVENT_TABLE(CTileSetView,IDocView)
    EVT_SCROLLWIN(CTileSetView::OnScroll)
    EVT_SIZE(CTileSetView::OnSize)
    EVT_PAINT(CTileSetView::OnPaint)
    EVT_CLOSE(CTileSetView::OnClose)

    EVT_LEFT_DOWN(CTileSetView::OnLeftClick)
    EVT_RIGHT_DOWN(CTileSetView::OnRightClick)
END_EVENT_TABLE()

CTileSetView::CTileSetView(CMainWnd* parentwnd,const string& fname)
    : IDocView(parentwnd,fname),
      pParent(parentwnd)
{
    pGraph=new CGraphFrame(this);
    pGraph->SetSize(GetClientSize());

    pTileset=pParent->vsp.Load(fname);

    ywin=0;
}

// --------------------------------- events ---------------------------------

void CTileSetView::OnSave(wxCommandEvent& event)
{
    pTileset->Save(sName.c_str());
}

void CTileSetView::OnPaint()
{
    wxPaintDC dc(this);

    Render();
}

void CTileSetView::OnSize(wxSizeEvent& event)
{
    pGraph->SetSize(GetClientSize());

    UpdateScrollbar();
    Render();
}

void CTileSetView::OnScroll(wxScrollWinEvent& event)
{
    switch (event.m_eventType)
    {
    case wxEVT_SCROLLWIN_TOP:       ywin=0;                     break;
    case wxEVT_SCROLLWIN_BOTTOM:    ywin=pTileset->NumTiles();  break;  // guaranteed to be too big, so that the usual scroll handler will clip it
    case wxEVT_SCROLLWIN_LINEUP:    ywin--;                     break;
    case wxEVT_SCROLLWIN_LINEDOWN:  ywin++;                     break;
    case wxEVT_SCROLLWIN_PAGEUP:    ywin-=GetScrollThumb(wxVERTICAL);   break;
    case wxEVT_SCROLLWIN_PAGEDOWN:  ywin+=GetScrollThumb(wxVERTICAL);   break;
    default:                        ywin=event.GetPosition();   break;
    };

    UpdateScrollbar();
    Render();
}

void CTileSetView::OnClose()
{
    pParent->vsp.Release(pTileset);

    Destroy();
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
}

//---------------------------

void CTileSetView::Render()
{
    int w,h;
    GetClientSize(&w,&h);

    const int tx=pTileset->Width();
    const int ty=pTileset->Height();

    int nTilewidth =w/tx;
    int nTileheight=(h/ty)+1;

    int nTile=ywin*nTilewidth;                  // first tile to draw

    pGraph->SetCurrent();
    pGraph->Clear();

    for (int y=0; y<nTileheight; y++)
    {
        for (int x=0; x<nTilewidth; x++)
        {
            pTileset->DrawTile(x*tx,y*ty,nTile,*pGraph);
            nTile++;
        }
    }

    int x;
#ifndef MSVC
    int y;          // VC6 is retarded.  It doesn't handle for loop scoping right.
#endif

    TilePos(pTileset->CurTile(),x,y);
    pGraph->Rect(x-1,y-1,tx+1,ty+1,RGBA(255,255,255));

    pGraph->ShowPage();
}

void CTileSetView::UpdateScrollbar()
{
    int w,h;
    GetClientSize(&w,&h);

    int nTilewidth =w/pTileset->Width();
    int nTileheight=h/pTileset->Height();

    int nTotalheight=pTileset->NumTiles()/nTilewidth;

    if (ywin>nTotalheight-nTileheight)  ywin=nTotalheight-nTileheight;
    if (ywin<0)                         ywin=0;

    SetScrollbar(wxVERTICAL,ywin,nTileheight,nTotalheight,true);
}

int CTileSetView::TileAt(int x,int y) const
{
    const int tx=pTileset->Width();
    const int ty=pTileset->Height();

    int nTilewidth =GetClientSize().GetWidth()/tx;

    x/=tx;      y/=ty;

    int t=(y+ywin)*nTilewidth+x;

    if (t>pTileset->NumTiles()) return 0;
    return t;
}

// Returns the position at which the tile is drawn at
void CTileSetView::TilePos(int tileidx,int& x,int& y) const
{
    int nTilewidth=GetClientSize().GetWidth()/pTileset->Width();

    x=tileidx%nTilewidth;
    y=tileidx/nTilewidth-ywin;

    x*=pTileset->Width();
    y*=pTileset->Height();
}