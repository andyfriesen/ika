

// I'm not entirely happy with this interface, but meh.
// It's conveniant for now.
//  --khross

#include "wx/event.h"

#include "spritesetview.h"
#include "imageview.h"
#include "main.h"
#include "movescripteditor.h"

namespace
{
    enum
    {
        id_deleteframe=100,
        id_insertframe,
        id_copyframe,
        id_pasteinto,
        id_pasteover,
        id_insertandpaste,
        id_editframe,

        id_zoomin,
        id_zoomout,
        id_zoomnormal,

        // I was planning on a button or something
        // for these.
        id_prevframe,
        id_nextframe
    };

    enum
    {       
        id_filesave,
        id_filesaveas,
        id_fileclose,

        id_chrmovescript
    };

};


BEGIN_EVENT_TABLE(CSpriteSetView,IDocView)

    EVT_MENU(id_chrmovescript,CSpriteSetView::OnShowMovescriptEditor)
    EVT_MENU(id_editframe,CSpriteSetView::OnEditFrame)

    EVT_MENU(id_zoomin,CSpriteSetView::OnZoomIn)
    EVT_MENU(id_zoomout,CSpriteSetView::OnZoomOut)
    EVT_MENU(id_zoomnormal,CSpriteSetView::OnZoomNormal)

    EVT_MENU(id_filesave,CSpriteSetView::OnSave)
    EVT_MENU(id_filesaveas,CSpriteSetView::OnSaveAs)
    EVT_MENU(id_fileclose,CSpriteSetView::OnClose)
    
    EVT_SCROLLWIN(CSpriteSetView::OnScroll)
    EVT_SIZE(CSpriteSetView::OnSize)
    
    EVT_PAINT(CSpriteSetView::OnPaint)
    EVT_CLOSE(CSpriteSetView::OnClose)

    EVT_LEFT_DOWN(CSpriteSetView::OnLeftClick)
    EVT_RIGHT_DOWN(CSpriteSetView::OnRightClick)

END_EVENT_TABLE()





CSpriteSetView::CSpriteSetView(CMainWnd* parentwnd,const string& fname):
IDocView(parentwnd,fname),
pParent(parentwnd),
nCurframe(0),
ywin(0),
nZoom(16),
pSprite(0)
{    

    InitMenu();
    InitAccelerators();

    pSprite=pParent->spriteset.Load(fname);
    sSpritename=pSprite?fname:"Untitled.chr";

    pGraph=new CGraphFrame(this);
    pGraph->SetSize(GetClientSize());


    pContextmenu=new wxMenu();
    pContextmenu->Append(id_deleteframe,"Delete");
    pContextmenu->Append(id_insertframe,"Insert");
    pContextmenu->Append(id_copyframe,"Copy");
    pContextmenu->Append(id_pasteinto,"Paste into");
    pContextmenu->Append(id_pasteover,"Paste over");
    pContextmenu->Append(id_insertandpaste,"Insert and paste");
    pContextmenu->AppendSeparator();
    pContextmenu->Append(id_editframe,"Edit");

    
    
    
    
    pMovescripteditor=new CMovescriptEditor(this,pSprite);


    //Render();

    SetFocus();
    

}

CSpriteSetView::~CSpriteSetView()
{
    delete pContextmenu;
}

void CSpriteSetView::OnLeftClick(wxMouseEvent& event)
{    
    int x=event.GetPosition().x;
    int y=event.GetPosition().y;

    const int tx=pSprite->Width();
    const int ty=pSprite->Height();

    int nSpritewidth=GetClientSize().GetWidth()/tx;

    x/=tx;      
    y/=ty;

    int t=(y+ywin)*nSpritewidth+x;

    if (t>pSprite->Count()) t=0;
    
    nCurframe=t;

    Render();
}

    

void CSpriteSetView::InitMenu()
{
    wxMenuBar* menubar=pParent->CreateBasicMenu();

    wxMenu* filemenu=menubar->Remove(0);
    filemenu->Insert(2,new wxMenuItem(filemenu,id_filesave,"&Save","Save the sprite to disk."));
    filemenu->Insert(3,new wxMenuItem(filemenu,id_filesaveas,"Save &As","Save the sprite under a new filename."));
    filemenu->Insert(4,new wxMenuItem(filemenu,id_fileclose,"&Close","Close the sprite view."));
    menubar->Append(filemenu,"&File");


    wxMenu* chrmenu=new wxMenu;
    chrmenu->Append(id_chrmovescript,"&Movescript...");

    menubar->Append(chrmenu,"&CHR");

    SetMenuBar(menubar);
}

void CSpriteSetView::InitAccelerators()
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

void CSpriteSetView::OnRightClick(wxMouseEvent& event)
{
    PopupMenu(pContextmenu,event.GetPosition());
}

void CSpriteSetView::OnSave(wxCommandEvent& event)
{
    pSprite->Save(sSpritename.c_str());
}

void CSpriteSetView::OnSaveAs(wxCommandEvent& event)
{
    wxFileDialog dlg(
        this,
        "Save CHR",
        "",
        "",
        "CHR files (*.chr)|*.chr|"
        "All files (*.*)|*.*",
        wxSAVE | wxOVERWRITE_PROMPT
        );

    int result=dlg.ShowModal();
    if (result==wxID_CANCEL)
        return;

    sSpritename=dlg.GetFilename().c_str();
    SetTitle(sSpritename.c_str());

    OnSave(event);
}

