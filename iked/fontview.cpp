
#include "fontview.h"
#include "main.h"

BEGIN_EVENT_TABLE(CFontView,IDocView)

    EVT_SCROLLWIN(CFontView::OnScroll)
    EVT_SIZE(CFontView::OnSize)
    EVT_PAINT(CFontView::OnPaint)
    EVT_CLOSE(CFontView::OnClose)
    EVT_ERASE_BACKGROUND(CFontView::OnEraseBackground)

    EVT_LEFT_DOWN(CFontView::OnLeftClick)
    EVT_RIGHT_DOWN(CFontView::OnRightClick)

END_EVENT_TABLE()

CFontView::CFontView(CMainWnd* parentwnd,const string& fname):
IDocView(parentwnd,fname),
pParent(parentwnd),
sFilename(fname),
nCurfont(0)
{

    pGraph = new CGraphFrame(this);
    pGraph->SetSize(GetClientSize());

    pFontfile = new CFontFile();
    pFontfile->Load(sFilename.c_str());

    ywin=0;

}

void CFontView::OnRightClick(wxMouseEvent& event)
{
}

void CFontView::OnLeftClick(wxMouseEvent& event)
{
    int x,y;
    event.GetPosition(&x,&y);
    nCurfont=FontAt(x,y);

    Render();
}


void CFontView::Render()
{
    CPixelMatrix& rGlyph=pFontfile->GetGlyph(nCurfont);

    int nWidth, nHeight;
    GetClientSize(&nWidth, &nHeight);

    const int nTx=rGlyph.Width();
    const int nTy=rGlyph.Height();


    int nFontwidth=nWidth/nTx;
    int nFontheight=(nHeight/nTy)+1;
    int nFont=ywin*nFontwidth;

    pGraph->SetCurrent();
    pGraph->Clear();     

    for(int y=0; y<nFontheight; y++)
    {
        for(int x=0; x<nFontwidth; x++)
        {
            // Grab the font bitmap, blit, and move right along.
            //  -- khross

            // I am amazed that this allocation/deallocation is not slow. --andy
            CPixelMatrix& rBitmap=pFontfile->GetGlyph(nFont);
            CImage rImage(rBitmap);
            pGraph->ScaleBlit(rImage,x*nTx+1,y*nTy+1,
                rGlyph.Width(),rGlyph.Height(),
                true);
            // add zooming.

            nFont++;

            if (nFont>=pFontfile->NumGlyphs()) 
            {
                y=nFontheight+1;    // bomb out of both loops
                break;
            }

        }
    }

    int x2, y2;

    FontPos(nCurfont,x2,y2);
    pGraph->Rect(x2-1,y2-1,nTx+1,nTy+1,RGBA(255,255,255));
    pGraph->ShowPage();
}

void CFontView::FontPos(int fontidx,int& x,int& y) const
{
    CPixelMatrix& rGlyph=pFontfile->GetGlyph(fontidx);
    int nFontwidth=GetClientSize().GetWidth()/rGlyph.Width();
    

    x=fontidx%nFontwidth;
    y=fontidx/nFontwidth-ywin;

    x*=rGlyph.Width();
    y*=rGlyph.Height();
}

int CFontView::FontAt(int x,int y) const
{
    CPixelMatrix& rGlyph=pFontfile->GetGlyph(nCurfont);
    const int tx=rGlyph.Width();
    const int ty=rGlyph.Height();

    int nFontwidth = GetClientSize().GetWidth()/tx;

    x/=tx;      
    y/=ty;

    int t=(y+ywin)*nFontwidth+x;

    if (t>pFontfile->NumGlyphs()) return 0;
    return t;
}

void CFontView::OnPaint()
{
    if (!pGraph) 
        return; // Can't be too careful with these wacky paint messages. -- khross

    wxPaintDC dc(this);
    Render();
}

void CFontView::OnSave(wxCommandEvent& event)
{
    // FIXME: CFontFile::Save isn't implemented. :x
    pFontfile->Save(sFilename.c_str());
}

void CFontView::OnClose()
{
    
    Destroy();
  
}

void CFontView::OnSize(wxSizeEvent& event)
{
    pGraph->SetSize(GetClientSize());
    UpdateScrollbar();
}

void CFontView::OnScroll(wxScrollWinEvent& event)
{
    switch (event.m_eventType)
    {
    case wxEVT_SCROLLWIN_TOP:       ywin=0;                     break;
    case wxEVT_SCROLLWIN_BOTTOM:    ywin=pFontfile->NumGlyphs();  break; 
    case wxEVT_SCROLLWIN_LINEUP:    ywin--;                     break;
    case wxEVT_SCROLLWIN_LINEDOWN:  ywin++;                     break;
    case wxEVT_SCROLLWIN_PAGEUP:    ywin-=GetScrollThumb(wxVERTICAL);   break;
    case wxEVT_SCROLLWIN_PAGEDOWN:  ywin+=GetScrollThumb(wxVERTICAL);   break;
    default:                        ywin=event.GetPosition();   break;
    };

    UpdateScrollbar();
    Render();
}

void CFontView::UpdateScrollbar()
{
    CPixelMatrix& rGlyph=pFontfile->GetGlyph(nCurfont);
    int nWidth,nHeight;
    GetClientSize(&nWidth, &nHeight);

    int nFontwidth  = nWidth/rGlyph.Width();
    int nFontheight = nHeight/rGlyph.Height();

    int nTotalheight=(pFontfile->NumGlyphs())/nFontwidth;

    if (ywin>nTotalheight-nFontheight)  
        ywin=nTotalheight-nFontheight;

    if (ywin<0)                         
        ywin=0;

    SetScrollbar(wxVERTICAL,ywin,nFontheight,nTotalheight,true);
}
