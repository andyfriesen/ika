/*
    Generic document class.  If something is common to all documents, then it should go here, so as to save redundancy.
*/

#ifndef DOCVIEW_H
#define DOCVIEW_H

#include "common/utility.h"
#include "wx/wx.h"

class CMainWnd;


class IDocView : public wxMDIChildFrame
{
protected:
    CMainWnd* pParent;
    std::string name;

    bool bChanged;

public:
    IDocView(CMainWnd* parent, const std::string& fname);
    virtual ~IDocView();

    virtual void OnClose(wxCloseEvent& event);
    virtual void OnSave(wxCommandEvent& event) = 0;

    const std::string& GetFileName() { return name; }

    virtual const void* GetResource() const = 0;        // Returns a pointer to the data that this document is editing.  Some windows just return a unique ID.

    DECLARE_EVENT_TABLE()
};

#endif