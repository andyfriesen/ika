
#include <wx/event.h>

#include "common/chr.h"

#include "document.h"
#include "imageview.h"
#include "listcontrol.h"
#include "importframesdlg.h"
#include "main.h"
#include "movescripteditor.h"
#include "spritesetview.h"

#include "commands/spritecommands.h"

namespace iked {

    namespace {
        enum {
            id_filler = 100,

            id_filesave,
            id_filesaveas,
            id_fileclose,

            id_editundo,
            id_editredo,

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
            id_zoomnormal,

            // controls
            id_animscriptlist,
            id_metadatalist,
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
        //EVT_MENU(id_fileclose, SpriteSetView::onClose)

        EVT_MENU(id_editundo, SpriteSetView::onUndo)
        EVT_MENU(id_editredo, SpriteSetView::onRedo)
        
        EVT_BUTTON(id_newanimscript, SpriteSetView::onNewAnimScript)
        EVT_BUTTON(id_destroyanimscript, SpriteSetView::onDestroyAnimScript)

        EVT_LIST_ITEM_ACTIVATED(id_animscriptlist, SpriteSetView::onEditAnimScript)

        EVT_MENU(id_deleteframe, SpriteSetView::onDestroyFrame)
    END_EVENT_TABLE()

    SpriteSetView::SpriteSetView(MainWindow* parentwnd, Document* doc, const std::string& fileName)
        : DocumentPanel(parentwnd, doc, fileName)
    {
        wxASSERT(doc && doc->asSpriteSet() != 0);

        Freeze();
        init();
        refresh();
        Thaw();

        doc->changed.add(bind(this, (void (SpriteSetView::*)(Document*))&SpriteSetView::onSpriteSetChanged));

        // Connect imagePanel event(s)
        imagePanel->rightClickImage.add(bind(this, &SpriteSetView::onRightClickFrame));
    }

    SpriteSetView::~SpriteSetView() {
        delete contextMenu;
    }

    void SpriteSetView::deleteDocument(Document* doc) {
        //getParent()->spriteset.free(doc);
        the<Controller<SpriteSet> >()->free(doc);
    }

