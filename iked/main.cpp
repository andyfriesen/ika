#include "main.h"

#include "resource.h"
#include "log.h"

// Document windows
#include "docview.h"
#include "mapview.h"
#include "tilesetview.h"
#include "projectview.h"
#include "codeview.h"
#include "configdlg.h"
#include "textview.h"

#include <wx\resource.h>
#include "controller.h"

IMPLEMENT_APP(CApp);

bool CApp::OnInit()
{
    Log::Init("iked.log");
    CMainWnd* mainwnd=new CMainWnd(NULL,-1,"iked",
        wxPoint(-1,-1),
        wxSize(600,400),
        wxDEFAULT_FRAME_STYLE | wxHSCROLL | wxVSCROLL);
    
    mainwnd->Show(TRUE);
    
    SetTopWindow(mainwnd);

    wxString sDlginfo=wxLoadUserResource("DIALOGS","TEXT");
    wxResourceParseString(const_cast<char*>(sDlginfo.c_str()));
  
    return TRUE;
}

BEGIN_EVENT_TABLE(CMainWnd,wxMDIParentFrame)
    EVT_MENU(CMainWnd::id_filequit,CMainWnd::FileQuit)
    EVT_MENU(CMainWnd::id_filenewproject,CMainWnd::NewProject)
    EVT_MENU(CMainWnd::id_filenewmap,CMainWnd::NewMap)
    EVT_MENU(CMainWnd::id_filenewscript,CMainWnd::NewScript)
    EVT_MENU(CMainWnd::id_fileopen,CMainWnd::OnOpen)

    EVT_CLOSE(CMainWnd::OnQuit)

    // Add more toolbar buttons as iked becomes more functional
    //   -- khross
    EVT_MENU(-1, CMainWnd::OnToolLeftClick)
    EVT_TOOL(CMainWnd::id_toolopen,CMainWnd::OnToolBarOpen)
    EVT_TOOL(CMainWnd::id_toolnewscript,CMainWnd::OnToolBarNewScript)
    EVT_TOOL(CMainWnd::id_toolnewmap,CMainWnd::OnToolBarNewMap)
    
END_EVENT_TABLE()

CMainWnd::CMainWnd(wxWindow* parent,const wxWindowID id,const wxString& title,
                   const wxPoint& position,const wxSize& size,const long style)
                   : wxMDIParentFrame(parent,id,title,position,size,style)
{

    wxToolBar* toolbar = CreateBasicToolBar();
    SetToolBar(toolbar);
    toolbar->Realize();

    wxMenuBar* menu=CreateBasicMenu();
    SetMenuBar(menu);

    CreateStatusBar();

    int nWidths[]={ -3,-1 };

    GetStatusBar()->SetFieldsCount(2);
    GetStatusBar()->SetStatusWidths(2,nWidths);

    vector<wxAcceleratorEntry> accel(CreateBasicAcceleratorTable());
    wxAcceleratorTable table(accel.size(),&*accel.begin());

    SetAcceleratorTable(table);
}

CMainWnd::~CMainWnd()
{
}

// creates the base menu items that apply to all app windows.
// This is here because wxWindows won't let me just tack on an extra menu depending on the active MDI child. ;P
wxMenuBar* CMainWnd::CreateBasicMenu()
{
    wxMenuBar* menu=new wxMenuBar;
    
    wxMenu* filemenu=new wxMenu;

    wxMenu* filenew=new wxMenu;
    filenew->Append(id_filenewproject,"&Project","Create an empty project workspace.");
    filenew->AppendSeparator();
    //filenew->Append(id_filenewmap,"&Map","Open a fresh, blank map.");
    filenew->Append(id_filenewscript,"New &Script","Create an empty Python script.");

    filemenu->Append(id_filenewmap,"&New",filenew,"Create a new document.");  
    filemenu->Append(id_fileopen,"&Open","Open an existing document.");
    filemenu->AppendSeparator();
    filemenu->Append(id_filequit,"&Quit","Close the application.");
    
    menu->Append(filemenu,"&File");

    return menu;
}

vector<wxAcceleratorEntry> CMainWnd::CreateBasicAcceleratorTable()
{
    vector<wxAcceleratorEntry> accel;
    accel.resize(2);
    accel[0].Set(wxACCEL_CTRL,(int)'O',id_fileopen);
    accel[1].Set(wxACCEL_CTRL,(int)'Q',id_filequit);

    return accel;
}

void CMainWnd::FileQuit(wxCommandEvent& event)
{
    Close(TRUE);
}

void CMainWnd::OnQuit(wxCloseEvent& event)
{
    for (std::set<IDocView*>::iterator i=pDocuments.begin(); i!=pDocuments.end(); i++)
    {
        IDocView* pDoc=*i;
        pDoc->Close();
    }

    pDocuments.empty();

    // continue with the default behaviour
    event.Skip();        
}

void CMainWnd::NewProject(wxCommandEvent& event)
{
    CProjectView* projectwnd=new CProjectView(this,"");
    projectwnd->SetFocus();
}

void CMainWnd::NewMap(wxCommandEvent& event)
{
/*    Map* m=new Map;
    VSP* v=new VSP;
    
    CMapWnd* mapview=new CMapWnd(this,"New map",wxPoint(-1,-1),wxSize(-1,-1),wxDEFAULT_FRAME_STYLE,m,v);*/
}

void CMainWnd::NewScript(wxCommandEvent& event)
{
    CCodeView* codeview=new CCodeView(this,"");
    
    pDocuments.insert(codeview);
    codeview->Activate();
}

