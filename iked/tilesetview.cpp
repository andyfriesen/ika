
#include "TileSetView.h"
#include "main.h"

BEGIN_EVENT_TABLE(CTileSetView,IDocView)
    EVT_SCROLL(CTileSetView::OnScroll)
    EVT_SIZE(CTileSetView::OnSize)
    EVT_PAINT(CTileSetView::OnPaint)
    EVT_CLOSE(CTileSetView::OnClose)
END_EVENT_TABLE()

CTileSetView::CTileSetView(CMainWnd* parentwnd,const string& fname)
    : IDocView(parentwnd,fname),
      pParent(parentwnd)
{
    pGraph=new CGraphFrame(this);
    pGraph->SetSize(GetClientSize());

    pTileset=pParent->vsp.Load(fname);
}

// --------------------------------- events ---------------------------------

void CTileSetView::OnSave(wxCommandEvent& event)
{
    pTileset->Save(sName.c_str());
}

void CTileSetView::OnClick(wxMouseEvent& event)
{
}

void CTileSetView::OnPaint()
{
    wxPaintDC dc(this);

    int w,h;
    GetClientSize(&w,&h);

    const int tx=pTileset->Width();
    const int ty=pTileset->Height();

    int nTilewidth =w/tx;
    int nTileheight=h/ty;

    int nTile=0;

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

    pGraph->ShowPage();
}

void CTileSetView::OnSize(wxSizeEvent& event)
{
}

void CTileSetView::OnScroll(wxScrollEvent& event)
{

}

void CTileSetView::OnClose()
{
    pParent->vsp.Release(pTileset);

    Destroy();
}
