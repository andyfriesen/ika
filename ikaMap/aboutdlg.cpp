#include "aboutdlg.h"
#include "mainwindow.h"
#include <wx/xrc/xmlres.h>

BEGIN_EVENT_TABLE(AboutDlg, wxDialog)
END_EVENT_TABLE()

AboutDlg::AboutDlg(MainWindow* mainWnd)
{
    wxXmlResource::Get()->LoadDialog(this, mainWnd, wxT("dialog_about"));
}
