
#include <wx/event.h>

#include "document.h"
#include "imageview.h"
#include "importframesdlg.h"
#include "main.h"
#include "movescripteditor.h"
#include "spritesetview.h"
#include "common/chr.h"

namespace iked {

    namespace {
        enum {
            id_filler = 100,

            id_filesave,
            id_filesaveas,
            id_fileclose,

            id_chrmovescript,
            id_chrimportframes,

            id_newanimscript,
            id_destroyanimscript,

            id_newmetadata,
            id_destroymetadata,

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

    }


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
        
    END_EVENT_TABLE()

    SpriteSetView::SpriteSetView(MainWindow* parentwnd, Document* doc, const std::string& fileName)
        : DocumentPanel(parentwnd, doc, fileName)
    {
        wxASSERT(doc && doc->asSpriteSet() != 0);

        Freeze();
        init();
        refresh();
        Thaw();

        // Connect imagePanel event(s)
        imagePanel->rightClickImage.add(bind(this, &SpriteSetView::onRightClickFrame));
    }

    SpriteSetView::~SpriteSetView() {
        delete contextMenu;
    }

    void SpriteSetView::deleteDocument(Document* doc) {
        getParent()->spriteset.free(doc);
    }

    void SpriteSetView::onSave(wxCommandEvent& event) {
        if (getName().empty()) {
            onSaveAs(event);
        } else {
            saveDocument();
        }
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

    void SpriteSetView::onRightClickFrame(int frameIndex) {
        wxPoint pos = ScreenToClient(::wxGetMousePosition());
        PopupMenu(contextMenu, pos);
    }

    void SpriteSetView::onEditFrame(wxCommandEvent& event) {
        throw std::runtime_error("NYI");
        //_parent->OpenDocument(new CImageView(_parent, &_sprite->Get(_curFrame)));
    }

    void SpriteSetView::onZoomIn(wxCommandEvent& event)    { imagePanel->incZoom(-1);  imagePanel->Refresh(); }
    void SpriteSetView::onZoomOut(wxCommandEvent& event)   { imagePanel->incZoom(1); imagePanel->Refresh(); }
    void SpriteSetView::onZoomNormal(wxCommandEvent& event){ imagePanel->setZoom(16); imagePanel->Refresh(); }

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
        SpriteSet* sprite = getSprite();        

        // Create controls
        hotXEdit = new wxTextCtrl(this, -1);
        hotYEdit = new wxTextCtrl(this, -1);
        hotWidthEdit = new wxTextCtrl(this, -1);
        hotHeightEdit = new wxTextCtrl(this, -1);

        wxButton* newScriptButton = new wxButton(this, id_newanimscript, "&New");
        wxButton* deleteScriptButton = new wxButton(this, id_destroyanimscript, "&Delete");
        wxButton* newMetaDataButton = new wxButton(this, id_newmetadata, "Ne&w");
        wxButton* deleteMetaDataButton = new wxButton(this, id_destroymetadata, "Dele&te");

        animScriptGrid = new wxListCtrl(this, -1, wxDefaultPosition, wxDefaultSize, wxLC_REPORT);
        animScriptGrid->InsertColumn(0, "Name");
        animScriptGrid->InsertColumn(1, "Value");

        metaDataGrid = new wxListCtrl(this, -1, wxDefaultPosition, wxDefaultSize, wxLC_REPORT);
        metaDataGrid->InsertColumn(0, "Name");
        metaDataGrid->InsertColumn(1, "Value");

        imagePanel = new ImageArrayPanel(this, sprite);

        // Create sizers and arrange everything all pretty-like.
        wxSizer* topSizer = new wxBoxSizer(wxHORIZONTAL);
        wxSizer* leftSide = new wxBoxSizer(wxVERTICAL);
        wxSizer* s1 = new wxGridSizer(2, 4);

        s1->Add(new wxStaticText(this, -1, "Hotspot X"));
        s1->Add(hotXEdit);
        s1->Add(new wxStaticText(this, -1, "Hotspot Y"));
        s1->Add(hotYEdit);
        s1->Add(new wxStaticText(this, -1, "Hotspot Width"));
        s1->Add(hotWidthEdit);
        s1->Add(new wxStaticText(this, -1, "Hotspot Height"));
        s1->Add(hotHeightEdit);

        wxSizer* s2 = new wxBoxSizer(wxHORIZONTAL);
        s2->Add(newScriptButton);
        s2->Add(deleteScriptButton);

        wxSizer* s3 = new wxBoxSizer(wxHORIZONTAL);
        s3->Add(newMetaDataButton);
        s3->Add(deleteMetaDataButton);

        leftSide->Add(s1);
        leftSide->Add(new wxStaticText(this, -1, "Animation Scripts"));
        leftSide->Add(animScriptGrid, 1, wxEXPAND);
        leftSide->Add(s2);
        leftSide->Add(new wxStaticText(this, -1, "Metadata"));
        leftSide->Add(metaDataGrid, 1, wxEXPAND);
        leftSide->Add(s3);

        topSizer->Add(leftSide, 0, wxEXPAND);
        topSizer->Add(imagePanel, 1, wxEXPAND);
        
        SetSizer(topSizer);
        topSizer->FitInside(this);

        initMenu();
        initAccelerators();
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

        wxMenu* viewMenu = new wxMenu;
        viewMenu->Append(id_zoomin, "Zoom &in");
        viewMenu->Append(id_zoomout, "Zoom &out");
        viewMenu->Append(id_zoomnormal, "Zoom &normal");
        //viewMenu->Append(id_zoomin2x, "Zoom &in 2x");
        //viewMenu->Append(id_zoomin4x, "Zoom &in 4x");
        menubar->Append(viewMenu, "&View");

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

    void SpriteSetView::refresh() {
        CCHRfile& chr = getSprite()->GetCHR();

        hotXEdit->SetValue(toString(chr.HotX()).c_str());
        hotYEdit->SetValue(toString(chr.HotY()).c_str());
        hotWidthEdit->SetValue(toString(chr.HotW()).c_str());
        hotHeightEdit->SetValue(toString(chr.HotH()).c_str());

        animScriptGrid->Clear();
        int index = 0;
        for (CCHRfile::StringMap::iterator
            iter = chr.moveScripts.begin();
            iter != chr.moveScripts.end();
            iter++
        ) {
            animScriptGrid->InsertItem(index, iter->first.c_str());
            animScriptGrid->SetItem(index, 1, iter->second.c_str());
            index++;
        }

        index = 0;
        metaDataGrid->Clear();
        for (CCHRfile::StringMap::iterator
            iter = chr.metaData.begin();
            iter != chr.metaData.end();
            iter++
        ) {
            metaDataGrid->InsertItem(index, iter->first.c_str());
            metaDataGrid->SetItem(index, 1, iter->second.c_str());
            index++;
        }
    }

    SpriteSet* SpriteSetView::getSprite() {
        // error checking?
        return static_cast<SpriteSet*>(getDocument()->asSpriteSet());
    }
}
