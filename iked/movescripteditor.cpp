
#include <wx/grid.h>
#include <wx/xrc/xmlres.h>

#include "movescripteditor.h"
#include "spriteset.h"
#include "spritesetview.h"
#include "common/chr.h"

namespace
{
    enum
    {
        id_animgrid = 666,
        id_metadatagrid
    };
}

BEGIN_EVENT_TABLE(CMovescriptEditor, wxDialog)
    //EVT_SIZE(CMovescriptEditor::OnSize) // Doesn't work.  Dammit.
    EVT_BUTTON(wxID_OK, CMovescriptEditor::OnOk)
    //EVT_CLOSE(CMovescriptEditor::OnClose)

    EVT_GRID_EDITOR_SHOWN(CMovescriptEditor::BeginEdit)
    EVT_GRID_CELL_CHANGE(CMovescriptEditor::EditCell)
END_EVENT_TABLE()

CMovescriptEditor::CMovescriptEditor(CSpriteSetView* parent, CSpriteSet* sprite, int idx)
    : pSprite(sprite)
    , pParent(parent)
    , animScriptGrid(0)
    , metaDataGrid(0)
{
    wxXmlResource::Get()->LoadDialog(this, parent, "dialog_spriteproperties");

    animScriptGrid = new wxGrid(this, id_animgrid);
    animScriptGrid->SetRowLabelSize(0);
    animScriptGrid->SetColLabelSize(0);
    animScriptGrid->CreateGrid(8, 2);
    animScriptGrid->EnableGridLines(false);

    metaDataGrid = new wxGrid(this, id_metadatagrid);
    metaDataGrid->SetRowLabelSize(0);
    metaDataGrid->SetColLabelSize(0);
    metaDataGrid->CreateGrid(8, 2);
    metaDataGrid->EnableGridLines(false);


    wxXmlResource::Get()->AttachUnknownControl("unknown_animscript", animScriptGrid);
    wxXmlResource::Get()->AttachUnknownControl("unknown_metadata", metaDataGrid);

    XRCCTRL(*this, "panel_main", wxPanel)->GetSizer()->Fit(this);

    UpdateDlg();
}

void CMovescriptEditor::UpdateDlg()
{
    struct Local
    {
        static void setInt(CMovescriptEditor* This, const char* name, int value)
        {
            XRCCTRL(*This, name, wxTextCtrl)->SetValue(toString(value).c_str());
        }
    };

    CCHRfile& chr = pSprite->GetCHR();

    Local::setInt(this, "edit_hotx", chr.HotX());
    Local::setInt(this, "edit_hoty", chr.HotY());
    Local::setInt(this, "edit_hotwidth", chr.HotW());
    Local::setInt(this, "edit_hotheight", chr.HotH());

    {   
        // Update the animation script table.
        wxASSERT(animScriptGrid != 0);
        animScriptGrid->BeginBatch();
        int delta = animScriptGrid->GetRows() - (chr.moveScripts.size()); // we want one row per script
        if (delta > 0)          animScriptGrid->DeleteRows(delta);
        else if (delta < 0)     animScriptGrid->AppendRows(-delta);

        int i = 0;
        animScriptGrid->ClearGrid();
        for (CCHRfile::StringMap::iterator 
            iter = chr.moveScripts.begin();
            iter != chr.moveScripts.end();
            iter++)
        {
            animScriptGrid->SetCellValue(iter->first.c_str(), i, 0);
            animScriptGrid->SetCellValue(iter->second.c_str(), i, 1);
            animScriptGrid->SetReadOnly(i, 0); // for now, scripts cannot be renamed
            i++;
        }
        animScriptGrid->EndBatch();
    }

    {   
        // Update the metadata table
        wxASSERT(metaDataGrid != 0);
        metaDataGrid->BeginBatch();
        int delta = metaDataGrid->GetRows() - (chr.metaData.size() + 1);
        if (delta > 0)          metaDataGrid->DeleteRows(delta);
        else if (delta < 0)     metaDataGrid->DeleteRows(-delta);

        metaDataGrid->ClearGrid();
        int pos=0;
        for (std::map<std::string, std::string>::iterator i = chr.metaData.begin(); i != chr.metaData.end(); i++)
        {
            metaDataGrid->SetCellValue(i->first.c_str(), pos, 0);
            metaDataGrid->SetCellValue(i->second.c_str(), pos, 1);
            pos++;
        }
        metaDataGrid->EndBatch();
    }
}

