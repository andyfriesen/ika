    
// TODO: way too many magic numbers


#include "types.h"

#include "main.h"
#include "codeview.h"
#include "fileio.h"


namespace
{
    enum    {   linecountmargin, foldmargin };
};

BEGIN_EVENT_TABLE(CCodeWnd,wxMDIChildFrame)
//    EVT_STC_STYLENEEDED(CCodeWnd::id_ed,CCodeWnd::OnStyleNeeded)
    EVT_STC_CHARADDED(CCodeWnd::id_ed,CCodeWnd::OnCharAdded)
    EVT_CLOSE(CCodeWnd::OnClose)

    EVT_MENU(CCodeWnd::id_filesave,CCodeWnd::OnSave)
    EVT_MENU(CCodeWnd::id_filesaveas,CCodeWnd::OnSaveAs)
    EVT_MENU(CCodeWnd::id_fileclose,CCodeWnd::OnClose)

    EVT_MENU(CCodeWnd::id_editundo,CCodeWnd::OnUndo)
    EVT_MENU(CCodeWnd::id_editredo,CCodeWnd::OnRedo)
    EVT_MENU(CCodeWnd::id_editcopy,CCodeWnd::OnCopy)
    EVT_MENU(CCodeWnd::id_editcut ,CCodeWnd::OnCut)
    EVT_MENU(CCodeWnd::id_editpaste,CCodeWnd::OnPaste)
    EVT_MENU(CCodeWnd::id_optionsfont,CCodeWnd::OnSyntaxHighlighting)

    EVT_MENU(CCodeWnd::id_editfind,CCodeWnd::OnFind)
    EVT_MENU(CCodeWnd::id_editreplace,CCodeWnd::OnReplace)
    
    EVT_FIND(-1,CCodeWnd::DoFind)
    EVT_FIND_NEXT(-1,CCodeWnd::DoFind)
    EVT_FIND_REPLACE(-1,CCodeWnd::DoFind)
    EVT_FIND_REPLACE_ALL(-1,CCodeWnd::DoFind)
    EVT_FIND_CLOSE(-1,CCodeWnd::DoFind)

END_EVENT_TABLE()

CCodeWnd::CCodeWnd(CMainWnd* parent,
                   const wxString& title,
                   const wxPoint& position,
                   const wxSize& size,
                   const long style,
                   const char* fname)
                   : wxMDIChildFrame(parent,-1,title,position,size,style)
{
    // --- Set up the menu ---
    // Basically, it takes the existing menu, and adds a few commands to it here and there.
    wxMenuBar* menubar=parent->CreateBasicMenu();

    // Extra file menu options
    wxMenu* filemenu=menubar->Remove(0);
    filemenu->Insert(2,new wxMenuItem(filemenu,id_filesave,"&Save","Save the script to disk."));
    filemenu->Insert(3,new wxMenuItem(filemenu,id_filesaveas,"Save &As","Save the script under a new filename."));
    filemenu->Insert(4,new wxMenuItem(filemenu,id_fileclose,"&Close","Close the script window."));
    menubar->Append(filemenu,"&File");

    wxMenu* editmenu=new wxMenu;
    editmenu->Append(id_editundo, "&Undo\tCtrl+Z","");
    editmenu->Append(id_editredo, "&Redo","");
    editmenu->AppendSeparator();
    editmenu->Append(id_editcopy, "&Copy\tCtrl+Ins","");
    editmenu->Append(id_editcut,  "C&ut\tShift+Del","");
    editmenu->Append(id_editpaste,"&Paste\tShift+Ins","");
    editmenu->Append(id_editselectall,"Select &All\tCtrl+a","");
    editmenu->AppendSeparator();
    editmenu->Append(id_editfind,"&Find...","");
    editmenu->Append(id_editreplace,"Replace...","");
    menubar->Append(editmenu,"&Edit");

    wxMenu* optionsmenu = new wxMenu;
    menubar->Append(optionsmenu,"&Options");
    optionsmenu->Append(id_optionsfont,"Co&lors...","");

    SetMenuBar(menubar);

    // --- Set up the text control ---
    pTextctrl=new wxStyledTextCtrl(this,id_ed,position,size,wxSTC_STYLE_INDENTGUIDE);

    // TODO: User syntax saving.  -- khross

    wxFont font(10, wxMODERN, wxNORMAL, wxNORMAL, false);
    pTextctrl->StyleSetFont(wxSTC_STYLE_DEFAULT, font);
    pTextctrl->StyleClearAll();

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
    pTextctrl->StyleSetItalic(1,true);

    pTextctrl->SetLexer(wxSTC_LEX_PYTHON);
    pTextctrl->SetKeyWords(0,
        "def lambda class return yield try raise except pass for while if else elif break continue "
        "global as import finally exec del print in is assert from and not or None"
        );

    // init the margins
    pTextctrl->SetMarginWidth       (linecountmargin,40);    // current line
    pTextctrl->SetMarginType        (linecountmargin,wxSTC_MARGIN_NUMBER);
    pTextctrl->SetMarginSensitive   (linecountmargin,false);

    pTextctrl->SetMarginWidth       (foldmargin,10);
    pTextctrl->SetMarginType        (foldmargin,wxSTC_MARGIN_SYMBOL);
    pTextctrl->SetMarginSensitive   (foldmargin,true);
    pTextctrl->SetMarginMask        (foldmargin,wxSTC_MASK_FOLDERS);
    pTextctrl->MarkerDefine         (wxSTC_MARKNUM_FOLDEROPEN,wxSTC_MARK_MINUS);
    pTextctrl->MarkerDefine         (wxSTC_MARKNUM_FOLDER,wxSTC_MARK_PLUS);
    pTextctrl->SetModEventMask      (wxSTC_MOD_CHANGEFOLD);
    pTextctrl->SetFoldFlags         (16);

    if (fname)
    {
        File f;
        if (f.OpenRead(fname))
        {
            int nSize=f.Size()+50;      // +50?  Don't ask.  I haven't the foggiest.
            char* c=new char[nSize];
            memset(c,0,nSize);

            f.Read(c,nSize);
            pTextctrl->InsertText(0,c);
            delete[] c;
            f.Close();

            sFilename=fname;
        } 
    }

    pTextctrl->EmptyUndoBuffer();
    pTextctrl->Show(true);
    pTextctrl->SetFocus();
}