    void SpriteSetView::onSave(wxCommandEvent& event) {
        if (getName().empty()) {
            onSaveAs(event);
        } else {
            refreshSprite();
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
            refreshSprite();

            std::string name = dlg.GetPath().c_str();
            setName(name);
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
        ImportFramesDlg dlg(this);
        SpriteSet* sprite = getSprite();

        if (dlg.ShowModal() != wxID_OK) {
            return;
        }

        if (dlg.frameWidth != sprite->getWidth() || dlg.frameHeight != sprite->getHeight()) {
            if (0 && !dlg.append) {
                //sprite->Resize(dlg.frameWidth, dlg.frameHeight);
            } else {
                // TODO: offer to crop or scale or some other things.
                wxMessageBox("The frames aren't the same size as the sprite!", "Error", wxOK | wxCENTER, this);
                return;
            }
        }

        importFrames(
            dlg.fileName, 
            dlg.frameWidth, 
            dlg.frameHeight, 
            dlg.rowSize, 
            dlg.frameCount, 
            dlg.pad,
            dlg.append ? commands::insertFrames : commands::eraseAndReplaceFrames
        );
    }

    void SpriteSetView::onNewAnimScript(wxCommandEvent& event) {
        int q = rand();
        SpriteSet* sprite = getSprite();
        CCHRfile* chr = &sprite->GetCHR();

        ScopedPtr<wxDialog> dialog = createScriptEditDialog("", "", false);
        int result = dialog->ShowModal();
        if (result == wxID_OK) {
            wxTextCtrl* nameEdit = wxDynamicCast(dialog->FindWindowByName("name"), wxTextCtrl);
            wxTextCtrl* valueEdit = wxDynamicCast(dialog->FindWindowByName("value"), wxTextCtrl);
            wxASSERT(nameEdit != 0);
            wxASSERT(valueEdit != 0);

            std::string name = nameEdit->GetValue().c_str();
            std::string value = valueEdit->GetValue().c_str();

            if (chr->moveScripts.count(name)) {
                wxMessageBox("There already exists a script with that name", "Error", wxOK, this);
            } else {
                sprite->sendCommand(new commands::UpdateSpriteAnimScriptCommand(sprite, name, value));
            }
        }
    }

    void SpriteSetView::onDestroyAnimScript(wxCommandEvent& event) {
        int selection = animScriptGrid->getSelection();
        
        if (selection == -1) {
            return;
        }

        const std::string scriptName = animScriptGrid->getString(selection, 0);
        SpriteSet* sprite = getSprite();
        sprite->sendCommand(new commands::UpdateSpriteAnimScriptCommand(sprite, scriptName, ""));
    }

    void SpriteSetView::onEditAnimScript(wxListEvent& event) {
        int selection = animScriptGrid->getSelection();
        if (selection == -1) {
            return;
        }

        const std::string scriptName = animScriptGrid->getString(selection, 0);

        ScopedPtr<wxDialog> dialog = createScriptEditDialog(scriptName, getSprite()->GetCHR().moveScripts[scriptName], true);
        int result = dialog->ShowModal();
        if (result == wxID_OK) {
            wxTextCtrl* valueEdit = wxDynamicCast(dialog->FindWindowByName("value"), wxTextCtrl);
            wxASSERT(valueEdit != 0);

            getSprite()->sendCommand(
                new commands::UpdateSpriteAnimScriptCommand(
                    getSprite(), 
                    scriptName, 
                    valueEdit->GetValue().c_str()
                )
            );
        } else {
            wxASSERT(result == wxID_CANCEL);
        }
    }

    void SpriteSetView::onDestroyFrame(wxCommandEvent& event) {
        SpriteSet* const spriteSet = getSprite();
        spriteSet->sendCommand(
            new commands::DeleteSpriteFrameCommand(
                spriteSet,
                imagePanel->getSelectedImage()
            )
        );
    }

    void SpriteSetView::onSpriteSetChanged(SpriteSet* spriteSet) {
        assert(spriteSet->asSpriteSet() != 0);
        refresh();
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

        animScriptGrid = new ListControl(this, id_animscriptlist, wxDefaultPosition, wxDefaultSize, wxLC_REPORT);
        animScriptGrid->InsertColumn(0, "Name");
        animScriptGrid->InsertColumn(1, "Value");

        metaDataGrid = new ListControl(this, id_metadatalist, wxDefaultPosition, wxDefaultSize, wxLC_REPORT);
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

        wxMenu* editMenu = new wxMenu;
        editMenu->Append(id_editundo, "&Undo\tCtrl+Z");
        editMenu->Append(id_editredo, "&Redo\tCtrl+Y");
        editMenu->AppendSeparator();
        editMenu->Append(id_chrmovescript, "&Movescript...");
        editMenu->Append(id_chrimportframes, "Import &Frames...");
        menubar->Append(editMenu, "&Edit");

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
        accel.resize(accel.size() + 6);

        accel[p++].Set(wxACCEL_CTRL, (int)'S', id_filesave);
        accel[p++].Set(0, (int)'+', id_zoomin);
        accel[p++].Set(0, (int)'-', id_zoomout);
        accel[p++].Set(0, (int)'=', id_zoomnormal);
        accel[p++].Set(wxACCEL_CTRL, (int)'Z', id_editundo);
        accel[p++].Set(wxACCEL_CTRL, (int)'Y', id_editredo);

        wxAcceleratorTable table(p, &*accel.begin());
        SetAcceleratorTable(table);
    }

    void SpriteSetView::refresh() {
        CCHRfile& chr = getSprite()->GetCHR();

        hotXEdit->SetValue(toString(chr.HotX()).c_str());
        hotYEdit->SetValue(toString(chr.HotY()).c_str());
        hotWidthEdit->SetValue(toString(chr.HotW()).c_str());
        hotHeightEdit->SetValue(toString(chr.HotH()).c_str());

        animScriptGrid->DeleteAllItems();
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
        metaDataGrid->DeleteAllItems();
        for (CCHRfile::StringMap::iterator
            iter = chr.metaData.begin();
            iter != chr.metaData.end();
            iter++
        ) {
            metaDataGrid->InsertItem(index, iter->first.c_str());
            metaDataGrid->SetItem(index, 1, iter->second.c_str());
            index++;
        }

        imagePanel->Refresh();
    }

    void SpriteSetView::refreshSprite() {
        SpriteSet* const sprite = getSprite();
        CCHRfile* const chr = &sprite->GetCHR();
        
        chr->HotX() = atoi(hotXEdit->GetValue().c_str());
        chr->HotY() = atoi(hotYEdit->GetValue().c_str());
        chr->HotW() = atoi(hotWidthEdit->GetValue().c_str());
        chr->HotH() = atoi(hotHeightEdit->GetValue().c_str());
    }

    SpriteSet* SpriteSetView::getSprite() const {
        SpriteSet* sprite = static_cast<SpriteSet*>(getDocument()->asSpriteSet());
        wxASSERT(sprite != 0);
        return sprite;
    }

    wxDialog* SpriteSetView::createScriptEditDialog(const std::string& name, const std::string& value, bool nameReadOnly) {
        // Create a little dialog to edit animation scripts or metadata
        wxDialog* dialog = new wxDialog(this, -1, "Edit animation script", wxDefaultPosition);

        wxTextCtrl* nameEdit = new wxTextCtrl(dialog, -1, name.c_str());
        nameEdit->SetName("name");

        wxTextCtrl* valueEdit = new wxTextCtrl(dialog, -1, value.c_str());
        valueEdit->SetName("value");

        if (nameReadOnly) {
            nameEdit->Enable(false);
        }

        wxSizer* sizer = new wxFlexGridSizer(2, 3);
        sizer->Add(new wxStaticText(dialog, -1, "Name"));
        sizer->Add(nameEdit);
        sizer->Add(new wxStaticText(dialog, -1, "Value"));
        sizer->Add(valueEdit);
        sizer->Add(new wxButton(dialog, wxID_OK, "Ok"));
        sizer->Add(new wxButton(dialog, wxID_CANCEL, "Cancel"));

        dialog->SetSizer(sizer);
        sizer->Fit(dialog);
        return dialog;
    }

    void SpriteSetView::importFrames(
        const std::string& fileName, 
        int frameWidth, 
        int frameHeight, 
        int rowSize, 
        int numFrames,
        bool pad,
        commands::ImportMode importMode
    ) {
        Canvas image(fileName.c_str());

        std::vector<Canvas> frames; 

        int xstep = frameWidth + (pad ? 1 : 0);
        int ystep = frameHeight + (pad ? 1 : 0);

        int xpos = pad ? 1 : 0;
        int ypos = pad ? 1 : 0;
        int curframe = 0;
        int row = 0;
        while (row++, curframe < numFrames) {
            for (int col = 0; col < rowSize && curframe < numFrames; col++) {
                Canvas frame(frameWidth, frameHeight);
                Blitter::Blit(image, frame, -xpos, -ypos, Blitter::OpaqueBlend());
                frames.push_back(frame);
                
                curframe++;

                xpos += xstep;
            }

            xpos = pad ? 1 : 0;
            ypos += ystep;
        }

        getSprite()->sendCommand(
            new commands::ImportSpriteFramesCommand(
                getSprite(),
                frames,
                getSprite()->getCount(),
                importMode
            )
        );
    }
}
