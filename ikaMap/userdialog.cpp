
#include <stdexcept>
#include "userdialog.h"
#include "misc.h"

UserDialog::UserDialog(int cols, char* caption, const ArgList& args)
    // TODO: get this to be a child of the main window. :P
    : wxDialog(0, caption, true)
    , _args(args)
{
    cols += cols; // double cols because we attach a caption to each element
    wxSizer* sizer = new wxFlexGridSizer(cols, 3, 3); // 3 pix padding on each axis

    for (ArgList::const_iterator iter = args.begin(); iter != args.end(); iter++)
    {
        const std::string name = iter->first;
        const std::string type = ::Lower(iter->second);

        if (type == "edit")
        {
            wxTextCtrl* t = new wxTextCtrl(this, -1, "", wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, name.c_str());
            t->SetName(name.c_str());
            sizer->Add(new wxStaticText(this, -1, name.c_str()));
            sizer->Add(t);
        }
        else
        {
            throw std::runtime_error(va("Unknown control type %s", type.c_str()));
        }
    }

    sizer->Add(new wxButton(this, wxID_OK, "Ok"));

    SetSizer(sizer);
    sizer->Fit(this);
}

int UserDialog::ShowModal()
{
    int result = wxDialog::ShowModal();
    _results.clear();

    // assemble results:
    for (ArgList::const_iterator iter = _args.begin(); iter != _args.end(); iter++)
    {
        const std::string name = iter->first;
        const std::string type = ::Lower(iter->second);

        if (type == "edit")
        {
            wxTextCtrl* ctrl = wxDynamicCast(wxWindow::FindWindowByName(name.c_str(), this), wxTextCtrl);
            if (!ctrl)
                throw std::runtime_error(va("Internal error: Unable to grab text control '%s'!", name.c_str()));

            _results.push_back(Arg(name, ctrl->GetValue().c_str()));
        }
        else
        {
            throw std::runtime_error(va("Internal error: Unable to handle control type '%s'. :o", type.c_str()));
        }
    }

    return result;
}

UserDialog::ArgList& UserDialog::GetResults()
{
    return _results;
}
