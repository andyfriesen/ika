#include "main.h"

#include "resource.h"
#include "log.h"

// Document windows
#include "mapview.h"
#include "projectview.h"
#include "codeview.h"
#include "configdlg.h"

#include <wx\resource.h>
#include "controller.h"

#include "dlg.wxr"

IMPLEMENT_APP(CApp);

bool CApp::OnInit()
{
    initlog("iked.log");
    CMainWnd* mainwnd=new CMainWnd(NULL,-1,"iked",
        wxPoint(-1,-1),
        wxSize(600,400),
        wxDEFAULT_FRAME_STYLE | wxHSCROLL | wxVSCROLL);
    
    mainwnd->Show(TRUE);
    
    SetTopWindow(mainwnd);

    wxResourceParseData(ConfigDlg);
  
    return TRUE;
}

BEGIN_EVENT_TABLE(CMainWnd,wxMDIParentFrame)
    EVT_MENU(CMainWnd::id_filequit,CMainWnd::FileQuit)
    EVT_MENU(CMainWnd::id_filenewproject,CMainWnd::NewProject)
    EVT_MENU(CMainWnd::id_filenewmap,CMainWnd::NewMap)
    EVT_MENU(CMainWnd::id_filenewscript,CMainWnd::NewScript)
    EVT_MENU(CMainWnd::id_fileopen,CMainWnd::OnOpen)

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

void CMainWnd::FileQuit(wxCommandEvent& event)
{
    Close(TRUE);
}

void CMainWnd::NewProject(wxCommandEvent& event)
{
    CProjectWnd* projectwnd=new CProjectWnd(this,"Untitled Project",wxPoint(-1,-1),wxSize(-1,-1),wxDEFAULT_FRAME_STYLE);
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
    CCodeWnd* codeview=new CCodeWnd(
        this,
        "Untitled script",
        wxPoint(-1,-1),
        wxSize(-1,-1),
        wxDEFAULT_FRAME_STYLE);
    codeview->SetFocus();
}

void CMainWnd::OnOpen(wxCommandEvent& event)
{
    wxFileDialog dlg(
        this,
        "Open File",
        "",
        "",
        "All known|*.ikaprj;*.py;*.map;*.vsp;*.chr;*.fnt|"
        "Iked Projects (*.ikaprj)|*.ikaprj|"
        "Python Scripts (*.py)|*.py|"
        "Maps (*.map)|*.map|"
        "VSP Tilesets (*.vsp)|*.vsp|"
        "Sprites (*.chr)|*.chr|"
        "Fonts (*.fnt)|*.fnt|"
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

    switch (type)
    {
    case t_project:
        {
            CProjectWnd* projectview=new CProjectWnd(
                this,
                fname.c_str(),
                wxDefaultPosition,
                wxDefaultSize,
                wxDEFAULT_FRAME_STYLE,
                fname.c_str()
                );

            projectview->SetFocus();
            return;
        }
    case t_script:
        {
            CCodeWnd* codeview=new CCodeWnd(
                this,
                fname.c_str(),
                wxPoint(-1,-1),
                wxSize(-1,-1),
                wxDEFAULT_FRAME_STYLE,
                fname.c_str()
                );
            
            codeview->SetFocus();
            return;
        }
    case t_config:
        {
#if 0
            wxDialog* dlg=new wxDialog();
            dlg->LoadFromResource(this,"ConfigDlg");
            dlg->ShowModal();
            dlg->Destroy();
#else
            CConfigDlg* configdlg=new CConfigDlg(
                this,
                -1,
                fname
                );
#endif
            return;
        }
    case t_map:
        {
            //Map* m=map.Load(fname);

            new CMapView(this,fname.c_str());
            return;
        }
    default:
        wxMessageDialog(this,"Not implemented yet","NYI",wxOK).ShowModal();
    };       
}

FileType CMainWnd::GetFileType(const std::string& fname)
{
    const int nExt=9;
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
    };

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
        wxIcon("wxicon_small_open_folder",wxBITMAP_TYPE_ICO_RESOURCE,16,16),
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
    if (event.GetId() == id_toolnewscript)
    {
        OnToolBarNewScript(event);
    }

    else if (event.GetId() == id_toolopen)
    {
        OnToolBarOpen(event);
    }

    else if (event.GetId() == id_toolnewmap)
    {
        OnToolBarNewMap(event);
    }

    // etc.

}
