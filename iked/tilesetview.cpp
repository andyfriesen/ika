
#include "main.h"
#include "tilesetview.h"
#include "imageview.h"
#include "importframesdlg.h"
#include "clipboard.h"

#include "wx/event.h"

namespace iked {
    namespace {
        enum {
            id_filler = 100,

            id_filesave,
            id_filesaveas,
            id_fileclose,
            id_importtiles,

            id_cuttile,
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

        struct TileSetFrame : public GraphicsFrame {
            DECLARE_EVENT_TABLE()

            TileSetPanel* _parent;
        public:
            TileSetFrame(TileSetPanel* parent)
                : GraphicsFrame(parent)
                , _parent(parent) 
            {}

            void onPaint(wxPaintEvent& event) {
                wxPaintDC blah(this);

                _parent->render();
            }
        };

        BEGIN_EVENT_TABLE(TileSetFrame, GraphicsFrame)
            EVT_PAINT(TileSetFrame::onPaint)
        END_EVENT_TABLE();
        __declspec(noreturn) void NYI() { throw std::runtime_error("Not yet impemented!!"); }
    }

    BEGIN_EVENT_TABLE(TileSetPanel, DocumentPanel)
        EVT_SCROLLWIN(TileSetPanel::onScroll)
        EVT_SIZE(TileSetPanel::onSize)
        EVT_ERASE_BACKGROUND(TileSetPanel::onEraseBackground)

        EVT_LEFT_DOWN(TileSetPanel::onLeftClick)
        EVT_RIGHT_DOWN(TileSetPanel::onRightClick)
        EVT_MOUSEWHEEL(TileSetPanel::onMouseWheel)

        EVT_MENU(id_filesave, TileSetPanel::onSave)
        EVT_MENU(id_filesaveas, TileSetPanel::onSaveAs)
        EVT_MENU(id_importtiles, TileSetPanel::onImportTiles)

        EVT_MENU(id_cuttile, TileSetPanel::onCutTile)
        EVT_MENU(id_copytile, TileSetPanel::onCopyTile)
        EVT_MENU(id_inserttile, TileSetPanel::onInsertTile)
        EVT_MENU(id_pasteover, TileSetPanel::onPasteOver)
        EVT_MENU(id_insertandpaste, TileSetPanel::onInsertAndPaste)
        EVT_MENU(id_edittile, TileSetPanel::onEditTile)

        EVT_MENU(id_zoomnormal, TileSetPanel::onZoomNormal)
        EVT_MENU(id_zoomin, TileSetPanel::onZoomIn)
        EVT_MENU(id_zoomout, TileSetPanel::onZoomOut)

        EVT_MENU(id_zoomin2x, TileSetPanel::onZoomIn2x)
        EVT_MENU(id_zoomin4x, TileSetPanel::onZoomIn4x)
        EVT_MENU(id_zoomout2x, TileSetPanel::onZoomOut2x)
        EVT_MENU(id_zoomout4x, TileSetPanel::onZoomOut4x)
    END_EVENT_TABLE()

    TileSetPanel::TileSetPanel(MainWindow* parentwnd, const std::string& fname)
        : DocumentPanel(parentwnd, fname)
        , parent(parentwnd)
        , ywin(0)
    {
        Init();

        tileSet = parent->vsp.get(fname);
    }

    TileSetPanel::TileSetPanel(MainWindow* parentwnd, int width, int height)
        : DocumentPanel(parentwnd, "")
        , parent(parentwnd)
        , ywin(0)
    {
        Init();

        tileSet = new TileSet(width, height, 0);
    }

    TileSetPanel::~TileSetPanel() {
        delete contextMenu;

        parent->vsp.free(tileSet);
        tileSet = 0;
    }


    void TileSetPanel::init() {
        SetIcon(wxIcon("vspicon", wxBITMAP_TYPE_ICO_RESOURCE));

        graph = new TileSetFrame(this);
        graph->SetSize(GetClientSize());
        wxMenuBar* menubar = parent->CreateBasicMenu();

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

        contextMenu = new wxMenu();
        contextMenu->Append(id_inserttile, "Insert");
        contextMenu->Append(id_cuttile, "Cut");
        contextMenu->Append(id_copytile, "Copy");
        //contextMenu->Append(id_pasteinto, "Paste into");
        contextMenu->Append(id_pasteover, "Paste over");
        contextMenu->Append(id_insertandpaste, "Insert and paste");
        //contextMenu->AppendSeparator();
        //contextMenu->Append(id_edittile, "Edit");
    }

    // --------------------------------- events ---------------------------------