CCodeWnd::~CCodeWnd()
{
    delete pTextctrl;
}

void CCodeWnd::SetSyntax(int nWhich, wxCommandEvent& event)
{
    // set script font/color/style
    //  -- khross

    if (nWhich==0) // whitespace
    {
        wxColour nColor;
        nColor=GetBackgroundColour();
        
        wxColourData hData;
        hData.SetColour(nColor);
        hData.SetChooseFull(true);

        for (int i=0; i<16; i++)
        {
            wxColour nCustom(i*16, i*16, i*16);
            hData.SetCustomColour(i,nCustom);
        }

        wxColourDialog cdialog(this, &hData);
        cdialog.SetTitle("Choose the background color");
        if (cdialog.ShowModal() == wxID_OK)
        {
            wxColourData retData = cdialog.GetColourData();
            nColor               = retData.GetColour();           

            pTextctrl->StyleSetBackground(nWhich,nColor);
            pTextctrl->StyleSetForeground(nWhich,nColor);
            SetBackgroundColour(nColor);
            
            

            for(int j=1; j<11; j++)
                pTextctrl->StyleSetBackground(j,nColor);

            
            pTextctrl->Show(true);
            pTextctrl->SetFocus();
            
        }
        return;
    }


    wxFontData f;
    wxFontDialog fontd(this,&f);

    if (fontd.ShowModal() == wxID_OK)
    {

        wxFontData retData  =   fontd.GetFontData();
        wxFont font         =   retData.GetChosenFont();
        wxColour color      =   retData.GetColour();

        pTextctrl->StyleSetFont(font.GetStyle(),font);
        pTextctrl->StyleSetForeground(nWhich,color);
        pTextctrl->StyleSetBold(nWhich,(font.GetStyle()==wxBOLD)?true:false);
        pTextctrl->StyleSetItalic(nWhich,(font.GetStyle()==wxITALIC)?true:false);
        pTextctrl->StyleSetUnderline(nWhich,font.GetUnderlined());
        pTextctrl->StyleSetSize(nWhich,font.GetPointSize());
        pTextctrl->StyleSetFaceName(nWhich,font.GetFaceName());
       
        pTextctrl->Show(true);
        pTextctrl->SetFocus();
    }
}

