
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

        struct TilesetFrame : public GraphicsFrame {
            DECLARE_EVENT_TABLE()

            TilesetPanel* _parent;
        public:
            TilesetFrame(TilesetPanel* parent)
                : GraphicsFrame(parent)
                , _parent(parent) 
            {}

            void onPaint(wxPaintEvent& event) {
                wxPaintDC blah(this);

                _parent->render();
            }
        };

        BEGIN_EVENT_TABLE(TilesetFrame, GraphicsFrame)
            EVT_PAINT(TilesetFrame::onPaint)
        END_EVENT_TABLE();
        __declspec(noreturn) void NYI() { throw std::runtime_error("Not yet impemented!!"); }
    }

    BEGIN_EVENT_TABLE(TilesetPanel, DocumentPanel)
        EVT_SCROLLWIN(TilesetPanel::onScroll)
        EVT_SIZE(TilesetPanel::onSize)
        EVT_ERASE_BACKGROUND(TilesetPanel::onEraseBackground)

        EVT_LEFT_DOWN(TilesetPanel::onLeftClick)
        EVT_RIGHT_DOWN(TilesetPanel::onRightClick)
        EVT_MOUSEWHEEL(TilesetPanel::onMouseWheel)

        EVT_MENU(id_filesave, TilesetPanel::onSave)
        EVT_MENU(id_filesaveas, TilesetPanel::onSaveAs)
        EVT_MENU(id_importtiles, TilesetPanel::onImportTiles)

        EVT_MENU(id_cuttile, TilesetPanel::onCutTile)
        EVT_MENU(id_copytile, TilesetPanel::onCopyTile)
        EVT_MENU(id_inserttile, TilesetPanel::onInsertTile)
        EVT_MENU(id_pasteover, TilesetPanel::onPasteOver)
        EVT_MENU(id_insertandpaste, TilesetPanel::onInsertAndPaste)
        EVT_MENU(id_edittile, TilesetPanel::onEditTile)

        EVT_MENU(id_zoomnormal, TilesetPanel::onZoomNormal)
        EVT_MENU(id_zoomin, TilesetPanel::onZoomIn)
        EVT_MENU(id_zoomout, TilesetPanel::onZoomOut)

        EVT_MENU(id_zoomin2x, TilesetPanel::onZoomIn2x)
        EVT_MENU(id_zoomin4x, TilesetPanel::onZoomIn4x)
        EVT_MENU(id_zoomout2x, TilesetPanel::onZoomOut2x)
        EVT_MENU(id_zoomout4x, TilesetPanel::onZoomOut4x)
    END_EVENT_TABLE()

    TilesetPanel::TilesetPanel(MainWindow* parentwnd, const std::string& fname)
        : DocumentPanel(parentwnd, fname)
        , parent(parentwnd)
        , ywin(0)
    {
        Init();

        tileset = parent->vsp.get(fname);
    }

    TilesetPanel::TilesetPanel(MainWindow* parentwnd, int width, int height)
        : DocumentPanel(parentwnd, "")
        , parent(parentwnd)
        , ywin(0)
    {
        Init();

        tileset = new Tileset(width, height, 0);
    }

    TilesetPanel::~TilesetPanel() {
        delete contextMenu;

        parent->vsp.free(tileset);
        tileset = 0;
    }


    void TilesetPanel::init() {
        SetIcon(wxIcon("vspicon", wxBITMAP_TYPE_ICO_RESOURCE));

        graph = new TilesetFrame(this);
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

    void TilesetPanel::onSave(wxCommandEvent& event) {
        tileset->save(getName());
    }

    void TilesetPanel::onSaveAs(wxCommandEvent& event) {
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
            Tileset* clone = parent->vsp.clone(tileset);
            if (clone != tileset) {
                // TODO: revert tileset to last saved state
                wxASSERT_MSG(0, "NYI");
                tileset = clone;
            }
        }

        //name = dlg.GetPath().c_str();
        SetTitle(getName().c_str());

        onSave(event);
    }

    void TilesetPanel::onPaint() {
        if (!tileset) {
            return; // blech
        }
        render();
    }

    void TilesetPanel::onSize(wxSizeEvent& event) {
        graph->SetSize(GetClientSize());

        updateScrollbar();
    }

    void TilesetPanel::onScroll(wxScrollWinEvent& event) {
        if (event.m_eventType == wxEVT_SCROLLWIN_TOP) {
            ywin = 0;                     
        } else if (event.m_eventType == wxEVT_SCROLLWIN_BOTTOM) {
            // set ywin to a huge value: the usual scroll handler will clip it
            ywin = tileset->getCount();        
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

    void TilesetPanel::onLeftClick(wxMouseEvent& event) {
        int x, y;
        event.GetPosition(&x, &y);

        int t = getTileAt(x, y);
        if (t != -1) {
            tileset->setCurTile(t);
            render();
        }
    }

    void TilesetPanel::onRightClick(wxMouseEvent& event) {
        tileIndex = getTileAt(event.GetX(), event.GetY());
        PopupMenu(contextMenu, event.GetPosition());
    }

    void TilesetPanel::onMouseWheel(wxMouseEvent& event) {
        int t = tileset->getCurTile() + tileset->getCount();
        if (event.GetWheelRotation() > 0) {
            t++;
        } else {
            t--;
        }
        tileset->setCurTile(t % tileset->getCount());

        render();
    }

    //---------------------------

    void TilesetPanel::onImportTiles(wxCommandEvent&) {
        NYI();
        ImportFramesDlg dlg(this);

        if (dlg.ShowModal(tileset->getWidth(), tileset->getHeight()) != wxID_OK) {
            return;
        }

        if (!dlg.frames.size()) {
            return;
        }

        std::vector<Canvas>& tiles = dlg.frames;
        for (uint i = 0; i < tiles.size(); i++) {
            tileset->append(tiles[i]);
        }
    }

    void TilesetPanel::setZoomFactor(int factor) {
        const int winWidth = GetClientSize().GetWidth();
        const int maxZoom = (tileset->getWidth() * 16 / winWidth) + 1;

        int zoom = graph->Zoom() - factor;
        zoom = clamp(zoom, maxZoom, 255);
        
        graph->Zoom(zoom);

        render();
        graph->ShowPage();
        updateScrollbar();
    }

    void TilesetPanel::onCutTile(wxCommandEvent&) {
        NYI();
        Clipboard::Set(tileset->getCanvas(tileIndex));
        tileset->remove(tileIndex);
        render();
    }

    void TilesetPanel::onCopyTile(wxCommandEvent&) {
        Clipboard::Set(tileset->getCanvas(tileIndex));
    }

    void TilesetPanel::onInsertTile(wxCommandEvent&) {
        NYI();
        Canvas blank(tileset->getWidth(), tileset->getHeight());
        tileset->insert(blank, tileIndex);
        render();
    }

    void TilesetPanel::onPasteOver(wxCommandEvent&) {
        NYI();
        const Canvas& c = Clipboard::GetCanvas();

        if (c.Width() == tileset->getWidth() && c.Height() == tileset->getHeight()) {
            tileset->setCanvas(c, tileIndex);
        }
        render();
    }

    void TilesetPanel::onInsertAndPaste(wxCommandEvent&) {
        NYI();
        Canvas& c = const_cast<Canvas&>(Clipboard::GetCanvas());

        if (c.Width() != tileset->getWidth() || c.Height() != tileset->getHeight()) {
            wxArrayString choices;
            choices.Add("Crop");
            choices.Add("Scale");

            std::string choice = wxGetSingleChoice(
                va("The clipboard contains a %ix%i image, but the tileset is %ix%i.\n"
                "Choose an operation, or cancel to abort.", c.Width(), c.Height(), tileset->getWidth(), tileset->getHeight()),
                "Paste tile", choices, this, -1, -1, true);

            if (choice.empty()) {
                return;
            }

            Canvas d(tileset->getWidth(), tileset->getHeight());
            if (choice == "Crop") {
                CBlitter<Opaque>::Blit(c, d, 0, 0);
            } else if (choice == "Scale") {
                CBlitter<Opaque>::ScaleBlit(c, d, 0, 0, d.Width(), d.Height());
            }

            c = d;
        }

        tileset->insert(c, tileIndex);
        render();
    }

    void TilesetPanel::onEditTile(wxCommandEvent&) {
        NYI();
        //parent->OpenDocument( new CImageView(parent, &tileset->getCanvas(tileIndex)) );
    }

    void TilesetPanel::onZoomNormal(wxCommandEvent&)    {   setZoomFactor(graph->Zoom() - 16);  }
    void TilesetPanel::onZoomIn(wxCommandEvent&)        {   setZoomFactor( 1);   }
    void TilesetPanel::onZoomOut(wxCommandEvent&)       {   setZoomFactor(-1);   }
    void TilesetPanel::onZoomIn2x(wxCommandEvent&)      {   setZoomFactor( 2);   }
    void TilesetPanel::onZoomOut2x(wxCommandEvent&)     {   setZoomFactor(-2);   }
    void TilesetPanel::onZoomIn4x(wxCommandEvent&)      {   setZoomFactor( 4);   }
    void TilesetPanel::onZoomOut4x(wxCommandEvent&)     {   setZoomFactor(-4);   }
    //---------------------------

    void TilesetPanel::render() {
        const int w = graph->LogicalWidth();
        const int h = graph->LogicalHeight();

        const int tx = tileset->getWidth();
        const int ty = tileset->getHeight();

        const int tileWidth  = w / tx;
        const int tileHeight = (h / ty) + 1;

        graph->SetCurrent();
        graph->Clear();

        // first tile to draw
        int tileIndex = ywin * tileWidth;
        // x/y position to draw each tile at
        int x = 0;
        int y = 0;
        while (tileIndex < tileset->getCount() && x < w && y < h) {
            graph->Blit(tileset->getImage(tileIndex), x * tx, y * ty, false);

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
        getTilePos(tileset->getCurTile(), cx, cy);
        graph->Rect(cx - 1, cy - 1, tx + 1, ty + 1, RGBA(255, 255, 255));

        graph->ShowPage();
    }

    void TilesetPanel::updateScrollbar() {
        const int w = graph->LogicalWidth();
        const int h = graph->LogicalHeight();

        const int tileWidth  = max(w / tileset->getWidth(), 1);    
        const int tileHeight = h / tileset->getHeight();

        const int totalHeight = tileset->getCount() / tileWidth;

        ywin = clamp(0, totalHeight - tileHeight, ywin);

        SetScrollbar(wxVERTICAL, ywin, tileHeight, totalHeight, true);
    }

    int TilesetPanel::getTileAt(int x, int y) const {
        const int tx = tileset->getWidth();
        const int ty = tileset->getHeight();

        const int tileWidth = max(graph->LogicalWidth() / tx, 1);

        x /= tx;      y /= ty;

        int t = (y + ywin) * tileWidth + x;

        if (t <= tileset->getCount()) {
            return t;
        } else {
            return -1;
        }
    }

    // Returns the position at which the tile is drawn at
    void TilesetPanel::getTilePos(int tileidx, int& x, int& y) const {
        int tileWidth = max(graph->LogicalWidth() / tileset->getWidth(), 1);

        x = tileidx % tileWidth;
        y = tileidx / tileWidth - ywin;

        x *= tileset->getWidth();
        y *= tileset->getHeight();
    }

}
