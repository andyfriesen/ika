    
// TODO: way too many magic numbers
#include <cassert>

#include "types.h"

#include "main.h"
#include "codeview.h"
#include "fileio.h"

namespace
{
    enum    {   linecountmargin, foldmargin };
};

BEGIN_EVENT_TABLE(CCodeView, IDocView)
    EVT_STC_CHARADDED(CCodeView::id_ed, CCodeView::OnCharAdded)

    EVT_MENU(CCodeView::id_filesave, CCodeView::OnSave)
    EVT_MENU(CCodeView::id_filesaveas, CCodeView::OnSaveAs)
    EVT_MENU(CCodeView::id_fileclose, CCodeView::OnClose)

    EVT_MENU(CCodeView::id_editundo, CCodeView::OnUndo)
    EVT_MENU(CCodeView::id_editredo, CCodeView::OnRedo)
    EVT_MENU(CCodeView::id_editcopy, CCodeView::OnCopy)
    EVT_MENU(CCodeView::id_editcut , CCodeView::OnCut)
    EVT_MENU(CCodeView::id_editpaste, CCodeView::OnPaste)
    EVT_MENU(CCodeView::id_optionsfont, CCodeView::OnSyntaxHighlighting)
    EVT_MENU(CCodeView::id_viewws, CCodeView::OnViewWhiteSpace)

    EVT_MENU(CCodeView::id_editfind, CCodeView::OnFind)
    EVT_MENU(CCodeView::id_editreplace, CCodeView::OnReplace)
    
    EVT_FIND(-1, CCodeView::DoFind)
    EVT_FIND_NEXT(-1, CCodeView::DoFind)
    EVT_FIND_REPLACE(-1, CCodeView::DoFind)
    EVT_FIND_REPLACE_ALL(-1, CCodeView::DoFind)
    EVT_FIND_CLOSE(-1, CCodeView::DoFind)

    EVT_CLOSE(CCodeView::OnClose)

    EVT_STC_MARGINCLICK(id_ed, CCodeView::OnMarginClick)

END_EVENT_TABLE()

// I highly detest extremely imperative code like this. --andy
void CCodeView::InitTextControl()
{
    pTextctrl = new wxStyledTextCtrl(this, id_ed);
    pTextctrl->SetLexer(wxSTC_LEX_PYTHON);
    pTextctrl->SetProperty("fold", "1");

    // TODO: User syntax saving.  -- khross

    // dunno, the font sizes come out differently.  Compensating here.
    wxFont font(10, wxMODERN, wxNORMAL, wxNORMAL, false);

    pTextctrl->StyleSetFont(wxSTC_STYLE_DEFAULT, font);
    pTextctrl->StyleClearAll();

    pTextctrl->SetViewWhiteSpace(wxSTC_WS_INVISIBLE);

    // defaults
    pTextctrl->StyleSetForeground(0,  wxColour(0x80, 0x80, 0x80));  // whitespace
    pTextctrl->StyleSetForeground(1,  wxColour(0x00, 0x7F, 0x00));  // code comments
    pTextctrl->StyleSetForeground(2,  wxColour(0x00, 0x7f, 0x00));  // numeric constants
    pTextctrl->StyleSetForeground(3,  wxColour(0x7f, 0x7f, 0x7f));  // " style string literals
    pTextctrl->StyleSetForeground(4,  wxColour(0x00, 0x7f, 0x7f));  // ' style string literals
    pTextctrl->StyleSetForeground(5,  wxColour(0x00, 0x00, 0x7f));  // keyword
    pTextctrl->StyleSetForeground(6,  wxColour(0x7f, 0x00, 0x7f));  // ''' strings
    pTextctrl->StyleSetForeground(7,  wxColour(0x7f, 0x00, 0x7f));  // """ strings
    pTextctrl->StyleSetForeground(8,  wxColour(0x00, 0x7f, 0x7f));  // class declaration name
    pTextctrl->StyleSetForeground(9,  wxColour(0x00, 0x7f, 0xFF));  // function declarations
    pTextctrl->StyleSetForeground(10, wxColour(0x7F, 0x00, 0xFF));  // operators
    pTextctrl->StyleSetForeground(11, wxColour(0x00, 0x00, 0x00));  // identifiers
    pTextctrl->StyleSetBold(5,  true);
    pTextctrl->StyleSetBold(10, true);
    pTextctrl->StyleSetItalic(1, true);
    
    pTextctrl->SetLexer(wxSTC_LEX_PYTHON);

    pTextctrl->SetKeyWords(0,
        "def lambda class return yield try raise except pass for while if else elif break continue "
        "global as import finally exec del print in is assert from and not or None"
        );

    // init the margins
    pTextctrl->SetMarginWidth       (linecountmargin, 40);    // current line
    pTextctrl->SetMarginType        (linecountmargin, wxSTC_MARGIN_NUMBER);
    pTextctrl->SetMarginSensitive   (linecountmargin, false);

    pTextctrl->SetMarginWidth       (foldmargin, 10);
    pTextctrl->SetMarginType        (foldmargin, wxSTC_MARGIN_SYMBOL);
    pTextctrl->SetMarginSensitive   (foldmargin, true);
    pTextctrl->SetMarginMask        (foldmargin, wxSTC_MASK_FOLDERS);
    pTextctrl->SetModEventMask      (wxSTC_MOD_CHANGEFOLD);

    // fold marker thingies
    const int stylesize = 7;
    const int styles[3][stylesize] = 
    {
        { wxSTC_MARK_MINUS,         wxSTC_MARK_PLUS,        wxSTC_MARK_EMPTY,           wxSTC_MARK_EMPTY,         wxSTC_MARK_EMPTY,                 wxSTC_MARK_EMPTY,                   wxSTC_MARK_EMPTY },
        { wxSTC_MARK_CIRCLEMINUS,   wxSTC_MARK_CIRCLEPLUS,  wxSTC_MARK_VLINE,           wxSTC_MARK_LCORNERCURVE,  wxSTC_MARK_CIRCLEPLUSCONNECTED,   wxSTC_MARK_CIRCLEMINUSCONNECTED,    wxSTC_MARK_TCORNERCURVE },
        { wxSTC_MARK_BOXMINUS,      wxSTC_MARK_BOXPLUS,     wxSTC_MARK_VLINE,           wxSTC_MARK_LCORNER,       wxSTC_MARK_BOXPLUSCONNECTED,      wxSTC_MARK_BOXMINUSCONNECTED,       wxSTC_MARK_TCORNER }
    };

    const int marknum[] =
    {   wxSTC_MARKNUM_FOLDEROPEN,   wxSTC_MARKNUM_FOLDER,   wxSTC_MARKNUM_FOLDERSUB,    wxSTC_MARKNUM_FOLDERTAIL, wxSTC_MARKNUM_FOLDEREND,          wxSTC_MARKNUM_FOLDEROPENMID,        wxSTC_MARKNUM_FOLDERMIDTAIL     };

    for (int i = 0; i < 7; i++)
        pTextctrl->MarkerDefine(marknum[i], styles[2][i]);
   

    bChanged = false;
}

