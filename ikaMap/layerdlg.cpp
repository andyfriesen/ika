
#include <cassert>
#include "wxinc.h"
#include <wx/xrc/xmlres.h>

#include "layerdlg.h"

#include "mainwindow.h"
#include "command.h"
#include "map.h"
#include "misc.h"

#include "log.h"

BEGIN_EVENT_TABLE(LayerDlg, wxDialog)
    EVT_BUTTON(wxID_OK, LayerDlg::OnOk)
    EVT_BUTTON(XRCID("button_resize"), LayerDlg::OnResize)
END_EVENT_TABLE()

template <typename T>
T* LayerDlg::Ctrl(const std::string& name, const T*)
{
    wxWindow* t = FindWindow(XRCID(name.c_str()));

#ifdef DEBUG
    wxASSERT(wxDynamicCast(t, T));
#endif

    return (T*)t;
}

void LayerDlg::OnOk(wxCommandEvent& event)
{
    label = Ctrl<wxTextCtrl>("edit_label")->GetValue().c_str();
    wrapx = Ctrl<wxCheckBox>("check_wrapx")->GetValue();
    wrapy = Ctrl<wxCheckBox>("check_wrapy")->GetValue();
    x     = atoi(Ctrl<wxTextCtrl>("edit_x")->GetValue().c_str());
    y     = atoi(Ctrl<wxTextCtrl>("edit_y")->GetValue().c_str());

    Map::Layer* lay = _mainWnd->GetMap()->GetLayer(_layerIndex);

    if (label != lay->label ||
        wrapx != lay->wrapx ||
        wrapy != lay->wrapy ||
        x     != lay->x     ||
        y     != lay->y)
        wxDialog::OnOK(event);
    else
        wxDialog::OnCancel(event);
}

void LayerDlg::OnResize(wxCommandEvent&)
{
    if (wxMessageBox("Anything beyond the bounds of the new size will be lost.\n"
                     "Are you sure?",
                     "DANGER WILL ROBINSON",
                     wxYES_NO, this)
                     == wxYES)
    {
        int newx = atoi(Ctrl<wxTextCtrl>("edit_width")->GetValue().c_str());
        int newy = atoi(Ctrl<wxTextCtrl>("edit_height")->GetValue().c_str());

        if (newx >= 1 && newy >= 1)
        {
            _mainWnd->HandleCommand(new ResizeLayerCommand(_layerIndex, newx, newy));
            //_lay->Resize(newx, newy);
        }
        else
            wxMessageBox("Width and height must both be positive integers!", "Error", wxOK);
    }
}

LayerDlg::LayerDlg(MainWindow* parent, uint layerIndex)
    : _layerIndex(layerIndex)
    , _mainWnd(parent)
{
    Map::Layer* lay = _mainWnd->GetMap()->GetLayer(layerIndex);

    wxXmlResource::Get()->LoadDialog(this, parent, "dialog_layer");

    Ctrl<wxTextCtrl>("edit_label")->SetValue(lay->label.c_str());
    Ctrl<wxTextCtrl>("edit_width")->SetValue(va("%i", lay->Width()));
    Ctrl<wxTextCtrl>("edit_height")->SetValue(va("%i", lay->Height()));
    Ctrl<wxCheckBox>("check_wrapx")->SetValue(lay->wrapx);
    Ctrl<wxCheckBox>("check_wrapy")->SetValue(lay->wrapy);
    Ctrl<wxTextCtrl>("edit_x")->SetValue(va("%i", lay->x));
    Ctrl<wxTextCtrl>("edit_y")->SetValue(va("%i", lay->y));

    Ctrl<wxPanel>("panel_main")->GetSizer()->Fit(this);
}