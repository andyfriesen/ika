#include "main.h"

#include "resource.h"
#include "common/utility.h"
#include "common/log.h"

// Document windows
#include "docview.h"
//#include "mapview.h"
#include "tilesetview.h"
#include "projectview.h"
#include "codeview.h"
#include "configdlg.h"
#include "textview.h"
#include "imageview.h"
#include "spritesetview.h"

#include "newprojectdlg.h"
#include "newmapdlg.h"
#include "newspritedlg.h"
#include "common/chr.h"
#include "common/version.h"

#include <wx/laywin.h>
#include <wx/sashwin.h>

#include "controller.h"

#ifdef _MSC_VER
//#   pragma warning(disable:4355)    // bah.
#endif

// HACK.  Why the hell does the linker want this to begin with?!
#ifndef DEBUG
void __cdecl wxAssert(int, char const*, int, char const*, char const*) {
    exit(-1);
}
#endif

namespace iked {

    namespace {
        enum {
            id_filequit = 1,
            id_filenewproject,
            id_filenewmap,
            id_filenewscript,
            id_filenewsprite,
            id_filenewtileset,
            id_fileopen,
            id_filesaveproject,
            id_filesaveprojectas,
        };

        struct SpriteSetAllocator : Allocator<SpriteSet> {
            SpriteSet* allocate(const std::string& fileName) {
                CCHRfile* chr = new CCHRfile;
                chr->Load(fileName);
                return new SpriteSet(chr, fileName);
            }

            void free(SpriteSet* ss) {
                delete ss;
            }
        };
    }

    BEGIN_EVENT_TABLE(MainWindow, wxMDIParentFrame)
        EVT_MENU(id_filequit, MainWindow::FileQuit)
        /*EVT_MENU(id_filenewproject, MainWindow::NewProject)
        EVT_MENU(id_filenewmap, MainWindow::NewMap)
        EVT_MENU(id_filenewscript, MainWindow::NewScript)
        EVT_MENU(id_filenewtileset, MainWindow::NewTileSet)*/
        EVT_MENU(id_filenewsprite, MainWindow::NewSprite)
        EVT_MENU(id_fileopen, MainWindow::OnOpen)
        EVT_MENU(id_filesaveproject, MainWindow::OnSaveProject)
        EVT_MENU(id_filesaveprojectas, MainWindow::OnSaveProjectAs)

        //EVT_SIZE(MainWindow::OnSize)
        EVT_CLOSE(MainWindow::OnQuit)
    END_EVENT_TABLE()

    MainWindow::MainWindow(const std::vector<std::string>& args)
                    : wxMDIParentFrame(
                        0, -1, va("iked %s", IKA_VERSION), wxDefaultPosition, wxDefaultSize,
                        // wxSize(600, 400),
                        wxDEFAULT_FRAME_STYLE | wxHSCROLL | wxVSCROLL
                    )
//                    , _project(new ProjectView(this))
//                    , spriteset(new SpriteSetAllocator())
    {
        Controller<SpriteSet>::initialize(new SpriteSetAllocator());

        wxToolBar* toolbar = CreateBasicToolBar();
        SetToolBar(toolbar);
        toolbar->Realize();

        wxMenuBar* menu = CreateBasicMenu();
        SetMenuBar(menu);

        SetIcon(wxIcon("appicon", wxBITMAP_TYPE_ICO_RESOURCE, 32, 32));

        CreateStatusBar();

        int widths[] = { -3, -1 };

        GetStatusBar()->SetFieldsCount(2);
        GetStatusBar()->SetStatusWidths(2, widths);

        std::vector<wxAcceleratorEntry> accel(CreateBasicAcceleratorTable());
        wxAcceleratorTable table(accel.size(), &*accel.begin());

        SetAcceleratorTable(table);

        for (size_t i = 1; i < args.size(); i++) {
            Open(args[i]);
        }
    }

    MainWindow::~MainWindow() {
    }

