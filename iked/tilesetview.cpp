
#include "main.h"
#include "tilesetview.h"
#include "imageview.h"
#include "importframesdlg.h"

#include "wx/event.h"

namespace
{
    enum
    {
        id_filler = 100,

        id_filesave,
        id_filesaveas,
        id_fileclose,
        id_importtiles,

        id_deletetile,
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
        
        CTileSetView* _parent;
    public:
        CTileSetFrame(CTileSetView* parent)
            : CGraphFrame(parent)
            , _parent(parent)
        {}

        void OnPaint(wxPaintEvent& event)
        {
            wxPaintDC blah(this);

            _parent->Paint();
        }
    };

    BEGIN_EVENT_TABLE(CTileSetFrame, CGraphFrame)
        EVT_PAINT(CTileSetFrame::OnPaint)
    END_EVENT_TABLE();
};

BEGIN_EVENT_TABLE(CTileSetView, IDocView)
    EVT_SCROLLWIN(CTileSetView::OnScroll)
    EVT_SIZE(CTileSetView::OnSize)
    EVT_CLOSE(CTileSetView::OnClose)
    EVT_ERASE_BACKGROUND(CTileSetView::OnEraseBackground)

    EVT_LEFT_DOWN(CTileSetView::OnLeftClick)
    EVT_RIGHT_DOWN(CTileSetView::OnRightClick)
    EVT_MOUSEWHEEL(CTileSetView::OnMouseWheel)

    EVT_MENU(id_filesave, CTileSetView::OnSave)
    EVT_MENU(id_filesaveas, CTileSetView::OnSaveAs)
    EVT_MENU(id_importtiles, CTileSetView::OnImportTiles)

    EVT_MENU(id_edittile, CTileSetView::OnEditTile)

    EVT_MENU(id_zoomnormal, CTileSetView::OnZoomNormal)
    EVT_MENU(id_zoomin, CTileSetView::OnZoomIn)
    EVT_MENU(id_zoomout, CTileSetView::OnZoomOut)

    EVT_MENU(id_zoomin2x, CTileSetView::OnZoomIn2x)
    EVT_MENU(id_zoomin4x, CTileSetView::OnZoomIn4x)
    EVT_MENU(id_zoomout2x, CTileSetView::OnZoomOut2x)
    EVT_MENU(id_zoomout4x, CTileSetView::OnZoomOut4x)

END_EVENT_TABLE()

CTileSetView::CTileSetView(CMainWnd* parentwnd, const string& fname)
    : IDocView(parentwnd, fname)
    , pParent(parentwnd)
    , ywin(0)
    , zoom(16)
{
    Init();

    pTileset = pParent->vsp.Load(fname);
}

CTileSetView::CTileSetView(CMainWnd* parentwnd, int width, int height)
    : IDocView(parentwnd, "")
    , pParent(parentwnd)
    , ywin(0)
    , zoom(16)
{
    Init();

    pTileset = new CTileSet();
    pTileset->New(width, height);
}

CTileSetView::~CTileSetView()
{
    delete pContextmenu;

    pParent->vsp.Release(pTileset);
    pTileset = 0;
}


void CTileSetView::Init()
{
    pGraph = new CTileSetFrame(this);
    pGraph->SetSize(GetClientSize());
    wxMenuBar* menubar = pParent->CreateBasicMenu();

    wxMenu* filemenu = menubar->Remove(0);
    filemenu->InsertSeparator(2);
    filemenu->Insert(3, new wxMenuItem(filemenu, id_filesave, "&Save", "Save the tileset to disk."));
    filemenu->Insert(4, new wxMenuItem(filemenu, id_filesaveas, "&Save As", "Save the tileset to disk under a new name."));
    filemenu->Insert(5, new wxMenuItem(filemenu, id_fileclose, "&Close", "Close the tileset window."));
    menubar->Append(filemenu, "&File");

    wxMenu* tilesetmenu = new wxMenu;
    tilesetmenu->Append(id_importtiles, "Import from image...", "");
    menubar->Append(tilesetmenu, "Tileset");

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

    pContextmenu = new wxMenu();
    pContextmenu->Append(id_deletetile, "Delete");
    pContextmenu->Append(id_inserttile, "Insert");
    pContextmenu->Append(id_copytile, "Copy");
    pContextmenu->Append(id_pasteinto, "Paste into");
    pContextmenu->Append(id_pasteover, "Paste over");
    pContextmenu->Append(id_insertandpaste, "Insert and paste");
    pContextmenu->AppendSeparator();
    pContextmenu->Append(id_edittile, "Edit");
}

// --------------------------------- events ---------------------------------

void CTileSetView::OnSave(wxCommandEvent& event)
{
    pTileset->Save(name.c_str());
}

