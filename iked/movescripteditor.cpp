
#include <wx/grid.h>
#include <wx/xrc/xmlres.h>

#include "movescripteditor.h"
#include "spriteset.h"
#include "spritesetview.h"
#include "chr.h"

#if 1

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
    EVT_CLOSE(CMovescriptEditor::OnClose)

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
            XRCCTRL(*This, name, wxTextCtrl)->SetValue(ToString(value).c_str());
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
        int delta = animScriptGrid->GetRows() - (chr.moveScripts.size() + 1); // we want one row per script, plus one at the end.
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
    CCHRfile::StringMap& animScripts = pSprite->GetCHR().moveScripts;

    animScripts.clear();
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
    //UpdateData();
    Show(false);
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
    int row = event.GetRow();
    int col = event.GetCol();
    const wxString cell = animScriptGrid->GetCellValue(row, col);

    if (cell.empty())
    {
        event.veto();
        //animScriptGrid->DeleteRows(row);
        //UpdateData();
        //pSprite->GetCHR().moveScripts.erase(pSprite->GetCHR().moveScripts.begin() + row);
        //animScriptGrid->DeleteRows(row);
    }
    else
    {
        if (col == 1) // editing the actual script
        {
            std::string name = animScriptGrid->GetCellValue(row, 0).c_str();
            pSprite->GetCHR().moveScripts[name] = cell.c_str();
        }
        // else rename the script (NYI.  Scripts are still ordinal)
        {
            // it'd be awful nice if we could know what was in the cell before it was changed.
            UpdateData();   // -_-
        }
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

#else
//#include "wx\resource.h"

BEGIN_EVENT_TABLE(CMovescriptEditor, wxDialog)
    EVT_CLOSE(CMovescriptEditor::OnClose)
END_EVENT_TABLE()


CMovescriptEditor::CMovescriptEditor(CSpriteSetView* parent, CSpriteSet* sprite, int idx)
    : wxDialog((wxWindow*)parent, -1, "CHR Properties", wxDefaultPosition, wxSize(400, 225))
    , pParent(parent)
    , pSprite(sprite)
    , nCurframe(idx)
{

    InitControls();
    UpdateDlg();

}

void CMovescriptEditor::InitControls()
{
#if 1
    // AGHAGHAGHAGHAGHAGHAGH

    const int column[] =
    {
        10, 85, 180, 245
    };

    const int line[] =
    {
        5, 25, 45, 65, 85, 105, 125, 145, 165, 185, 205, 225,
    };

    const wxSize editsize = wxSize(50, 20);
    const wxSize bigedit = wxSize(300, 20);
    const wxSize labelsize = wxSize(64, 20);
    const wxSize smalllabel = wxSize(14, 20);

    for (int i = 0; i < 4; i++)
        movescript.push_back(new wxTextCtrl(this, -1, "", wxPoint(column[1], line[i]), bigedit));

    pDesc       =new wxTextCtrl(this, -1, "", wxPoint(column[1], line[4]), bigedit);

    pHotx       =new wxTextCtrl(this, -1, "", wxPoint(column[1], line[5]), editsize);
    pHoty       =new wxTextCtrl(this, -1, "", wxPoint(column[3], line[5]), editsize);
    pHotw       =new wxTextCtrl(this, -1, "", wxPoint(column[1], line[6]), editsize);
    pHoth       =new wxTextCtrl(this, -1, "", wxPoint(column[3], line[6]), editsize);

    pLeft       =new wxTextCtrl(this, -1, "", wxPoint(column[1], line[7]), editsize);
    pRight      =new wxTextCtrl(this, -1, "", wxPoint(column[3], line[7]), editsize);
    pUp         =new wxTextCtrl(this, -1, "", wxPoint(column[1], line[8]), editsize);
    pDown       =new wxTextCtrl(this, -1, "", wxPoint(column[3], line[8]), editsize);

    new wxStaticText(this, -1, "Moving up",           wxPoint(column[0], line[0]), labelsize);
    new wxStaticText(this, -1, "Moving down",         wxPoint(column[0], line[1]), labelsize);
    new wxStaticText(this, -1, "Moving left",         wxPoint(column[0], line[2]), labelsize);
    new wxStaticText(this, -1, "Moving right",        wxPoint(column[0], line[3]), labelsize);

    new wxStaticText(this, -1, "Description",         wxPoint(column[0], line[4]), labelsize);
    new wxStaticText(this, -1, "Hot X",               wxPoint(column[0], line[5]), labelsize);
    new wxStaticText(this, -1, "Hot Y",               wxPoint(column[2], line[5]), labelsize);
    new wxStaticText(this, -1, "Hot width",           wxPoint(column[0], line[6]), labelsize);
    new wxStaticText(this, -1, "Hot height",          wxPoint(column[2], line[6]), labelsize);

    new wxStaticText(this, -1, "Idle left",           wxPoint(column[0], line[7]), labelsize);
    new wxStaticText(this, -1, "Idle right",          wxPoint(column[2], line[7]), labelsize);
    new wxStaticText(this, -1, "Idle up",             wxPoint(column[0], line[8]), labelsize);
    new wxStaticText(this, -1, "Idle down",           wxPoint(column[2], line[8]), labelsize);

#else
    // LoadFromResource((wxWindow*)parent, "dialog1");
    // pMovlist=(wxListBox*)wxFindWindowByName("Movescript", this);    
#endif

}

void CMovescriptEditor::UpdateDlg()
{
    CCHRfile& chr = pSprite->GetCHR();

    const uint s = min(movescript.size(), chr.moveScripts.size());
    for (uint i = 0; i < s; i++)
    {
        wxTextCtrl* p = movescript[i];

        const char* c = chr.moveScripts[i].c_str();
        
        p->SetValue(chr.moveScripts[i].c_str());
    }

    pDesc->SetValue(chr.metaData["description"].c_str());

    pHotx->SetValue(ToString(chr.HotX(nCurframe)).c_str());
    pHoty->SetValue(ToString(chr.HotY(nCurframe)).c_str());
    pHotw->SetValue(ToString(chr.HotW(nCurframe)).c_str());
    pHoth->SetValue(ToString(chr.HotH(nCurframe)).c_str());

    pLeft->SetValue(chr.moveScripts[8 + face_left].c_str());
    pRight->SetValue(chr.moveScripts[8 + face_right].c_str());
    pUp->SetValue(chr.moveScripts[8 + face_up].c_str());
    pDown->SetValue(chr.moveScripts[8 + face_down].c_str());

}

void CMovescriptEditor::UpdateData()
{
    CCHRfile& chr = pSprite->GetCHR();

    for (uint i = 0; i < movescript.size(); i++)
        chr.moveScripts[i] = movescript[i]->GetValue().c_str();

    
    chr.metaData["description"] = pDesc->GetValue().c_str();

    chr.moveScripts[8 + face_left]    =   pLeft->GetValue().c_str();
    chr.moveScripts[8 + face_right]   =   pRight->GetValue().c_str();
    chr.moveScripts[8 + face_up]      =   pUp->GetValue().c_str();
    chr.moveScripts[8 + face_down]    =   pDown->GetValue().c_str();

    int& hotx = chr.HotX(nCurframe);
    int& hoty = chr.HotY(nCurframe);
    int& hotw = chr.HotW(nCurframe);
    int& hoth = chr.HotH(nCurframe);

    hotx = atoi(pHotx->GetValue().c_str());
    hoty = atoi(pHoty->GetValue().c_str());
    hotw = atoi(pHotw->GetValue().c_str());
    hoth = atoi(pHoth->GetValue().c_str());

}

void CMovescriptEditor::OnClose(wxCommandEvent& event)
{
    UpdateData();
    Show(false);
}

#endif