
#include "wx/event.h"

#include "fontview.h"
#include "main.h"

namespace
{
    class CFontFrame : public CGraphFrame
    {
        DECLARE_EVENT_TABLE()

        CFontView* pFontview;

    public:
        CFontFrame(wxWindow* parent, CFontView* fontview)
            : CGraphFrame(parent)
            , pFontview(fontview)
        {}

        void OnPaint(wxPaintEvent& event)
        {
            wxPaintDC dc(this);

            pFontview->Paint();
        }
    };

    BEGIN_EVENT_TABLE(CFontFrame, CGraphFrame)
        EVT_PAINT(CFontFrame::OnPaint)
    END_EVENT_TABLE()
}

BEGIN_EVENT_TABLE(CFontView,IDocView)

    EVT_SCROLLWIN(CFontView::OnScroll)
    EVT_CLOSE(CFontView::OnClose)
    EVT_ERASE_BACKGROUND(CFontView::OnEraseBackground)

    EVT_LEFT_DOWN(CFontView::OnLeftClick)
    EVT_RIGHT_DOWN(CFontView::OnRightClick)

    EVT_MENU(CFontView::id_filesave,CFontView::OnSave)
    EVT_MENU(CFontView::id_filesaveas,CFontView::OnSaveAs)
    EVT_MENU(CFontView::id_optionscolor,CFontView::OnChangeBackgroundColor)

END_EVENT_TABLE()

CFontView::CFontView(CMainWnd* parentwnd,const string& fname)
    : IDocView(parentwnd,fname)
    , pParent(parentwnd)
    , sFilename(fname)
    , nCurfont(0)
    , ywin(0)
{

    pGraph = new CFontFrame(this, this);
    pGraph->SetSize(GetClientSize());

    pFontfile = new CFontFile();
    pFontfile->Load(sFilename.c_str());

    wxMenuBar* menubar=pParent->CreateBasicMenu();
    wxMenu* filemenu=menubar->Remove(0);
    filemenu->Insert(2,new wxMenuItem(filemenu,id_filesave,"&Save","Save the font to disk."));
    filemenu->Insert(3,new wxMenuItem(filemenu,id_filesaveas,"Save &As","Save the font under a new filename."));
    filemenu->Insert(4,new wxMenuItem(filemenu,id_fileclose,"&Close","Close the font window."));
    menubar->Append(filemenu,"&File");

    wxMenu* optionsmenu = new wxMenu;
    menubar->Append(optionsmenu,"&Options");
    optionsmenu->Append(id_optionscolor,"Change background color...","");

    SetMenuBar(menubar);
}

CFontView::~CFontView()
{
//    pParent->font.Release(pFontfile);
    delete pFontfile;
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

    int tx = 0, ty = 0;
    int nWidth, nHeight;

    GetClientSize(&nWidth, &nHeight);

    tx=pFontfile->Width();
    ty=pFontfile->Height();

    int nFontwidth = nWidth / tx;
    int nFontheight = (nHeight / ty) + 1;
    int nFont=ywin * nFontwidth;

    pGraph->SetCurrent();
    pGraph->Clear();     

    for(int y = 0; y < nFontheight; y++)
    {
        for(int x = 0; x < nFontwidth; x++)
        {
            // Grab the font bitmap, blit, and move right along.
            //  -- khross

            // Too slow. -- andy
            CPixelMatrix& rBitmap = pFontfile->GetGlyph(nFont);
            CImage rImage(rBitmap);
            pGraph->ScaleBlit(rImage, x * tx + 1, y * ty + 1,
                rBitmap.Width(), rBitmap.Height(), true);

            // TODO: add zooming.

            nFont++;

            if (nFont >= pFontfile->NumGlyphs()) 
                goto nomoredrawing; // abort the loop

            nFontwidth = nWidth / tx;
            nFontheight = (nHeight / ty) + 1;

        }
    }
nomoredrawing:

    int x2, y2;
    tx = pFontfile->Width();
    ty = pFontfile->Height();

    FontPos(nCurfont, x2, y2);
    pGraph->Rect(x2 - 1, y2 - 1, tx + 1, ty + 1, RGBA(255, 255, 255));
    pGraph->ShowPage();
}

void CFontView::FontPos(int fontidx,int& x,int& y) const
{
    int nFontwidth=GetClientSize().GetWidth()/pFontfile->Width();
    

    x=fontidx%nFontwidth;
    y=fontidx/nFontwidth-ywin;

    x*=pFontfile->Width();
    y*=pFontfile->Height();
}

int CFontView::FontAt(int x,int y) const
{
    const int tx=pFontfile->Width();
    const int ty=pFontfile->Height();

    int nFontwidth = GetClientSize().GetWidth()/tx;

    x/=tx;      
    y/=ty;

    int t=(y+ywin)*nFontwidth+x;

    if (t>pFontfile->NumGlyphs()) return 0;
    return t;
}

void CFontView::Paint()
{
    if (!pFontfile)
        return; // Can't be too careful with these wacky paint messages. -- khross

    Render();
}

void CFontView::OnSave(wxCommandEvent& event)
{
    // FIXME: CFontFile::Save isn't implemented. :x
    pFontfile->Save(sFilename.c_str());
}

void CFontView::OnSaveAs(wxCommandEvent& event)
{
    wxFileDialog dlg
    (
        this,
        "Open File",
        "",
        "",
        "Font files (*.fnt)|*.fnt|"
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
    if (event.m_eventType == wxEVT_SCROLLWIN_TOP)           ywin = 0;
    else if (event.m_eventType == wxEVT_SCROLLWIN_BOTTOM)   ywin = pFontfile->NumGlyphs();
    else if (event.m_eventType == wxEVT_SCROLLWIN_LINEUP)   ywin--;
    else if (event.m_eventType == wxEVT_SCROLLWIN_LINEDOWN) ywin++;
    else if (event.m_eventType == wxEVT_SCROLLWIN_PAGEUP)   ywin -= GetScrollThumb(wxVERTICAL);
    else if (event.m_eventType == wxEVT_SCROLLWIN_PAGEDOWN) ywin += GetScrollThumb(wxVERTICAL);
    else                                                    ywin = event.GetPosition();

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

void CFontView::OnChangeBackgroundColor(wxCommandEvent& event)
{
        wxColour nColor;
        nColor=GetBackgroundColour();
        
        wxColourData hData;
        hData.SetColour(nColor);
        hData.SetChooseFull(false);

        for (int i=0; i<16; i++)
        {
            wxColour nCustom(i*16, i*16, i*16);
            hData.SetCustomColour(i,nCustom);
        }

        wxColourDialog cdialog(this, &hData);
        cdialog.SetTitle("Choose the background color");
        if (cdialog.ShowModal() == wxID_OK)
        {
            wxColourData retData = cdialog.GetColourData();
            nColor               = retData.GetColour();
            SetBackgroundColour(nColor);

            u8 r=nColor.Red();
            u8 g=nColor.Green();
            u8 b=nColor.Blue();

            glClearColor(r,g,b,0);
            Render();
            
        }
}