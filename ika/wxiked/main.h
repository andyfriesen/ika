
#ifndef MAIN_H
#define MAIN_H

#include <wx/wx.h>
#include "map.h"
#include "vsp.h"
#include "resource.h"

class CApp : public wxApp
{
public:
	virtual bool OnInit();
};

enum appID
{
	id_filequit=1,
	id_filenewmap,
};

class CMainWnd : public wxMDIParentFrame
{
public:
	CMainWnd(wxWindow* parent,const wxWindowID id,const wxString& title,
		const wxPoint& position,const wxSize& size,const long style);

	void OnQuit(wxCloseEvent& event);
	void FileQuit(wxCommandEvent& event);
	void NewMap(wxCommandEvent& event);
private:
	DECLARE_EVENT_TABLE()


private:
	// This window owns all the maps, VSPs, CHrs, etc... that get loaded.  It doles them out to child windows when they ask,
	// and nukes things that no longer need to be in memory

	CResourceController<Map> mapcontroller;
	CResourceController<VSP> vspcontroller;
	// TODO: fonts, sprites, scripts, anything else that comes to mind
};

#endif