    void MainWindow::FileQuit(wxCommandEvent& event) {
        Close(TRUE);
    }

#if 0
    void MainWindow::NewProject(wxCommandEvent& event) {
        NewProjectDlg dlg(this);
        if (dlg.ShowModal() == wxID_OK) {
            _project->New(dlg.FileName());
        }
    }

    void MainWindow::NewMap(wxCommandEvent& event) {
        /*NewMapDlg dlg(this);
        if (dlg.ShowModal() == wxID_OK)
            OpenDocument(new MapView(this, dlg.width, dlg.height, dlg.tilesetname));*/
    }

    void MainWindow::NewScript(wxCommandEvent& event) {
        CCodeView* codeview = new CCodeView(this, "");
        
        pDocuments.insert(codeview);
        codeview->Activate();
    }

    void MainWindow::NewTileSet(wxCommandEvent& event) {
        NewSpriteDlg dlg(this);
        dlg.SetTitle("New Tileset");

        if (dlg.ShowModal() == wxID_OK)
            OpenDocument(new CTileSetView(this, dlg.width, dlg.height));
    }

#endif

    void MainWindow::NewSprite(wxCommandEvent& event) {
        //NewSpriteDlg dlg(this);

        // Create a dialog to ask the user for the size of the sprite.
        wxDialog dlg(this, -1, "New Sprite", wxDefaultPosition);
        wxDialog* const dialog = &dlg; // I like pointer syntax better

        wxTextCtrl* widthEdit = new wxTextCtrl(dialog, -1);
        wxTextCtrl* heightEdit = new wxTextCtrl(dialog, -1);

        wxSizer* s = new wxFlexGridSizer(2, 3);
        s->Add(new wxStaticText(dialog, -1, "Width"));
        s->Add(widthEdit);
        s->Add(new wxStaticText(dialog, -1, "Height"));
        s->Add(heightEdit);
        s->Add(new wxButton(dialog, wxID_OK, "Ok"));
        s->Add(new wxButton(dialog, wxID_CANCEL, "Cancel"));
        dialog->SetSizer(s);
        s->Fit(dialog);

        int result = dialog->ShowModal();

        if (result == wxID_OK) {
            int width = atoi(widthEdit->GetValue().c_str());
            int height = atoi(heightEdit->GetValue().c_str());
            SpriteSet* sprite = new SpriteSet(new CCHRfile(width, height), "");
            SpriteSetView* view = new SpriteSetView(this, sprite, "");

            OpenDocument(view);
        }
    }

    void MainWindow::OnOpen(wxCommandEvent& event) {
        wxFileDialog dlg(
            this,
            "Open File",
            "",
            "",
            "All known|*.ika-sprite;*.chr|"
            "Sprites (*.ika-sprite;*.chr)|*.ika-sprite;*.chr|"
            "All files (*.*)|*.*"
            /*"All known|*.ikaprj;*.py;*.ika-map;*.vsp;*.ika-sprite;*.chr;*.fnt;*.txt;*.dat|"
            "iked Projects (*.ikaprj)|*.ikaprj|"
            "Python Scripts (*.py)|*.py|"
            "Maps (*.ika-map)|*.ika-map|"
            "VSP Tilesets (*.vsp)|*.vsp|"
            "Sprites (*.ika-sprite;*.chr)|*.ika-sprite;*.chr|"
            "Fonts (*.fnt)|*.fnt|"
            "Text (*.txt)|*.txt|"
            "Dat Files (*.dat)|*.dat|"
            "All files (*.*)|*.*"*/,
            wxOPEN | wxMULTIPLE
        );

        int result = dlg.ShowModal();
        if (result==wxID_CANCEL) {
            return;
        }

        wxArrayString filenames;
        dlg.GetPaths(filenames);

        for (uint i = 0; i < filenames.Count(); i++) {
            Open(std::string( filenames[i].c_str() ));
        }
    }

    void MainWindow::OnSize(wxSizeEvent& event) {
        int w, h;
        GetClientSize(&w, &h);

        //_project->SetSize(0, 0, 200, h);
        GetClientWindow()->SetSize(200, 0, w - 200, h);
    }

