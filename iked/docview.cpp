
#include "docview.h"
#include "main.h"

IDocView::IDocView(CMainWnd* parent,const string& fname)
    :   wxMDIChildFrame(parent,-1,fname.c_str(),wxDefaultPosition,wxDefaultSize),
        pParent(parent),    sName(fname)
{
    SetTitle(fname.length()?
            fname.c_str()   :   "Untitled"
    );

}

IDocView::~IDocView()
{
    pParent->OnChildClose(this);
}

