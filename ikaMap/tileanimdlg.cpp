
#include <cassert>
#include <wx/xrc/xmlres.h>

#include "tileanimdlg.h"
#include "mainwindow.h"
#include "command.h"

#include "common/log.h"

BEGIN_EVENT_TABLE(TileAnimDlg, wxDialog)
    EVT_LISTBOX(-1, TileAnimDlg::OnSelectStrand)
    EVT_BUTTON(wxID_OK, TileAnimDlg::OnOK)
END_EVENT_TABLE()

TileAnimDlg::TileAnimDlg(MainWindow* mainWnd)
    : _curStrand(-1)
{
    wxXmlResource::Get()->LoadDialog(this, mainWnd, wxT("dialog_tileanim"));

    wxSizer* sizer = XRCCTRL(*this, "panel_main", wxPanel)->GetSizer();
    sizer->Fit(this);

    _strandList = XRCCTRL(*this, "list_animstrands", wxListBox);
    assert(_strandList);
}

Command* TileAnimDlg::Execute(const std::vector<VSP::AnimState>& animData)
{
    _oldData = animData;
    _changedStrands.clear();

    _strandList->Clear();
    for (uint i = 0; i < _oldData.size(); i++)
    {
        _strandList->Append(toString(i).c_str());
    }

    if (ShowModal() != wxID_OK)
        return 0;

    std::vector<Command*> commands;
    
    for (std::map<uint, VSP::AnimState>::iterator
        iter = _changedStrands.begin();
        iter != _changedStrands.end();
        iter++)
    {
        const VSP::AnimState& _new = iter->second;
        const VSP::AnimState& old = _oldData[iter->first];

        // Don't send strands that aren't different.
        if (old.start  != _new.start ||
            old.finish != _new.finish ||
            old.delay  != _new.delay ||
            old.mode   != _new.mode)
        {
            commands.push_back(new UpdateTileAnimStrandCommand(iter->first, iter->second));
        }
    }

    Log::Write("3");

    if (commands.size())
        return new CompositeCommand(commands);
    else
        return 0;
}

void TileAnimDlg::UpdateData()
{
    if (_curStrand < 0)
        return;

    VSP::AnimState strand;
    strand.start   = atoi(XRCCTRL(*this, "edit_starttile", wxTextCtrl)->GetValue().c_str());
    strand.finish  = atoi(XRCCTRL(*this, "edit_endtile", wxTextCtrl)->GetValue().c_str());
    strand.delay   = atoi(XRCCTRL(*this, "edit_delay", wxTextCtrl)->GetValue().c_str());
    strand.mode    = XRCCTRL(*this, "radio_mode", wxRadioBox)->GetSelection();

    _changedStrands[_curStrand] = strand;
}

void TileAnimDlg::UpdateDlg()
{
    VSP::AnimState strand =
        _changedStrands.count(_curStrand) ? _changedStrands[_curStrand]
                                     : _oldData[_curStrand];

    XRCCTRL(*this, "edit_starttile", wxTextCtrl)->SetValue(toString(strand.start).c_str());
    XRCCTRL(*this, "edit_endtile", wxTextCtrl)->SetValue(toString(strand.finish).c_str());
    XRCCTRL(*this, "edit_delay", wxTextCtrl)->SetValue(toString(strand.delay).c_str());
    XRCCTRL(*this, "radio_mode", wxRadioBox)->SetSelection(strand.mode);
}

void TileAnimDlg::OnSelectStrand(wxCommandEvent& event)
{
    if (_curStrand >= 0)
        UpdateData();

    _curStrand = _strandList->GetSelection();

    UpdateDlg();
}

void TileAnimDlg::OnOK(wxCommandEvent& event)
{
    if (_curStrand >= 0)
        UpdateData();
    return wxDialog::OnOK(event);
}