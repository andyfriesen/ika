#ifndef PROJECTVIEW_H
#define PROJECTVIEW_H

#include "types.h"
#include <wx\wx.h>
#include <wx\treectrl.h>
#include <wx\imaglist.h>

class CMainWnd; // in main.cpp/h

class CProjectWnd : public wxMDIChildFrame
{
    class CProjectTree* pTreectrl;
    wxImageList* pImagelist;
    std::string sFilename;

    wxMenu* filemenu;
    wxMenu* foldermenu;

public:

    enum
    {
        id_filler=100,

        id_filesave,
    };

    CProjectWnd(CMainWnd* parent,const wxString& title,const wxPoint& position,const wxSize& size,const long style,const char* fname=0);
    ~CProjectWnd();

    // events
    void OnSave(wxCommandEvent& event);

    // bleh
    void Load(const char* fname);           // load a project file into the window
    void Save(const char* fname);           // save to a project file

    DECLARE_EVENT_TABLE()
};

#endif