void CCodeWnd::OnSyntaxHighlighting(wxCommandEvent& event)
{
    const wxString szChoices[] = 
        {
        "Whitespace",
        "Comments",
        "Numerical constants",
        "\" Style string literals",
        "' Style string literals",
        "Keywords",
        "'' Style strings",
        "\"\" Style strings",
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
        SetSyntax(sdialog.GetSelection(),event);

    
}

void CCodeWnd::OnFind(wxCommandEvent& event)
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

void CCodeWnd::OnReplace(wxCommandEvent& event)
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

void CCodeWnd::DoFind(wxFindDialogEvent& event)
{
    // Handles find/replace stuff.
    // TODO: break this up, maybe.
    //   --  khross

    wxEventType type;

    type=event.GetEventType();
    if (type == wxEVT_COMMAND_FIND || type == wxEVT_COMMAND_FIND_NEXT)
    {

        int nLine=pTextctrl->FindText
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
            pTextctrl->SetSelection(nLine,nLine+(event.GetFindString().length()));
        }

    }

    else if (type == wxEVT_COMMAND_FIND_REPLACE || type == wxEVT_COMMAND_FIND_REPLACE_ALL)
    {
        int nLine=pTextctrl->FindText
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
            pTextctrl->SetSelection(nLine,nLine+(event.GetFindString().length()));
            pTextctrl->ReplaceSelection(event.GetReplaceString());

            if (type == wxEVT_COMMAND_FIND_REPLACE_ALL) DoFind(event);
        }
    }

    wxFindReplaceDialog *pDlg = event.GetDialog();
    pDlg->Destroy();

}



void CCodeWnd::OnStyleNeeded(wxStyledTextEvent& event)
{
    int currEndStyled = pTextctrl->GetEndStyled();
    pTextctrl->Colourise(currEndStyled, event.GetPosition());
}

void CCodeWnd::OnCharAdded(wxStyledTextEvent& event)
{
    static char linebuf[1000];
    char k=event.GetKey();

    if (k=='\n')
    {
        int nCurline=pTextctrl->GetCurrentLine();

        // go up until we find the last line with something on it, and remember how far it's indented.
        wxString s;
        int nLine=nCurline-1;
        while (nLine>0)
        {
            s=pTextctrl->GetLine(nLine);
            if (s.Len()==0)
                nLine--;
            else
            {
                // found one.  Now we simply copy all the spaces and tabs until we find a non-whitespace char
                for (unsigned int i=0; i<s.Len(); i++)
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
        s=pTextctrl->GetLine(nCurline-1).Trim();
        if (s.Right(4)[0]==':')
            strcat(linebuf,"\t");

        if (linebuf[0])
            pTextctrl->ReplaceSelection(linebuf);
    }
}

void CCodeWnd::OnSave(wxCommandEvent& event)
{
    if (sFilename.length()==0)
    {
        OnSaveAs(event);
        return;
    }

    File f;
    if (!f.OpenWrite(sFilename.c_str()))
        return;

    int nSize=pTextctrl->GetLength();

    wxString s=pTextctrl->GetText();
    f.Write(s.c_str(),nSize);
    
    f.Close();
}

void CCodeWnd::OnSaveAs(wxCommandEvent& event)
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

    int result=dlg.ShowModal();
    if (result==wxID_CANCEL)
        return;

    sFilename=dlg.GetFilename().c_str();
    SetTitle(sFilename.c_str());

    OnSave(event);
}

// Trivial stuff
void CCodeWnd::OnClose(wxCommandEvent& event)
{   
    File f;
    f.OpenRead(sFilename.c_str(),false);

    if (pTextctrl->GetLength()==0)
    {
        Destroy();
        return;
    }
    
    if (!sFilename.length() || f.Size()!=pTextctrl->GetLength())
    {
        wxMessageDialog msgdlg
            (
                this,
                "This file has been modified. Save?",
                "iked",
                wxYES_NO | wxCANCEL | wxICON_QUESTION,
                wxDefaultPosition
            );

        int nDecision=msgdlg.ShowModal();

        switch(nDecision)
        {

            case wxID_YES: OnSave(event); break;
            case wxID_CANCEL: return;
            //case wxID_NO: 

        }
    }
    
    Destroy();
}



void CCodeWnd::OnUndo(wxCommandEvent& event)        {   pTextctrl->Undo();      }
void CCodeWnd::OnRedo(wxCommandEvent& event)        {   pTextctrl->Redo();      }
void CCodeWnd::OnCopy(wxCommandEvent& event)        {   pTextctrl->Copy();      }
void CCodeWnd::OnCut(wxCommandEvent& event)         {   pTextctrl->Cut();       }
void CCodeWnd::OnPaste(wxCommandEvent& event)       {   pTextctrl->Paste();     }
void CCodeWnd::OnSelectAll(wxCommandEvent& event)   {   pTextctrl->SelectAll(); }