    void TileSetPanel::onSave(wxCommandEvent& event) {
        tileSet->save(getName());
    }

    void TileSetPanel::onSaveAs(wxCommandEvent& event) {
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
        if (result == wxID_CANCEL) {
            return;
        }

        std::string oldName = getName();
        std::string newName = dlg.GetPath().c_str();

        if (oldName != newName) {
            TileSet* clone = parent->vsp.clone(tileSet);
            if (clone != tileSet) {
                // TODO: revert tileSet to last saved state
                wxASSERT_MSG(0, "NYI");
                tileSet = clone;
            }
        }

        //name = dlg.GetPath().c_str();
        SetTitle(getName().c_str());

        onSave(event);
    }

    void TileSetPanel::onPaint() {
        if (!tileSet) {
            return; // blech
        }
        render();
    }

    void TileSetPanel::onSize(wxSizeEvent& event) {
        graph->SetSize(GetClientSize());

        updateScrollbar();
    }

    void TileSetPanel::onScroll(wxScrollWinEvent& event) {
        if (event.m_eventType == wxEVT_SCROLLWIN_TOP) {
            ywin = 0;                     
        } else if (event.m_eventType == wxEVT_SCROLLWIN_BOTTOM) {
            // set ywin to a huge value: the usual scroll handler will clip it
            ywin = tileSet->getCount();        
        } else if (event.m_eventType == wxEVT_SCROLLWIN_LINEUP) {
            ywin--;                     
        } else if (event.m_eventType == wxEVT_SCROLLWIN_LINEDOWN) {
            ywin++;                     
        } else if (event.m_eventType == wxEVT_SCROLLWIN_PAGEUP) {
            ywin -= GetScrollThumb(wxVERTICAL);   
        } else if (event.m_eventType == wxEVT_SCROLLWIN_PAGEDOWN) {
            ywin += GetScrollThumb(wxVERTICAL);   
        } else {
            ywin = event.GetPosition();

            updateScrollbar();
            render();
        }
    }

    void TileSetPanel::onLeftClick(wxMouseEvent& event) {
        int x, y;
        event.GetPosition(&x, &y);

        int t = getTileAt(x, y);
        if (t != -1) {
            tileSet->setCurTile(t);
            render();
        }
    }

    void TileSetPanel::onRightClick(wxMouseEvent& event) {
        tileIndex = getTileAt(event.GetX(), event.GetY());
        PopupMenu(contextMenu, event.GetPosition());
    }

    void TileSetPanel::onMouseWheel(wxMouseEvent& event) {
        int t = tileSet->getCurTile() + tileSet->getCount();
        if (event.GetWheelRotation() > 0) {
            t++;
        } else {
            t--;
        }
        tileSet->setCurTile(t % tileSet->getCount());

        render();
    }

    //---------------------------

    void TileSetPanel::onImportTiles(wxCommandEvent&) {
        NYI();
        ImportFramesDlg dlg(this);

        if (dlg.ShowModal(tileSet->getWidth(), tileSet->getHeight()) != wxID_OK) {
            return;
        }

        if (!dlg.frames.size()) {
            return;
        }

        std::vector<Canvas>& tiles = dlg.frames;
        for (uint i = 0; i < tiles.size(); i++) {
            tileSet->append(tiles[i]);
        }
    }

    void TileSetPanel::setZoomFactor(int factor) {
        const int winWidth = GetClientSize().GetWidth();
        const int maxZoom = (tileSet->getWidth() * 16 / winWidth) + 1;

        int zoom = graph->Zoom() - factor;
        zoom = clamp(zoom, maxZoom, 255);
        
        graph->Zoom(zoom);

        render();
        graph->ShowPage();
        updateScrollbar();
    }

    void TileSetPanel::onCutTile(wxCommandEvent&) {
        NYI();
        Clipboard::Set(tileSet->getCanvas(tileIndex));
        tileSet->remove(tileIndex);
        render();
    }

    void TileSetPanel::onCopyTile(wxCommandEvent&) {
        Clipboard::Set(tileSet->getCanvas(tileIndex));
    }

    void TileSetPanel::onInsertTile(wxCommandEvent&) {
        NYI();
        Canvas blank(tileSet->getWidth(), tileSet->getHeight());
        tileSet->insert(blank, tileIndex);
        render();
    }

    void TileSetPanel::onPasteOver(wxCommandEvent&) {
        NYI();
        const Canvas& c = Clipboard::GetCanvas();

        if (c.Width() == tileSet->getWidth() && c.Height() == tileSet->getHeight()) {
            tileSet->setCanvas(c, tileIndex);
        }
        render();
    }