void CMovescriptEditor::UpdateData()
{
    CCHRfile& chr = pSprite->GetCHR();
    CCHRfile::StringMap& animScripts = chr.moveScripts;

    //animScripts.clear();
    for (int i = 0; i < animScriptGrid->GetRows(); i++)
    {
        std::string a = animScriptGrid->GetCellValue(i, 0).c_str();
        std::string b = animScriptGrid->GetCellValue(i, 1).c_str();
        animScripts[a] = b;
    }

    chr.HotX() = atoi(XRCCTRL(*this, "edit_hotx", wxTextCtrl)->GetValue().c_str());
    chr.HotY() = atoi(XRCCTRL(*this, "edit_hoty", wxTextCtrl)->GetValue().c_str());
    chr.HotW() = atoi(XRCCTRL(*this, "edit_hotwidth", wxTextCtrl)->GetValue().c_str());
    chr.HotH() = atoi(XRCCTRL(*this, "edit_hotheight", wxTextCtrl)->GetValue().c_str());
}

void CMovescriptEditor::OnSize(wxCommandEvent& event)
{
    // this gets called before the constructor finishes. :P
    if (!animScriptGrid || !metaDataGrid)
        return;

    wxSize newSize(animScriptGrid->GetSize().GetWidth() - animScriptGrid->GetPosition().x, animScriptGrid->GetSize().GetHeight());
    animScriptGrid->SetSize(newSize);
    metaDataGrid->SetSize(newSize);
    XRCCTRL(*this, "panel_main", wxPanel)->GetSizer()->Layout();
}

void CMovescriptEditor::OnClose(wxCommandEvent& event)
{
    UpdateData();
    Show(false);
}

void CMovescriptEditor::OnOk(wxCommandEvent&)
{
    UpdateData();
    EndModal(true);
}

void CMovescriptEditor::BeginEdit(wxGridEvent& event)
{
    wxGrid* grid = wxDynamicCast(event.GetEventObject(), wxGrid);
    wxASSERT(grid);

    oldValue = grid->GetCellValue(event.GetRow(), event.GetCol());
}

void CMovescriptEditor::EditCell(wxGridEvent& event)
{
    wxGrid* grid = wxDynamicCast(event.GetEventObject(), wxGrid);
    wxASSERT(grid != 0);

    if (grid == animScriptGrid)     EditAnimScript(event);
    else if (grid == metaDataGrid)  EditMetaData(event);
    else
        wxASSERT(false);
}

void CMovescriptEditor::EditAnimScript(wxGridEvent& event)
{
    CCHRfile::StringMap& scripts = pSprite->GetCHR().moveScripts;

    int row = event.GetRow();
    int col = event.GetCol();
    const wxString cell = animScriptGrid->GetCellValue(row, col);

    if (col == 0) // renaming the script
    {
        if (!scripts.count(cell.c_str()))
        {
            std::string oldName = oldValue;
            std::string script = scripts[oldValue];
            scripts.erase(oldValue);
            scripts[cell.c_str()] = script;
        }
        else
        {
            ::wxMessageBox("A script with this name already exists.", "oops", wxOK | wxCENTER, this);
            animScriptGrid->SetCellValue(oldValue.c_str(), row, col);
        }
    }
    else // editing the actual script
    {
        std::string name = animScriptGrid->GetCellValue(row, 0).c_str();
        scripts[name] = cell.c_str();
    }
}

void CMovescriptEditor::EditMetaData(wxGridEvent& event)
{
    int row = event.GetRow();
    int col = event.GetCol();
    const std::string cell = metaDataGrid->GetCellValue(row, col).c_str();
    std::map<std::string, std::string>& md = pSprite->GetCHR().metaData;

    if (col == 0)
    {
        if (cell.empty())           // deleting a key
        {
            md.erase(oldValue);
            metaDataGrid->DeleteRows(row);
        }
        else if (!md.count(cell))   // creating a new key
        {
            md[cell] = "";
            metaDataGrid->AppendRows();
        }
        else                        // renaming a key
        {
            md[cell] = md[oldValue];
            md.erase(oldValue);
        }
    }
    else // changing the value of a piece of metadata
    {
        md[metaDataGrid->GetCellValue(row, 0).c_str()] = cell;
    }
}

