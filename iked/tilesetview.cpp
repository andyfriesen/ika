
#include "TileSetView.h"
#include "main.h"

BEGIN_EVENT_TABLE(CTileSetView,IDocView)
    EVT_CLOSE(CTileSetView::OnClose)
    EVT_PAINT(CTileSetView::OnPaint)
END_EVENT_TABLE()

CTileSetView::CTileSetView(CMainWnd* parentwnd,const string& fname)
    : IDocView(parentwnd,fname),
      pParent(parentwnd)
{
    pGraph=new CGraphFrame(this);
    pGraph->SetSize(GetClientSize());

    pTileset=pParent->vsp.Load(fname);
}


void CTileSetView::OnClose()
{
    pParent->vsp.Release(pTileset);

    Destroy();
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

void CTileSetView::OnSave(wxCommandEvent& event)
{
    pTileset->Save(sName.c_str());
}