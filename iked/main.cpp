#include "main.h"

#include "resource.h"
#include "log.h"

#include "projectview.h"
#include "codeview.h"

#include <wx\resource.h>

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
    EVT_MENU(id_filequit,CMainWnd::FileQuit)
    EVT_MENU(id_filenewproject,CMainWnd::NewProject)
    EVT_MENU(id_filenewmap,CMainWnd::NewMap)
    EVT_MENU(id_filenewscript,CMainWnd::NewScript)
    EVT_MENU(id_fileopen,CMainWnd::OnOpen)
END_EVENT_TABLE()

CMainWnd::CMainWnd(wxWindow* parent,const wxWindowID id,const wxString& title,
                   const wxPoint& position,const wxSize& size,const long style)
                   : wxMDIParentFrame(parent,id,title,position,size,style)
{
    CreateToolBar(wxNO_BORDER | wxTB_FLAT | wxTB_HORIZONTAL);
    
    wxMenuBar* menu=CreateBasicMenu();
    
    SetMenuBar(menu);
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
    default:
        wxMessageDialog(this,"Not implemented yet","NYI",wxOK).ShowModal();
    };       
}

FileType CMainWnd::GetFileType(const std::string& fname)
{
    const int nExt=8;
    char* ext[] =
    {
        "",
        ".",
        ".ikaprj",
        ".py",
        ".vsp",
        ".chr",
        ".fnt",
        ".map"
    };

    int idx=fname.rfind(".");

    std::string sExt=fname.substr(idx);
    
    strlwr((char*)sExt.c_str());

    for (int i=2; i<nExt; i++)      // magic number.  Suck.
        if (sExt==ext[i])
            return (FileType)i;
    return t_unknown;
}
