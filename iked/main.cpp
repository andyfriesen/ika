#include "main.h"

#include "resource.h"
#include "misc.h"
#include "log.h"

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

#include <wx/xrc/xmlres.h>
#include <wx/laywin.h>
#include <wx/sashwin.h>

#include "controller.h"

#ifdef _MSC_VER
#   pragma warning(disable:4355)    // bah.
#endif

// HACK.  This shouldn't be needed in release mode. -_-
#ifndef DEBUG
void __cdecl wxAssert(int, char const*, int, char const*, char const*)
{
    exit(-1);
}
#endif

IMPLEMENT_APP(CApp);

bool CApp::OnInit()
{
    extern void InitXmlResource(); // resource.cpp

    wxXmlResource::Get()->InitAllHandlers();
#if 0 && defined(DEBUG)
    wxXmlResource::Get()->Load("resource.xrc");
#else
    InitXmlResource();
#endif

    Log::Init("iked.log");
    CMainWnd* mainwnd = new CMainWnd(NULL, -1, "iked",
        wxPoint(-1, -1),
        wxSize(600, 400),
        wxDEFAULT_FRAME_STYLE | wxHSCROLL | wxVSCROLL);
    
    mainwnd->Show(TRUE);
    
    SetTopWindow(mainwnd);
  
    return TRUE;
}

namespace
{
    enum
    {
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
}

BEGIN_EVENT_TABLE(CMainWnd, wxMDIParentFrame)
    EVT_MENU(id_filequit, CMainWnd::FileQuit)
    EVT_MENU(id_filenewproject, CMainWnd::NewProject)
    EVT_MENU(id_filenewmap, CMainWnd::NewMap)
    EVT_MENU(id_filenewscript, CMainWnd::NewScript)
    EVT_MENU(id_filenewsprite, CMainWnd::NewSprite)
    EVT_MENU(id_filenewtileset, CMainWnd::NewTileSet)
    EVT_MENU(id_fileopen, CMainWnd::OnOpen)
    EVT_MENU(id_filesaveproject, CMainWnd::OnSaveProject)
    EVT_MENU(id_filesaveprojectas, CMainWnd::OnSaveProjectAs)