void CCodeView::InitAccelerators()
{
    vector < wxAcceleratorEntry> accel(pParent->CreateBasicAcceleratorTable());

    int p = accel.size();                 // where we start appending to the table
    accel.resize(accel.size()+2);

    accel[p++].Set(wxACCEL_CTRL, (int)'S', id_filesave);
    accel[p++].Set(wxACCEL_CTRL, (int)'W', id_fileclose);

    wxAcceleratorTable table(p, &*accel.begin());

    SetAcceleratorTable(table);
}

CCodeView::CCodeView(CMainWnd* parent,
                   const string& name)
                   : IDocView(parent, name)
{
    // --- Set up the menu ---
    // Basically, it takes the existing menu, and adds a few commands to it here and there.
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
    editmenu->AppendSeparator();
    editmenu->Append(id_editfind,       "&Find...", "");
    editmenu->Append(id_editreplace,    "Replace...", "");
    menubar->Append(editmenu, "&Edit");

    editmenu->Enable(id_editfind, false);
    editmenu->Enable(id_editreplace, false);

    wxMenu* optionsmenu = new wxMenu;
    menubar->Append(optionsmenu, "&Options");
    optionsmenu->Append(id_optionsfont, "Co&lors...", "");

    wxMenu* viewmenu = new wxMenu;
    menubar->Append(viewmenu, "&View");
    viewmenu->Append(id_viewws, "Visible Whitespace", "", true);
       

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

void CCodeView::SetSyntax(int nWhich, wxCommandEvent& event)
{
    // set script font / color / style
    //  -- khross

    
    if (nWhich==0) // whitespace
    {
        /*
        wxColour nColor;
        nColor = GetBackgroundColour();
        
        wxColourData hData;
        hData.SetColour(nColor);
        hData.SetChooseFull(true);

        for (int i = 0; i < 16; i++)
        {
            wxColour nCustom(i * 16, i * 16, i * 16);
            hData.SetCustomColour(i, nCustom);
        }

        wxColourDialog cdialog(this, &hData);
        cdialog.SetTitle("Choose the background color");
        if (cdialog.ShowModal() == wxID_OK)
        {
            wxColourData retData = cdialog.GetColourData();
            nColor               = retData.GetColour();           

            pTextctrl->StyleSetBackground(nWhich, nColor);
            pTextctrl->StyleSetForeground(nWhich, nColor);
            SetBackgroundColour(nColor);
            
            

            for(int j = 1; j < 11; j++)
                pTextctrl->StyleSetBackground(j, nColor);

            
            pTextctrl->Show(true);
            pTextctrl->SetFocus();
            
        }
        */
        return;
    }


    wxFontData f;
    wxFontDialog fontd(this, &f);

    if (fontd.ShowModal() == wxID_OK)
    {

        wxFontData retData  =   fontd.GetFontData();
        wxFont font         =   retData.GetChosenFont();
        wxColour color      =   retData.GetColour();

        pTextctrl->StyleSetFont(font.GetStyle(), font);
        pTextctrl->StyleSetForeground(nWhich, color);
        
        pTextctrl->StyleSetFontAttr(nWhich,         
#ifndef WX232
            // Compensate for wx2.2.9's fruity way of messing up the point sizes.
            font.GetPointSize()-2,
#else
            font.GetPointSize(),
#endif
            font.GetFaceName(),
            font.GetWeight()==wxBOLD,
            font.GetStyle()==wxITALIC,
            font.GetUnderlined());
              
        pTextctrl->Show(true);
        pTextctrl->SetFocus();
    }
}

void CCodeView::OnSyntaxHighlighting(wxCommandEvent& event)
{
    const wxString szChoices[] = 
        {
        //"Whitespace",
        "Comments",
        "Numerical constants",
        "\" Style string literals",
        "' Style string literals",
        "Keywords",
        "''' Style strings",
        "\"\"\" Style strings",
        "Class declarations",
        "Function declarations",
        "Operators",
        "Identifiers" 
        };

    wxSingleChoiceDialog sdialog(
        this,
        "",
        "Syntax highlighting options",
        WXSIZEOF(szChoices),
        szChoices,
        NULL,
        wxOK | wxCANCEL | wxCENTRE,
        wxDefaultPosition
        );

    if (sdialog.ShowModal() == wxID_OK)
        SetSyntax(sdialog.GetSelection()+1, event);

    
}

void CCodeView::OnViewWhiteSpace(wxCommandEvent& event)
{
    wxMenuBar& menubar=*GetMenuBar();

    int nId = menubar.FindMenuItem("&View", "Visible Whitespace");
    assert(nId != wxNOT_FOUND);

    if (pTextctrl->GetViewWhiteSpace() == wxSTC_WS_INVISIBLE)
    {
        pTextctrl->SetViewWhiteSpace(wxSTC_WS_VISIBLEALWAYS);
        menubar.Check(nId, true);
    }
    else
    {
        pTextctrl->SetViewWhiteSpace(wxSTC_WS_INVISIBLE);
        menubar.Check(nId, false);
    }

    pTextctrl->Show(true);
    pTextctrl->SetFocus();
}

void CCodeView::OnFind(wxCommandEvent& event)
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

void CCodeView::OnReplace(wxCommandEvent& event)
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

void CCodeView::DoFind(wxFindDialogEvent& event)
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
            event.GetFlags() & wxFR_MATCHCASE ? true : false
            //event.GetFlags() & wxFR_WHOLEWORD ? true : false
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
            event.GetFlags() & wxFR_MATCHCASE ? true : false
            //event.GetFlags() & wxFR_WHOLEWORD ? true : false
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

void CCodeView::OnStyleNeeded(wxStyledTextEvent& event)
{
    int currEndStyled = pTextctrl->GetEndStyled();
    pTextctrl->Colourise(currEndStyled, event.GetPosition());
}

void CCodeView::OnCharAdded(wxStyledTextEvent& event)
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
        if (s.Right(4)[0]==':')
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

void CCodeView::OnSave(wxCommandEvent& event)
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

void CCodeView::OnSaveAs(wxCommandEvent& event)
{
    wxFileDialog dlg(
        this,
        "Open File",
        "",
        "",
        "Python Scripts (*.py)|*.py|"
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

const void* CCodeView::GetResource() const
{
    // This is kinda dirty, but it works.  The code itself is stored in the text control,
    // so we don't really have a pointer to a resource to give.  So, we return this,
    // since we're guaranteed that no other open document will have the same value.
    return this;
}

// Trivial stuff

void CCodeView::OnMarginClick(wxStyledTextEvent& event)
{
    int nLine = pTextctrl->LineFromPosition(event.GetPosition());

    pTextctrl->ToggleFold(nLine);
}

void CCodeView::OnUndo(wxCommandEvent& event)        {   pTextctrl->Undo();      }
void CCodeView::OnRedo(wxCommandEvent& event)        {   pTextctrl->Redo();      }
void CCodeView::OnCopy(wxCommandEvent& event)        {   pTextctrl->Copy();      }
void CCodeView::OnCut(wxCommandEvent& event)         {   pTextctrl->Cut();       }
void CCodeView::OnPaste(wxCommandEvent& event)       {   pTextctrl->Paste();     }
void CCodeView::OnSelectAll(wxCommandEvent& event)   {   pTextctrl->SelectAll(); }