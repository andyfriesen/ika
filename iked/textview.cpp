
#include "common/utility.h"
#include "textview.h"
#include "main.h"
#include "common/fileio.h"

namespace
{
    enum    {   linecountmargin, foldmargin };
};

BEGIN_EVENT_TABLE(CTextView, IDocView)
    EVT_STC_CHARADDED(CTextView::id_ed, CTextView::OnCharAdded)

    EVT_MENU(CTextView::id_filesave, CTextView::OnSave)
    EVT_MENU(CTextView::id_filesaveas, CTextView::OnSaveAs)
    EVT_MENU(CTextView::id_fileclose, CTextView::OnClose)

    EVT_MENU(CTextView::id_editundo, CTextView::OnUndo)
    EVT_MENU(CTextView::id_editredo, CTextView::OnRedo)
    EVT_MENU(CTextView::id_editcopy, CTextView::OnCopy)
    EVT_MENU(CTextView::id_editcut , CTextView::OnCut)
    EVT_MENU(CTextView::id_editpaste, CTextView::OnPaste)

#ifdef WX232
    EVT_MENU(CTextView::id_editfind, CTextView::OnFind)
    EVT_MENU(CTextView::id_editreplace, CTextView::OnReplace)
    
    EVT_FIND(-1, CTextView::DoFind)
    EVT_FIND_NEXT(-1, CTextView::DoFind)
    EVT_FIND_REPLACE(-1, CTextView::DoFind)
    EVT_FIND_REPLACE_ALL(-1, CTextView::DoFind)
    EVT_FIND_CLOSE(-1, CTextView::DoFind)
#endif

    EVT_CLOSE(CTextView::OnClose)

END_EVENT_TABLE()


CTextView::CTextView(CMainWnd* parent, const std::string& name):IDocView(parent, name)
{
    wxMenuBar* menubar = parent->CreateBasicMenu();

    // Extra file menu options
    // the debug build generates a runtime warning from wx.  Ignore it.
    wxMenu* filemenu = menubar->Remove(0);
    filemenu->InsertSeparator(2);
    filemenu->Insert(3, new wxMenuItem(filemenu, id_filesave, "&Save", "Save the script to disk."));
    filemenu->Insert(4, new wxMenuItem(filemenu, id_filesaveas, "Save &As", "Save the script under a new filename."));
    filemenu->Insert(5, new wxMenuItem(filemenu, id_fileclose, "&Close", "Close the script window."));
    menubar->Append(filemenu, "&File");

    wxMenu* editmenu = new wxMenu;
    editmenu->Append(id_editundo,       "&Undo\tCtrl + Z", "");
    editmenu->Append(id_editredo,       "&Redo", "");
    editmenu->AppendSeparator();
    editmenu->Append(id_editcopy,       "&Copy\tCtrl + Ins", "");
    editmenu->Append(id_editcut,        "C&ut\tShift + Del", "");
    editmenu->Append(id_editpaste,      "&Paste\tShift + Ins", "");
    editmenu->Append(id_editselectall,  "Select &All\tCtrl + A", "");
#ifdef WX232
    editmenu->AppendSeparator();
    editmenu->Append(id_editfind,       "&Find...", "");
    editmenu->Append(id_editreplace,    "Replace...", "");
#endif
    menubar->Append(editmenu, "&Edit");

#ifndef WX232
    editmenu->Enable(id_editfind, false);
    editmenu->Enable(id_editreplace, false);
#endif


    SetMenuBar(menubar);
    
    InitTextControl();                  // set up the text control
    InitAccelerators();

    if (name.length())
    {
        File f;
        if (f.OpenRead(name.c_str()))
        {
            int nSize = f.Size()+50;      // +50?  Don't ask.  I haven't the foggiest.
            char* c = new char[nSize];
            memset(c, 0, nSize);

            f.Read(c, nSize);
            pTextctrl->InsertText(0, c);
            delete[] c;
            f.Close();

            this->name = name;
        } 
    }

    pTextctrl->EmptyUndoBuffer();
    pTextctrl->Show(true);
    pTextctrl->SetFocus();
}