void CMainWnd::OnOpen(wxCommandEvent& event)
{
    wxFileDialog dlg(
        this,
        "Open File",
        "",
        "",
        "All known|*.ikaprj;*.py;*.map;*.vsp;*.chr;*.fnt;*.txt;*.dat|"
        "Iked Projects (*.ikaprj)|*.ikaprj|"
        "Python Scripts (*.py)|*.py|"
        "Maps (*.map)|*.map|"
        "VSP Tilesets (*.vsp)|*.vsp|"
        "Sprites (*.chr)|*.chr|"
        "Fonts (*.fnt)|*.fnt|"
        "Text (*.txt)|*.txt|"
        "Dat Files (*.dat)|*.dat|"
        "All files (*.*)|*.*",
        wxOPEN | wxMULTIPLE
        );

    int result=dlg.ShowModal();
    if (result==wxID_CANCEL)
        return;

    wxArrayString filenames;
    dlg.GetPaths(filenames);

    for (int i=0; i<filenames.Count(); i++)
        Open(std::string( filenames[i].c_str() ));
}

void CMainWnd::Open(const std::string& fname)
{
    FileType type=GetFileType(fname);

    IDocView* pWnd;

    switch (type)
    {
    case t_project:     pWnd=new CProjectView(this,fname);      break;
    case t_script:      pWnd=new CCodeView(this,fname);         break;
    case t_map:         pWnd=new CMapView(this,fname.c_str());  break;
    case t_vsp:         pWnd=new CTileSetView(this,fname.c_str());  break;
    case t_font:        pWnd=new CFontView(this,fname.c_str()); break;
    case t_text:
    case t_dat:         pWnd=new CTextView(this,fname.c_str()); break;

    case t_config:
        {
            CConfigDlg* configdlg=new CConfigDlg(
                this,
                -1,
                fname
                );

            configdlg->ShowModal();
            configdlg->Close(TRUE);

            return;
        }
    default:
        {
            wxMessageDialog(this,"Not implemented yet","NYI",wxOK).ShowModal();
            return;
        }
    };   
    
    pDocuments.insert(pWnd);
    pWnd->Activate();
}

FileType CMainWnd::GetFileType(const std::string& fname)
{
    
    char* ext[] =
    {
        "",
        "",
        "ikaprj",
        "py",
        "vsp",
        "chr",
        "fnt",
        "map",
        "cfg",
        "txt",
        "dat"
    };

    const int nExt=sizeof(ext);

    int idx=fname.rfind(".")+1;

    std::string sExt=fname.substr(idx);
    
    strlwr((char*)sExt.c_str());

    for (int i=2; i<nExt; i++)      // magic number.  Suck.
        if (sExt==ext[i])
            return (FileType)i;
    return t_unknown;
}

wxToolBar* CMainWnd::CreateBasicToolBar()
{
    wxToolBar* pToolbar = new wxToolBar
        (
            this,
            -1,
            wxDefaultPosition,
            wxDefaultSize,
            wxTB_HORIZONTAL | wxNO_BORDER,
            "iked toolbar"
         );
    

    pToolbar->AddTool(
        id_toolopen,
        wxIcon("foldericon",wxBITMAP_TYPE_ICO_RESOURCE,16,16),
        wxNullBitmap,
        false,
        -1,-1,
        NULL,
        "Open",
        "Open a file.");

    pToolbar->AddSeparator();

    pToolbar->AddTool(
        id_toolnewscript,
        wxIcon("pyicon",wxBITMAP_TYPE_ICO_RESOURCE,16,16),
        wxNullBitmap,
        false,
        -1,-1,
        NULL,
        "Create source",
        "Create a new source document.");

    // doesn't do anything yet.
    pToolbar->AddTool(
        id_toolnewmap,
        wxIcon("mapicon",wxBITMAP_TYPE_ICO_RESOURCE,16,16),
        wxNullBitmap,
        false,
        -1,-1,
        NULL,
        "Create map",
        "Create a new map.");

    //pToolbar->Realize();
    return pToolbar;
}

void CMainWnd::OnToolBarNewScript(wxCommandEvent& event)
{
    wxToolBar* pToolbar = GetToolBar();
    if (!pToolbar) return;
    pToolbar->EnableTool(id_toolnewscript,!pToolbar->GetToolState(id_toolnewscript));
    NewScript(event);
}

void CMainWnd::OnToolBarOpen(wxCommandEvent& event)
{
    wxToolBar* pToolbar = GetToolBar();
    if (!pToolbar) return;
    pToolbar->EnableTool(id_toolopen,!pToolbar->GetToolState(id_toolopen));
    OnOpen(event);
}

void CMainWnd::OnToolBarNewMap(wxCommandEvent& event)
{
    wxToolBar* pToolbar = GetToolBar();
    if (!pToolbar) return;
    pToolbar->EnableTool(id_toolopen,!pToolbar->GetToolState(id_toolopen));
    // ....
}


void CMainWnd::OnToolLeftClick(wxCommandEvent& event)
// this handles the left click mouse event.
{
    switch (event.GetId())
    {
    case id_toolnewscript:  OnToolBarNewScript(event);  break;
    case id_toolopen:       OnToolBarOpen(event);       break;
    case id_toolnewmap:     OnToolBarNewMap(event);     break;
    }

    // etc.
}

void CMainWnd::OnChildClose(IDocView* child)
{
    pDocuments.erase(child);
}