    void MainWindow::OnQuit(wxCloseEvent& event) {
        /*
         * Close each window in succession.
         * Windows can refuse to close, however, and we respect that: 
         * if any windows remain open after the fact, cancel the 
         * close operation and stay open.
         */

        std::set<DocumentPanel*>::iterator iter = pDocuments.begin();
        while (iter != pDocuments.end()) {
            DocumentPanel* panel = *pDocuments.begin();
            ++iter;
            panel->Close();
        }

        if (pDocuments.empty() || !event.CanVeto()) {
            event.Skip();
        } else {
            event.Veto();
        }
    }

    void MainWindow::Open(const std::string& fname) {
        try {
#if 1
            // First, see if the document is already open
            foreach (DocumentPanel* panel, pDocuments) {
                if (Path::equals(panel->getName(), fname)) {
                    panel->Activate();
                    return;
                }
            }
#endif

            // okay.  It's not open.  Open it.
            FileType type = GetFileType(fname);

            DocumentPanel* wnd = 0;

            switch (type) {
                case t_chr: {
                    wnd = new SpriteSetView(this, the<Controller<SpriteSet> >()->get(fname), fname.c_str());  
                    break;
                }

                /*            
                case t_script: {
                    wnd = new CCodeView(this, fname);          
                    break;
                }

                case t_map: {
                    wnd = new MapView(this, fname.c_str());  break;
                }

                case t_vsp: {
                    wnd = new CTileSetView(this, fname.c_str());  break;
                }

                case t_font: {
                    wnd = new FontView(this, fname.c_str());  break;
                }

                case t_text: {
                case t_dat: 
                    wnd = new CTextView(this, fname.c_str());  break;
                }

                case t_project: {
                    _project->Load(fname);                          
                    return;
                }

                case t_config: {
                    CConfigDlg* configdlg = new CConfigDlg(this, -1, fname);

                    configdlg->ShowModal();
                    configdlg->Close(TRUE);

                    return;
                }

                case t_unknown: {
                    wxMessageDialog(this, "Unknown filetype", "", wxOK).ShowModal();        return;
                }
                */

                default: {
                    wxMessageDialog(this, "Not implemented yet", "NYI", wxOK).ShowModal();  return;
                }
            };   

            // Control only gets here for the conventional files. (not projects, or config files)
            wxASSERT(wnd != 0);
            OpenDocument(wnd);
        }
        catch (std::runtime_error err) {
            ::wxMessageBox(va("Unable to load '%s':\n%s", fname.c_str(), err.what()), "Error opening file", wxID_OK | wxCENTER, this);
        }
    }

    void MainWindow::OpenDocument(DocumentPanel* newwnd) {
        pDocuments.insert(newwnd);
        newwnd->Activate();
    }

    DocumentPanel* MainWindow::FindWindow(const void* rsrc) const {
#if 0
        for (std::set<DocumentPanel*>::const_iterator i = pDocuments.begin(); i != pDocuments.end(); i++) {
            if ((*i)->GetResource() == rsrc) {
                return *i;
            }
        }
#endif

        return 0;
    }

    void MainWindow::OnSaveProject(wxCommandEvent& event) {
#if 0
        if (!_project->GetFileName().empty()) {
            _project->Save();
        } else {
            OnSaveProjectAs(event);
        }
#endif
    }

    void MainWindow::OnSaveProjectAs(wxCommandEvent& event) {
#if 0
        wxFileDialog dlg(
            this,
            "Save Project As",
            "",
            "",
            "iked Projects (*.ikaprj)|*.ikaprj|"
            "All files (*.*)|*.*",
            wxSAVE | wxOVERWRITE_PROMPT | wxCHANGE_DIR
            );

        int result = dlg.ShowModal();

        if (result != wxCANCEL)
            _project->Save(dlg.GetPath().c_str());
#endif
    }

    void MainWindow::OnChildClose(DocumentPanel* child) {
        pDocuments.erase(child);
    }

