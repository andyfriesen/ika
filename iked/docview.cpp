
#include "docview.h"
#include "main.h"

BEGIN_EVENT_TABLE(IDocView, wxMDIChildFrame)
    EVT_CLOSE(IDocView::OnClose)
END_EVENT_TABLE()

IDocView::IDocView(CMainWnd* parent, const string& fname)
    :   wxMDIChildFrame(parent, -1, fname.c_str(), wxDefaultPosition, wxDefaultSize),
        pParent(parent),    name(fname),   bChanged(false)
{
    SetTitle(fname.length()?
            fname.c_str()   :   "Untitled"
    );
}

IDocView::~IDocView()
{
    pParent->OnChildClose(this);
}

void IDocView::OnClose(wxCommandEvent& event)
{
    if (bChanged)
    {
        wxMessageDialog msgdlg
            (
                this,
                "This file has been modified. Save?",
                name.c_str(),
                wxYES_NO | wxCANCEL | wxICON_QUESTION,
                wxDefaultPosition
            );

        int nDecision = msgdlg.ShowModal();

        switch(nDecision)
        {
            case wxID_YES: OnSave(event); break;
            case wxID_CANCEL: return;
            //case wxID_NO:
        }
    }
    
    Destroy();
}