    EVT_SIZE(CMainWnd::OnSize)
    EVT_CLOSE(CMainWnd::OnQuit)
END_EVENT_TABLE()

CMainWnd::CMainWnd(wxWindow* parent, const wxWindowID id, const wxString& title,
                   const wxPoint& position, const wxSize& size, const long style)
                   : wxMDIParentFrame(parent, id, title, position, size, style)
                   , _project(new ProjectView(this))
{
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

    int argc = wxGetApp().argc;
    char** argv = wxGetApp().argv;
    for (int i = 1; i < argc; i++)
    {
        Open(argv[i]);
    }
}

CMainWnd::~CMainWnd()
{
}

void CMainWnd::FileQuit(wxCommandEvent& event)
{
    Close(TRUE);
}

void CMainWnd::NewProject(wxCommandEvent& event)
{
    NewProjectDlg dlg(this);
    if (dlg.ShowModal() == wxID_OK)
    {
        _project->New(dlg.FileName());
    }
}

void CMainWnd::NewMap(wxCommandEvent& event)
{
    /*NewMapDlg dlg(this);
    if (dlg.ShowModal() == wxID_OK)
        OpenDocument(new MapView(this, dlg.width, dlg.height, dlg.tilesetname));*/
}

void CMainWnd::NewScript(wxCommandEvent& event)
{
    CCodeView* codeview = new CCodeView(this, "");
    
    pDocuments.insert(codeview);
    codeview->Activate();
}

void CMainWnd::NewSprite(wxCommandEvent& event)
{
    NewSpriteDlg dlg(this);
    if (dlg.ShowModal() == wxID_OK)
        OpenDocument(new CSpriteSetView(this, dlg.width, dlg.height));
}

void CMainWnd::NewTileSet(wxCommandEvent& event)
{
    NewSpriteDlg dlg(this);
    dlg.SetTitle("New Tileset");

    if (dlg.ShowModal() == wxID_OK)
        OpenDocument(new CTileSetView(this, dlg.width, dlg.height));
}

void CMainWnd::OnOpen(wxCommandEvent& event)
{
    wxFileDialog dlg(
        this,
        "Open File",
        "",
        "",
        "All known|*.ikaprj;*.py;*.ika-map;*.vsp;*.ika-sprite;*.chr;*.fnt;*.txt;*.dat|"
        "iked Projects (*.ikaprj)|*.ikaprj|"
        "Python Scripts (*.py)|*.py|"
        "Maps (*.ika-map)|*.ika-map|"
        "VSP Tilesets (*.vsp)|*.vsp|"
        "Sprites (*.ika-sprite;*.chr)|*.ika-sprite;*.chr|"
        "Fonts (*.fnt)|*.fnt|"
        "Text (*.txt)|*.txt|"
        "Dat Files (*.dat)|*.dat|"
        "All files (*.*)|*.*",
        wxOPEN | wxMULTIPLE
        );

    int result = dlg.ShowModal();
    if (result==wxID_CANCEL)
        return;

    wxArrayString filenames;
    dlg.GetPaths(filenames);

    for (uint i = 0; i < filenames.Count(); i++)
        Open(std::string( filenames[i].c_str() ));
}

void CMainWnd::OnSize(wxSizeEvent& event)
{
    int w, h;
    GetClientSize(&w, &h);

    _project->SetSize(0, 0, 200, h);
    GetClientWindow()->SetSize(200, 0, w - 200, h);
}

void CMainWnd::OnQuit(wxCloseEvent& event)
{
    /*
     * Broken, and I have no idea how to fix it.
     * If windows are open, I close them all, then check to see if any remain.
     * But they still remain, since the close messages just get queued up, for
     * later processing.  I have thus far been unable to force this processing
     * to occur.  Blech.
     */

    for (std::set<IDocView*>::iterator i = pDocuments.begin(); i != pDocuments.end(); i++)
    {
        IDocView* pDoc=*i;
        pDoc->Close();
    }

    if (pDocuments.empty() || !event.CanVeto())
        event.Skip();
    else
        event.Veto();
}

void CMainWnd::Open(const std::string& fname)
{
    try
    {
        // First, see if the document is already open
        for (std::set<IDocView*>::iterator i = pDocuments.begin(); i != pDocuments.end(); i++)
        {
            if (Path::Compare((*i)->GetFileName(), fname))
            {
                (*i)->Activate();
                return;
            }
        }

        // okay.  It's not open.  Open it.
        FileType type = GetFileType(fname);

        IDocView* wnd = 0;

        switch (type)
        {
        case t_script:      wnd = new CCodeView     (this, fname);          break;
        //case t_map:         wnd = new MapView      (this, fname.c_str());  break;
        case t_vsp:         wnd = new CTileSetView  (this, fname.c_str());  break;
        case t_font:        wnd = new CFontView     (this, fname.c_str());  break;
        case t_text:
        case t_dat:         wnd = new CTextView     (this, fname.c_str());  break;
        case t_chr:         wnd = new CSpriteSetView(this, fname.c_str());  break;

        case t_project:     _project->Load(fname);                          return;
        case t_config:
            {
                CConfigDlg* configdlg = new CConfigDlg(
                    this,
                    -1,
                    fname
                    );

                configdlg->ShowModal();
                configdlg->Close(TRUE);

                return;
            }
        case t_unknown: wxMessageDialog(this, "Unknown filetype", "", wxOK).ShowModal();        return;
        default:        wxMessageDialog(this, "Not implemented yet", "NYI", wxOK).ShowModal();  return;
        };   

        // Control only gets here for the conventional files. (not projects, or config files)
        wxASSERT(wnd != 0);
        OpenDocument(wnd);
    }
    catch (std::runtime_error err)
    {
        ::wxMessageBox(va("There was an error loading the file\n%s", err.what()), "Error opening file", wxID_OK | wxCENTER, this);
    }
}

void CMainWnd::OpenDocument(IDocView* newwnd)
{
    pDocuments.insert(newwnd);
    newwnd->Activate();
}

IDocView* CMainWnd::FindWindow(const void* rsrc) const
{
    for (std::set<IDocView*>::const_iterator i = pDocuments.begin(); i != pDocuments.end(); i++)
        if ((*i)->GetResource() == rsrc)
            return *i;

    return 0;
}

void CMainWnd::OnSaveProject(wxCommandEvent& event)
{
    if (!_project->GetFileName().empty())
        _project->Save();
    else
        OnSaveProjectAs(event);
}

void CMainWnd::OnSaveProjectAs(wxCommandEvent& event)
{
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
}

void CMainWnd::OnChildClose(IDocView* child)
{
    pDocuments.erase(child);
}

// creates the base menu items that apply to all app windows.
// This is here because wxWindows won't let me just tack on an extra menu depending on the active MDI child. ;P
wxMenuBar* CMainWnd::CreateBasicMenu()
{
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

std::vector<wxAcceleratorEntry> CMainWnd::CreateBasicAcceleratorTable()
{
    std::vector<wxAcceleratorEntry> accel;
    accel.resize(2);
    accel[0].Set(wxACCEL_CTRL, (int)'O', id_fileopen);
    accel[1].Set(wxACCEL_CTRL, (int)'Q', id_filequit);

    return accel;
}

wxToolBar* CMainWnd::CreateBasicToolBar()
{
    const int SEPARATOR = -1337;
    struct
    {
        int id;
        const char* icon;
        const char* tooltip;
        const char* desc;
    } tools[] =
    {
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

    for (int i = 0; i < numtools; i++)
    {
        if (tools[i].id == SEPARATOR)
            pToolbar->AddSeparator();
        else
            pToolbar->AddTool(
                tools[i].id, 
                wxIcon(tools[i].icon, wxBITMAP_TYPE_ICO_RESOURCE, 16, 16), 
                wxNullBitmap, 
                false, -1, -1, 0, 
                tools[i].tooltip, 
                tools[i].desc);
    }
    return pToolbar;
}

FileType CMainWnd::GetFileType(const std::string& fname)
{
    static const struct
    {
        char* ext;
        FileType type;
    } types[] =
    {
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

    std::string sExt = ::Lower(Path::Extension(fname));
    
    for (int i = 0; i < types_count; i++)
    {
        if (sExt == types[i].ext)
            return types[i].type;
    }

    return t_unknown;
}