    // creates the base menu items that apply to all app windows.
    // This is here because wxWindows won't let me just tack on an extra menu depending on the active MDI child. ;P
    wxMenuBar* MainWindow::CreateBasicMenu() {
        wxMenuBar* menu = new wxMenuBar;
        
        wxMenu* filemenu = new wxMenu;

        wxMenu* filenew = new wxMenu;
        filenew->Append(id_filenewproject, "&Project", "Create an empty project workspace.");
        filenew->AppendSeparator();
        filenew->Append(id_filenewmap, "&Map", "Create an empty map.");
        filenew->Append(id_filenewscript, "&Script", "Create an empty Python script.");
        filenew->Append(id_filenewsprite, "S&prite", "Create a new sprite.");
        filenew->Append(id_filenewtileset, "&Tileset", "Create a new tileset.");

        filemenu->Append(id_filenewmap, "&New", filenew, "Create a new document.");  
        filemenu->Append(id_fileopen, "&Open", "Open an existing document.");
        filemenu->AppendSeparator();
        filemenu->Append(id_filesaveproject, "Save &Project");
        filemenu->Append(id_filesaveprojectas, "Save P&roject As...");
        filemenu->Append(id_filequit, "&Quit", "Close the application.");
        
        menu->Append(filemenu, "&File");

        return menu;
    }

    std::vector<wxAcceleratorEntry> MainWindow::CreateBasicAcceleratorTable() {
        std::vector<wxAcceleratorEntry> accel;
        accel.resize(2);
        accel[0].Set(wxACCEL_CTRL, (int)'O', id_fileopen);
        accel[1].Set(wxACCEL_CTRL, (int)'Q', id_filequit);

        return accel;
    }

    wxToolBar* MainWindow::CreateBasicToolBar() {
        const int SEPARATOR = -1337;
        struct {
            int id;
            const char* icon;
            const char* tooltip;
            const char* desc;
        } tools[] = {
            {   id_fileopen,        "foldericon",   "Open",             "Open a file."  },
            {   SEPARATOR   },
            {   id_filenewmap,      "mapicon",      "Create a map",     "Create a new map." },
            {   id_filenewscript,   "scripticon",   "Create script",    "Create a new Python script."   },
            {   id_filenewsprite,   "spriteicon",   "Create sprite",    "Create a new sprite."  },
            {   id_filenewtileset,  "vspicon",      "Create tileset",   "Create a new tileset." },
        };
        const int numtools = sizeof tools / sizeof tools[0];

        wxToolBar* pToolbar = new wxToolBar
            (
                this,
                -1/*,
                wxDefaultPosition,
                wxDefaultSize,
                wxTB_HORIZONTAL | wxNO_BORDER,
                "iked toolbar"*/
            );

        for (int i = 0; i < numtools; i++) {
            if (tools[i].id == SEPARATOR) {
                pToolbar->AddSeparator();
            } else {
                pToolbar->AddTool(
                    tools[i].id, 
                    wxIcon(tools[i].icon, wxBITMAP_TYPE_ICO_RESOURCE, 16, 16), 
                    wxNullBitmap, 
                    false, -1, -1, 0, 
                    tools[i].tooltip, 
                    tools[i].desc);
            }
        }
        return pToolbar;
    }

    FileType MainWindow::GetFileType(const std::string& fname) {
        static const struct {
            char* ext;
            FileType type;
        } types[] = {
            { "ikaprj",     t_project   },
            { "py",         t_script    },
            { "vsp",        t_vsp       },
            { "chr",        t_chr       },
            { "ika-sprite", t_chr       },
            { "fnt",        t_font      },
            { "ika-map",    t_map       },
            { "cfg",        t_config    },
            { "txt",        t_text      },
            { "dat",        t_text      }
        };

        static const types_count = sizeof types / sizeof types[0];

        std::string sExt = ::toLower(Path::getExtension(fname));
        
        for (int i = 0; i < types_count; i++) {
            if (sExt == types[i].ext) {
                return types[i].type;
            }
        }

        return t_unknown;
    }

}
