#ifndef PROJECTVIEW_H
#define PROJECTVIEW_H

#include "types.h"
#include <wx\wx.h>
#include <wx\treectrl.h>
#include <wx\imaglist.h>
#include "docview.h"

class CMainWnd; // in main.cpp/h

class CProjectView : public IDocView
{
    class CProjectTree* pTreectrl;
    wxImageList* pImagelist;

    wxMenu* filemenu;
    wxMenu* foldermenu;

public:

    enum
    {
        id_filler=100,

        id_filesave,
        id_filesaveas
    };

    CProjectView(CMainWnd* parent,const string& title);
    ~CProjectView();

    // events
    virtual void OnSave(wxCommandEvent& event);
    void OnSaveAs(wxCommandEvent& event);
    virtual void OnClose(wxCommandEvent& event);
    virtual const void* GetResource() const;

    // bleh
    void Load(const char* fname);           // load a project file into the window
    void Save(const char* fname);           // save to a project file

    DECLARE_EVENT_TABLE()
};

#endif