
#ifndef DOCVIEW_H
#define DOCVIEW_H

#include "types.h"
#include <wx\wx.h>

class CMainWnd;


class IDocView : public wxMDIChildFrame
{
protected:
    CMainWnd* pParent;
    string sName;

public:
    IDocView(CMainWnd* parent,const string& fname);
    virtual ~IDocView();

    //virtual void OnClose(wxCommandEvent& event);
    virtual void OnSave(wxCommandEvent& event)=0;

    const string& GetFileName() { return sName; }

//    DECLARE_EVENT_TABLE()
};

#endif