void CTextView::InitTextControl()
{
    pTextctrl = new wxStyledTextCtrl(this, id_ed);
    pTextctrl->SetLexer(wxSTC_LEX_NULL);
   // pTextctrl->SetProperty("fold", "1");

    
    // dunno, the font sizes come out differently.  Compensating here.
#ifdef WX232
    wxFont font(10, wxMODERN, wxNORMAL, wxNORMAL, false);
#else
    wxFont font(8, wxMODERN, wxNORMAL, wxNORMAL, false);
#endif

    pTextctrl->StyleSetFont(wxSTC_STYLE_DEFAULT, font);
    pTextctrl->StyleClearAll();
    pTextctrl->SetViewWhiteSpace(wxSTC_WS_INVISIBLE);

    // init the margins
    pTextctrl->SetMarginWidth       (linecountmargin, 40);    // current line
    pTextctrl->SetMarginType        (linecountmargin, wxSTC_MARGIN_NUMBER);
    pTextctrl->SetMarginSensitive   (linecountmargin, false);

    bChanged = false;
}

void CTextView::InitAccelerators()
{
    std::vector<wxAcceleratorEntry> accel(pParent->CreateBasicAcceleratorTable());

    int p = accel.size();                 // where we start appending to the table
    accel.resize(accel.size()+2);

    accel[p++].Set(wxACCEL_CTRL, (int)'S', id_filesave);
    accel[p++].Set(wxACCEL_CTRL, (int)'W', id_fileclose);

    wxAcceleratorTable table(p, &*accel.begin());

    SetAcceleratorTable(table);
}


#ifdef WX232

void CTextView::OnFind(wxCommandEvent& event)
{
    wxFindReplaceData fdata;

    wxFindReplaceDialog* pDialog = new wxFindReplaceDialog
        (
            this,
            &fdata,
            "Find"
            
        );
    pDialog->Show(true);
}

void CTextView::OnReplace(wxCommandEvent& event)
{
    wxFindReplaceData fdata;

    wxFindReplaceDialog* pDialog = new wxFindReplaceDialog
        (
            this,
            &fdata,
            "Replace",
            wxFR_REPLACEDIALOG
        );
    pDialog->Show(true);
}

void CTextView::DoFind(wxFindDialogEvent& event)
{
    // Handles find / replace stuff.
    // TODO: break this up, maybe.
    //   --  khross

    wxEventType type;

    type = event.GetEventType();
    if (type == wxEVT_COMMAND_FIND || type == wxEVT_COMMAND_FIND_NEXT)
    {

        int nLine = pTextctrl->FindText
        (
            pTextctrl->GetCurrentLine(),
            event.GetFlags() & wxFR_DOWN ? pTextctrl->GetLength():-pTextctrl->GetLength(),
            event.GetFindString(),
            event.GetFlags() & wxFR_MATCHCASE ? true : false,
            event.GetFlags() & wxFR_WHOLEWORD ? true : false
        );

        if (nLine)
        {
            pTextctrl->GotoPos(nLine);
            pTextctrl->SetSelection(nLine, nLine+(event.GetFindString().length()));
        }

    }

    else if (type == wxEVT_COMMAND_FIND_REPLACE || type == wxEVT_COMMAND_FIND_REPLACE_ALL)
    {
        int nLine = pTextctrl->FindText
        (
            pTextctrl->GetCurrentLine(),
            event.GetFlags() & wxFR_DOWN ? pTextctrl->GetLength():-pTextctrl->GetLength(),
            event.GetFindString(),
            event.GetFlags() & wxFR_MATCHCASE ? true : false,
            event.GetFlags() & wxFR_WHOLEWORD ? true : false
        );

        if (nLine)
        {
            pTextctrl->GotoPos(nLine);
            pTextctrl->SetSelection(nLine, nLine+(event.GetFindString().length()));
            pTextctrl->ReplaceSelection(event.GetReplaceString());

            if (type == wxEVT_COMMAND_FIND_REPLACE_ALL) DoFind(event);
        }
    }

    wxFindReplaceDialog *pDlg = event.GetDialog();
    pDlg->Destroy();
}

