
#include <wx/event.h>
#include <wx/xrc/xmlres.h>

#include "document.h"
#include "imageview.h"
#include "importframesdlg.h"
#include "main.h"
#include "movescripteditor.h"
#include "spritesetview.h"
#include "wx_events.h"

namespace iked {

    namespace {
        enum {
            id_filler = 100,

            id_filesave,
            id_filesaveas,
            id_fileclose,

            id_chrmovescript,
            id_chrimportframes,

            id_deleteframe,
            id_insertframe,
            id_copyframe,
            id_pasteinto,
            id_pasteover,
            id_insertandpaste,
            id_editframe,

            id_zoomin,
            id_zoomout,
            id_zoomnormal
        };

    };


    BEGIN_EVENT_TABLE(SpriteSetView, DocumentPanel)

        EVT_MENU(id_chrmovescript, SpriteSetView::onShowMovescriptEditor)
        EVT_MENU(id_chrimportframes, SpriteSetView::onImportFrames)
        //EVT_MENU(id_editframe, SpriteSetView::onEditFrame)

        EVT_MENU(id_zoomin, SpriteSetView::onZoomIn)
        EVT_MENU(id_zoomout, SpriteSetView::onZoomOut)
        EVT_MENU(id_zoomnormal, SpriteSetView::onZoomNormal)

        EVT_MENU(id_filesave, SpriteSetView::onSave)
        EVT_MENU(id_filesaveas, SpriteSetView::onSaveAs)
        EVT_MENU(id_fileclose, SpriteSetView::onClose)
        
        EVT_COMMAND(-1, EVT_IMAGE_RIGHT_CLICK, SpriteSetView::onRightClickFrame)

        //EVT_PAINT(SpriteSetView::onPaint)
        EVT_ERASE_BACKGROUND(SpriteSetView::OnEraseBackground)

    END_EVENT_TABLE()

    SpriteSetView::SpriteSetView(MainWindow* parentwnd, Document* doc, const std::string& fileName)
        : DocumentPanel(parentwnd, doc, fileName)
    {
        wxASSERT(doc && doc->asSpriteSet() != 0);
        Freeze();
        mainPanel = wxXmlResource::Get()->LoadPanel(this, "panel_spriteset");

        init();
        Thaw();
        Refresh();
    }

    SpriteSetView::~SpriteSetView() {
        delete contextMenu;
    }

    void SpriteSetView::onSave(wxCommandEvent& event) {
        getDocument()->save(getName());
    }

    void SpriteSetView::onSaveAs(wxCommandEvent& event) {
        wxFileDialog dlg(
            this,
            "Save CHR",
            "",
            "",
            "ika-sprite files (*.ika-sprite)|*.ika-sprite|"
            "CHR files (*.chr)|*.chr|"
            "All files (*.*)|*.*",
            wxSAVE | wxOVERWRITE_PROMPT
            );

        int result = dlg.ShowModal();
        if (result == wxID_OK) {
            std::string name = dlg.GetPath().c_str();
            saveDocument(name);
        }
    }

    void SpriteSetView::onRightClickFrame(wxCommandEvent& event) {
        int frameIndex = event.GetInt();
        int x, y;

        imagePanel->getImagePos(frameIndex, &x, &y);

        PopupMenu(contextMenu, wxPoint(x, y));
    }

    void SpriteSetView::onEditFrame(wxCommandEvent& event) {
        throw std::runtime_error("NYI");
        //_parent->OpenDocument(new CImageView(_parent, &_sprite->Get(_curFrame)));
    }

    void SpriteSetView::onZoomIn(wxCommandEvent& event)    { imagePanel->Zoom(1);  }
    void SpriteSetView::onZoomOut(wxCommandEvent& event)   { imagePanel->Zoom(-1); }
    void SpriteSetView::onZoomNormal(wxCommandEvent& event){ imagePanel->Zoom(16 - imagePanel->Zoom()); } 
    void SpriteSetView::onPaint(wxPaintEvent& event) {
        wxPaintDC paintDc(this);
        imagePanel->Refresh();
    }

    void SpriteSetView::onShowMovescriptEditor(wxCommandEvent& event) {
#if 1
        throw std::runtime_error("Not yet implemented");
#else
        moveScriptEditor->Show(true);
        moveScriptEditor->UpdateDlg();
#endif
    }

