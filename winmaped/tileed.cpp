#include "tileed.h"

void CTileEd::Execute(HINSTANCE hInst,HWND hWnd,VSP* vsp,int tileidx)
{
    pVsp=vsp;
    nCurtile=tileidx;
    
    CPixelMatrix src=pVsp->GetTile(tileidx);
    
    StartEdit(hInst,hWnd,src);
}

void CTileEd::UpdateData()
{
    // ... gah
    pVsp->GetTile(nCurtile)=curimage;
}

void CTileEd::GoPrev()
{
    if (nCurtile==0)
        return;
    
    if (bAltered)
    {
        int result=MessageBox(
            hWnd,
            "Your changes haven't been copied to the VSP yet.\nDo you wish to do so now?\nSelecting 'no' will discard any changes you have made.",
            "Woah there!",
            MB_YESNO);
        if (result==IDYES)
            UpdateData();
    }
    
    nCurtile--;
    curimage=pVsp->GetTile(nCurtile);
    PurgeUndo();
    PurgeRedo();
    bAltered=false;
}

void CTileEd::GoNext()
{
    if (nCurtile>=pVsp->NumTiles())
        return;
    
    if (bAltered)
    {
        int result=MessageBox(
            hWnd,
            "Your changes haven't been copied to the VSP yet.\nDo you wish to do so now?\nSelecting 'no' will discard any changes you have made.",
            "Woah there!",
            MB_YESNO);
        if (result==IDYES)
            UpdateData();
    }
    
    nCurtile++;
    curimage=pVsp->GetTile(nCurtile);
    PurgeUndo();
    PurgeRedo();
    bAltered=false;
}

void CTileEd::Resize(int newwidth,int newheight)
{
    // This'll resize the whole VSP.. when I get to it.
}

int CTileEd::HandleCommand(HWND hWnd,int wParam)
{
    switch (LOWORD(wParam))
    {
    case ID_TILEED_UPDATE:	UpdateData();	bAltered=false; return 0;
    case ID_TILEED_UPDATEANDEXIT:
        UpdateData();
        bAltered=false;
        PostMessage(hWnd,WM_CLOSE,0,0);
        return 0;
    }
    
    return 0;
}