void CTileSetView::OnSaveAs(wxCommandEvent& event)
{
    wxFileDialog dlg(
        this,
        "Save CHR",
        "",
        "",
        "Tilesets (*.vsp)|*.vsp|"
        "All files (*.*)|*.*",
        wxSAVE | wxOVERWRITE_PROMPT
        );

    int result = dlg.ShowModal();
    if (result==wxID_CANCEL)
        return;

    std::string oldname(name);

    name = dlg.GetPath().c_str();
    SetTitle(name.c_str());

    OnSave(event);

    if (!Path::Compare(name, oldname))
    {
        pTileset->Load(oldname.c_str());
        if (!pParent->vsp.Release(pTileset))
            delete pTileset;
        pTileset = pParent->vsp.Load(name.c_str());
    }
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
    if (event.m_eventType == wxEVT_SCROLLWIN_TOP)           ywin = 0;                     
    else if (event.m_eventType == wxEVT_SCROLLWIN_BOTTOM)   ywin = pTileset->Count();        // guaranteed to be too big, so that the usual scroll handler will clip it
    else if (event.m_eventType == wxEVT_SCROLLWIN_LINEUP)   ywin--;                     
    else if (event.m_eventType == wxEVT_SCROLLWIN_LINEDOWN) ywin++;                     
    else if (event.m_eventType == wxEVT_SCROLLWIN_PAGEUP)   ywin -= GetScrollThumb(wxVERTICAL);   
    else if (event.m_eventType == wxEVT_SCROLLWIN_PAGEDOWN) ywin += GetScrollThumb(wxVERTICAL);   
    else                                                    ywin = event.GetPosition();

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
    int x, y;
    event.GetPosition(&x, &y);

    pTileset->SetCurTile(TileAt(x, y));
    Render();
}

void CTileSetView::OnRightClick(wxMouseEvent& event)
{
    PopupMenu(pContextmenu, event.GetPosition());
    nTile = TileAt(event.GetX(), event.GetY());
}

void CTileSetView::OnMouseWheel(wxMouseEvent& event)
{
    int t = pTileset->CurTile() + pTileset->Count();
    if (event.GetWheelRotation() > 0)   t++;
    else                                t--;
    pTileset->SetCurTile(t % pTileset->Count());

    Render();
}

//---------------------------

void CTileSetView::OnImportTiles(wxCommandEvent&)
{
    ImportFramesDlg dlg(this);

    if (dlg.ShowModal(pTileset->Width(), pTileset->Height()) != wxID_OK)
        return;

    if (!dlg.frames.size())
        return;

    std::vector<Canvas>& tiles = dlg.frames;
    for (int i = 0; i < tiles.size(); i++)
        pTileset->AppendTile(tiles[i]);
}

void CTileSetView::Zoom(int factor)
{
    zoom -= factor;
    if (zoom < 1) zoom = 1;
    if (zoom > 255) zoom = 255;

    Render();
    pGraph->ShowPage();
    UpdateScrollbar();
}

void CTileSetView::OnEditTile(wxCommandEvent&)
{
    pParent->OpenDocument( new CImageView(pParent, &pTileset->Get(nTile)) );
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
    int w, h;
    GetClientSize(&w, &h);

    const int tx = pTileset->Width() * 16 / zoom;
    const int ty = pTileset->Height() * 16 / zoom;

    int nTilewidth =w / tx;
    int nTileheight=(h / ty)+1;

    int nTile = ywin * nTilewidth;                  // first tile to draw

    pGraph->SetCurrent();
    pGraph->Clear();

    for (int y = 0; y < nTileheight; y++)
    {
        for (int x = 0; x < nTilewidth; x++)
        {
            if (nTile >= pTileset->Count())
                goto breakloop;

            pGraph->ScaleBlit(pTileset->GetImage(nTile),
                x * tx, y * ty,
                tx, ty,
                false);

            nTile++;
        }
    }
breakloop:

    int cx, cy;
    TilePos(pTileset->CurTile(), cx, cy);
    pGraph->Rect(cx - 1, cy - 1, tx + 1, ty + 1, RGBA(255, 255, 255));

    pGraph->ShowPage();
}

void CTileSetView::UpdateScrollbar()
{
    int w, h;
    GetClientSize(&w, &h);

    int nTilewidth  = w / pTileset->Width() * zoom / 16;
    int nTileheight = h / pTileset->Height() * zoom / 16;

    int nTotalheight = pTileset->Count() / nTilewidth;

    if (ywin > nTotalheight - nTileheight)  ywin = nTotalheight - nTileheight;
    if (ywin < 0)                           ywin = 0;

    SetScrollbar(wxVERTICAL, ywin, nTileheight, nTotalheight, true);
}

int CTileSetView::TileAt(int x, int y) const
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
void CTileSetView::TilePos(int tileidx, int& x, int& y) const
{
    int nTilewidth = GetClientSize().GetWidth() / (pTileset->Width() * 16 / zoom);

    x = tileidx % nTilewidth;
    y = tileidx / nTilewidth - ywin;

    x *= pTileset->Width() * 16 / zoom;
    y *= pTileset->Height() * 16 / zoom;
}