#endif

void CTextView::OnStyleNeeded(wxStyledTextEvent& event)
{
    int currEndStyled = pTextctrl->GetEndStyled();
    pTextctrl->Colourise(currEndStyled, event.GetPosition());
}

void CTextView::OnCharAdded(wxStyledTextEvent& event)
{
    static char linebuf[1000];
    char k = event.GetKey();

    if (k=='\n')
    {
        int nCurline = pTextctrl->GetCurrentLine();

        // go up until we find the last line with something on it, and remember how far it's indented.
        wxString s;
        int nLine = nCurline - 1;
        while (nLine > 0)
        {
            s = pTextctrl->GetLine(nLine);
            if (s.Len()==0)
                nLine--;
            else
            {
                // found one.  Now we simply copy all the spaces and tabs until we find a non - whitespace char
                for (unsigned int i = 0; i < s.Len(); i++)
                {
                    if (s[i]!=' ' && s[i]!='\t')
                        break;
                    linebuf[i]=s[i];
                }
                linebuf[i]=0;
                break;
            }
        }

        // autoindent after def, while, etc....
        // I have NO idea why I'm going 4 back from the end. ;P
        s = pTextctrl->GetLine(nCurline - 1).Trim();
        if (s.Right(4).StartsWith(":"))
            strcat(linebuf, "\t");

        if (linebuf[0])
            pTextctrl->ReplaceSelection(linebuf);
    }

    if (!bChanged)
    {
        bChanged = true;
        SetTitle(wxString("* ")+GetTitle());
    }
}

void CTextView::OnSave(wxCommandEvent& event)
{
    if (name.length()==0)
    {
        OnSaveAs(event);
        return;
    }

    File f;
    if (!f.OpenWrite(name.c_str()))
        return;

    int nSize = pTextctrl->GetTextLength();

    if (nSize)
    {
        // FIXME: pTextctrl->GetText() truncates the last character. ;P

        int start = pTextctrl->GetSelectionStart();
        int end = pTextctrl->GetSelectionEnd();

        pTextctrl->InsertText(nSize, " ");    // hack, so GetText massacres this, and not something the user wrote.

        wxString s = pTextctrl->GetText();
        f.Write(s.c_str(), nSize);

        // now to nuke the char we added on the end
        pTextctrl->SetSelection(nSize, nSize + 1);
        pTextctrl->Clear();

        pTextctrl->SetSelection(start, end);
    }
    
    f.Close();

    if (bChanged)
    {
        bChanged = false;
        SetTitle(name.c_str());   // remove the * from the title
    }
}

void CTextView::OnSaveAs(wxCommandEvent& event)
{
    wxFileDialog dlg(
        this,
        "Open File",
        "",
        "",
        "All files (*.*)|*.*",
        wxSAVE | wxOVERWRITE_PROMPT
        );

    int result = dlg.ShowModal();
    if (result==wxID_CANCEL)
        return;

    name = dlg.GetFilename().c_str();
    SetTitle(name.c_str());

    OnSave(event);
}


const void* CTextView::GetResource() const
{
    return this;
}

void CTextView::OnUndo(wxCommandEvent& event)        {   pTextctrl->Undo();      }
void CTextView::OnRedo(wxCommandEvent& event)        {   pTextctrl->Redo();      }
void CTextView::OnCopy(wxCommandEvent& event)        {   pTextctrl->Copy();      }
void CTextView::OnCut(wxCommandEvent& event)         {   pTextctrl->Cut();       }
void CTextView::OnPaste(wxCommandEvent& event)       {   pTextctrl->Paste();     }
void CTextView::OnSelectAll(wxCommandEvent& event)   {   pTextctrl->SelectAll(); }