void CSpriteSetView::OnClose(wxCommandEvent& event)
{
    pParent->spriteset.Release(pSprite);
    pSprite=0;

    Destroy();
}

const void* CSpriteSetView::GetResource() const
{
    return pSprite;
}

void CSpriteSetView::OnPaint()
{
    if (!pGraph || !pSprite)
        return;

    wxPaintDC dc(this);

    Render();
}

void CSpriteSetView::OnPreviousFrame(wxCommandEvent& event)
{
    if (nCurframe>0)
        nCurframe--;
    else
        nCurframe=pSprite->Count();

    Render();

}

void CSpriteSetView::OnNextFrame(wxCommandEvent& event)
{
    if (nCurframe<pSprite->Count())
        nCurframe++;
    else
        nCurframe=0;

    Render();

}

void CSpriteSetView::Zoom(int nZoomscale)
{
    int nZoom=pGraph->Zoom()-nZoomscale;

    if (nZoom<1) nZoom=1;
    if (nZoom>255) nZoom=255;

    pGraph->Zoom(nZoom);

    UpdateScrollbar();

    Render();      
}

void CSpriteSetView::OnEditFrame(wxCommandEvent& event)
{
    pParent->OpenDocument(new CImageView(pParent,&pSprite->Get(nCurframe)));
}

void CSpriteSetView::OnSize(wxSizeEvent& event)
{
    pGraph->SetSize(GetClientSize());

    UpdateScrollbar();
}

void CSpriteSetView::Render()
{
    if (!pSprite)
        return;

    int nWidth, nHeight;
    GetClientSize(&nWidth, &nHeight);


    int tx=pSprite->Width();
    int ty=pSprite->Height();


    int nSpritewidth=nWidth/tx;
    int nSpriteheight=(nHeight/ty)+1;
    int nSprite=ywin*nSpritewidth;


    pGraph->SetCurrent();
    pGraph->Clear();     


    for(int y=0; y<nSpriteheight; y++)
    {
        for(int x=0; x<nSpritewidth; x++)
        {
            
            Canvas& rBitmap=pSprite->Get(nSprite);

            CImage img(rBitmap);
            
            pGraph->Blit(img,x*tx,y*ty,true);
            //pGraph->Rect(x*tx-1,y*ty-1,rBitmap.Width()+1,rBitmap.Height()+1,RGBA(255,255,255));



            nSprite++;


            if (nSprite>=pSprite->Count()) 
            {
                y=nSpriteheight+1;
                break;
            }

            nSpritewidth=nWidth/tx;
            nSpriteheight=(nHeight/ty)+1;

        }
        
    }

    int x2,y2;
    tx=pSprite->Width();
    ty=pSprite->Height();
    SpritePos(nCurframe,x2,y2);

    pGraph->Rect(x2-1,y2-1,tx+1,ty+1,RGBA(255,255,255));

    pGraph->ShowPage();

}

void CSpriteSetView::UpdateScrollbar()
{
    int w,h;
    GetClientSize(&w,&h);

    int nSpritewidth=w/pSprite->Width();
    int nSpriteheight=h/pSprite->Height();

    int nTotalheight=pSprite->Count()/nSpritewidth;

    if (ywin>nTotalheight-nSpriteheight)  ywin=nTotalheight-nSpriteheight;
    if (ywin<0)                           ywin=0;

    SetScrollbar(wxVERTICAL,ywin,nSpriteheight,nTotalheight,true);
}


void CSpriteSetView::OnScroll(wxScrollWinEvent& event)
{
    if (event.m_eventType == wxEVT_SCROLLWIN_TOP)           ywin=0;
    else if (event.m_eventType == wxEVT_SCROLLWIN_BOTTOM)   ywin=pSprite->Count();
    else if (event.m_eventType == wxEVT_SCROLLWIN_LINEUP)   ywin--;
    else if (event.m_eventType == wxEVT_SCROLLWIN_LINEDOWN) ywin++;
    else if (event.m_eventType == wxEVT_SCROLLWIN_PAGEUP)   ywin-=GetScrollThumb(wxVERTICAL);
    else if (event.m_eventType == wxEVT_SCROLLWIN_PAGEDOWN) ywin+=GetScrollThumb(wxVERTICAL);
    else                                                    ywin=event.GetPosition();

    UpdateScrollbar();
    Render();
}

void CSpriteSetView::SpritePos(int idx,int& x,int& y) const
{
    int w=GetClientSize().GetWidth()/pSprite->Width();
    
    x=idx%w;
    y=idx/w-ywin;

    x*=pSprite->Width();
    y*=pSprite->Height();
}

void CSpriteSetView::OnShowMovescriptEditor(wxCommandEvent& event)
{
    pMovescripteditor->Show(true);

}

void CSpriteSetView::OnZoomIn(wxCommandEvent& event)    { Zoom(1);  }
void CSpriteSetView::OnZoomOut(wxCommandEvent& event)   { Zoom(-1); }
void CSpriteSetView::OnZoomNormal(wxCommandEvent& event){ Zoom(16-nZoom); } 
