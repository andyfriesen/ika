
#include "main.h"
#include "mapview.h"
#include "map.h"
#include "vsp.h"
#include "resource.h"
CResourceController<Map> mapcontroller;

IMPLEMENT_APP(CApp);

bool CApp::OnInit()
{
	CMainWnd* mainwnd=new CMainWnd(NULL,-1,"iked",
		wxPoint(-1,-1),
		wxSize(200,200),
		wxDEFAULT_FRAME_STYLE | wxHSCROLL | wxVSCROLL);

	mainwnd->Show(TRUE);

	SetTopWindow(mainwnd);

	return TRUE;
}

BEGIN_EVENT_TABLE(CMainWnd,wxMDIParentFrame)
	EVT_MENU(id_filequit,CMainWnd::FileQuit)
	EVT_MENU(id_filenewmap,CMainWnd::NewMap)
	//EVT_CLOSE(CMainWnd::OnQuit)
END_EVENT_TABLE()

CMainWnd::CMainWnd(wxWindow* parent,const wxWindowID id,const wxString& title,
		const wxPoint& position,const wxSize& size,const long style)
		: wxMDIParentFrame(parent,id,title,position,size,style)
{
	CreateToolBar(wxNO_BORDER | wxTB_FLAT | wxTB_HORIZONTAL);

	wxMenuBar* menu=new wxMenuBar;

	wxMenu* filemenu=new wxMenu;
	filemenu->Append(id_filenewmap,"New &Map","Create a new map");
	filemenu->Append(id_filequit,"&Quit","Close the application");

	menu->Append(filemenu,"&File");

	SetMenuBar(menu);
}

void CMainWnd::OnQuit(wxCloseEvent& event)
{
	event.Skip();
}

void CMainWnd::FileQuit(wxCommandEvent& event)
{
	Close(TRUE);
}

void CMainWnd::NewMap(wxCommandEvent& event)
{
	Map* m=new Map;
	VSP* v=new VSP;

	CMapWnd* mapview=new CMapWnd(this,"New map",wxPoint(-1,-1),wxSize(-1,-1),wxDEFAULT_FRAME_STYLE,m,v);
}