    void TileSetPanel::onInsertAndPaste(wxCommandEvent&) {
        NYI();
        Canvas& c = const_cast<Canvas&>(Clipboard::GetCanvas());

        if (c.Width() != tileSet->getWidth() || c.Height() != tileSet->getHeight()) {
            wxArrayString choices;
            choices.Add("Crop");
            choices.Add("Scale");

            std::string choice = wxGetSingleChoice(
                va("The clipboard contains a %ix%i image, but the tileSet is %ix%i.\n"
                "Choose an operation, or cancel to abort.", c.Width(), c.Height(), tileSet->getWidth(), tileSet->getHeight()),
                "Paste tile", choices, this, -1, -1, true);

            if (choice.empty()) {
                return;
            }

            Canvas d(tileSet->getWidth(), tileSet->getHeight());
            if (choice == "Crop") {
                CBlitter<Opaque>::Blit(c, d, 0, 0);
            } else if (choice == "Scale") {
                CBlitter<Opaque>::ScaleBlit(c, d, 0, 0, d.Width(), d.Height());
            }

            c = d;
        }

        tileSet->insert(c, tileIndex);
        render();
    }

    void TileSetPanel::onEditTile(wxCommandEvent&) {
        NYI();
        //parent->OpenDocument( new CImageView(parent, &tileSet->getCanvas(tileIndex)) );
    }

    void TileSetPanel::onZoomNormal(wxCommandEvent&)    {   setZoomFactor(graph->Zoom() - 16);  }
    void TileSetPanel::onZoomIn(wxCommandEvent&)        {   setZoomFactor( 1);   }
    void TileSetPanel::onZoomOut(wxCommandEvent&)       {   setZoomFactor(-1);   }
    void TileSetPanel::onZoomIn2x(wxCommandEvent&)      {   setZoomFactor( 2);   }
    void TileSetPanel::onZoomOut2x(wxCommandEvent&)     {   setZoomFactor(-2);   }
    void TileSetPanel::onZoomIn4x(wxCommandEvent&)      {   setZoomFactor( 4);   }
    void TileSetPanel::onZoomOut4x(wxCommandEvent&)     {   setZoomFactor(-4);   }
    //---------------------------

    void TileSetPanel::render() {
        const int w = graph->LogicalWidth();
        const int h = graph->LogicalHeight();

        const int tx = tileSet->getWidth();
        const int ty = tileSet->getHeight();

        const int tileWidth  = w / tx;
        const int tileHeight = (h / ty) + 1;

        graph->SetCurrent();
        graph->Clear();

        // first tile to draw
        int tileIndex = ywin * tileWidth;
        // x/y position to draw each tile at
        int x = 0;
        int y = 0;
        while (tileIndex < tileSet->getCount() && x < w && y < h) {
            graph->Blit(tileSet->getImage(tileIndex), x * tx, y * ty, false);

            tileIndex++;
            x++;
            if (x > tileWidth) {
                x = 0;
                y++;
                if (y > tileHeight) {
                    break;
                }
            }
        }

        int cx, cy;
        getTilePos(tileSet->getCurTile(), cx, cy);
        graph->Rect(cx - 1, cy - 1, tx + 1, ty + 1, RGBA(255, 255, 255));

        graph->ShowPage();
    }

    void TileSetPanel::updateScrollbar() {
        const int w = graph->LogicalWidth();
        const int h = graph->LogicalHeight();

        const int tileWidth  = max(w / tileSet->getWidth(), 1);    
        const int tileHeight = h / tileSet->getHeight();

        const int totalHeight = tileSet->getCount() / tileWidth;

        ywin = clamp(0, totalHeight - tileHeight, ywin);

        SetScrollbar(wxVERTICAL, ywin, tileHeight, totalHeight, true);
    }

    int TileSetPanel::getTileAt(int x, int y) const {
        const int tx = tileSet->getWidth();
        const int ty = tileSet->getHeight();

        const int tileWidth = max(graph->LogicalWidth() / tx, 1);

        x /= tx;      y /= ty;

        int t = (y + ywin) * tileWidth + x;

        if (t <= tileSet->getCount()) {
            return t;
        } else {
            return -1;
        }
    }

    // Returns the position at which the tile is drawn at
    void TileSetPanel::getTilePos(int tileidx, int& x, int& y) const {
        int tileWidth = max(graph->LogicalWidth() / tileSet->getWidth(), 1);

        x = tileidx % tileWidth;
        y = tileidx / tileWidth - ywin;

        x *= tileSet->getWidth();
        y *= tileSet->getHeight();
    }

}