    void SpriteSetView::onImportFrames(wxCommandEvent& event) {
#if 1
        throw std::runtime_error("Not yet implemented");
#else
        ImportFramesDlg dlg(this);
        SpriteSet* sprite = getSprite();

        if (dlg.ShowModal() != wxID_OK)
            return;

        std::vector<Canvas>& frames = dlg.frames;
        if (frames.size() == 0)
            return;

        // this is horribly inefficient
        if (!dlg.append) {
            while (sprite->getCount()) {
                sprite->remove(0);
            }
        }

        if (frames[0].Width() != _sprite->Width() || frames[0].Height() != _sprite->Height()) {
            if (!dlg.append) {
                _sprite->Resize(frames[0].Width(), frames[0].Height());
            } else {
                // TODO: offer to crop or scale or some other things.
                wxMessageBox("The frames aren't the same size as the sprite!", "Error", wxOK | wxCENTER, this);
                return;
            }
        }

        for (uint i = 0; i < frames.size(); i++) {
            _sprite->append(frames[i]);
        }
#endif
    }

    void SpriteSetView::init() {
        animScriptGrid = new wxGrid(mainPanel, -1);
        animScriptGrid->SetRowLabelSize(0);
        animScriptGrid->SetColLabelSize(0);
        animScriptGrid->CreateGrid(8, 2);
        animScriptGrid->EnableGridLines(false);

        metaDataGrid = new wxGrid(mainPanel, -1);
        metaDataGrid->SetRowLabelSize(0);
        metaDataGrid->SetColLabelSize(0);
        metaDataGrid->CreateGrid(8, 2);
        metaDataGrid->EnableGridLines(false);

        imagePanel = new ImageArrayPanel(mainPanel, getSprite());
        imagePanel->SetSize(500, 500);

        wxXmlResource::Get()->AttachUnknownControl("unknown_animscript", animScriptGrid);
        wxXmlResource::Get()->AttachUnknownControl("unknown_metadata", metaDataGrid);
        wxXmlResource::Get()->AttachUnknownControl("unknown_frames", imagePanel);

        mainPanel->GetSizer()->FitInside(this);

        initMenu();
        initAccelerators();

#if 0
        wxSizer* sizer = new wxBoxSizer(wxVERTICAL);
        sizer->Add(imagePanel, 1, wxEXPAND);
        SetSizer(sizer);
        sizer->Fit(this);

        moveScriptEditor = new MoveScriptEditor(this, _sprite);

        SetFocus();
#endif
    }

    void SpriteSetView::initMenu() {
        wxMenuBar* menubar = getParent()->CreateBasicMenu();

        wxMenu* filemenu = menubar->Remove(0);
        filemenu->InsertSeparator(2);
        filemenu->Insert(3, new wxMenuItem(filemenu, id_filesave, "&Save", "Save the sprite to disk."));
        filemenu->Insert(4, new wxMenuItem(filemenu, id_filesaveas, "Save &As", "Save the sprite under a new filename."));
        filemenu->Insert(5, new wxMenuItem(filemenu, id_fileclose, "&Close", "Close the sprite view."));
        menubar->Append(filemenu, "&File");

        wxMenu* chrmenu = new wxMenu;
        chrmenu->Append(id_chrmovescript, "&Movescript...");
        chrmenu->Append(id_chrimportframes, "Import &Frames...");
        menubar->Append(chrmenu, "&CHR");

        SetMenuBar(menubar);

        // Context menu
        contextMenu = new wxMenu();
        contextMenu->Append(id_deleteframe, "Delete");
        contextMenu->Append(id_insertframe, "Insert");
        contextMenu->Append(id_copyframe, "Copy");
        contextMenu->Append(id_pasteinto, "Paste into");
        contextMenu->Append(id_pasteover, "Paste over");
        contextMenu->Append(id_insertandpaste, "Insert and paste");
        // Until the pixel editor is up and running
        //contextMenu->AppendSeparator();
        //contextMenu->Append(id_editframe, "Edit");
    }

    void SpriteSetView::initAccelerators() {
        std::vector<wxAcceleratorEntry> accel = getParent()->CreateBasicAcceleratorTable();

        int p = accel.size();
        accel.resize(accel.size()+4);

        accel[p++].Set(wxACCEL_CTRL, (int)'S', id_filesave);
        accel[p++].Set(0, (int)'+', id_zoomin);
        accel[p++].Set(0, (int)'-', id_zoomout);
        accel[p++].Set(0, (int)'=', id_zoomnormal);

        wxAcceleratorTable table(p, &*accel.begin());
        SetAcceleratorTable(table);
    }

    SpriteSet* SpriteSetView::getSprite() {
        // error checking?
        return static_cast<SpriteSet*>(getDocument()->asSpriteSet());
    }
}
