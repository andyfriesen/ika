#include "projectview.h"
#include "main.h"
#include "misc.h"

struct CLeaf : public wxTreeItemData
{
    string sName;                           // the filename
    FileType   type;                        // the kind of resource
    CLeaf()
    {}
    
    CLeaf(const CLeaf& l)
        : sName(l.sName), type(l.type)
    {}
    
    CLeaf(const string& n, FileType t)
        : sName(n), type(t)
    {}
};

//////////////////////////////////////////////////////////////////////

class CProjectTree : public wxTreeCtrl
{
    wxMenu* filemenu;
    wxMenu* foldermenu;
    wxMenu* rootmenu;

    CMainWnd*   pMainwnd;

    CLeaf*    pSelected;  // The context menus use this. 

    void SetChanged()
    {
        if (!bChanged)
            bChanged=true;
    }

public:

    bool bChanged; // bleh

    enum
    {
        id_filler=100,
        id_treectrl,

        id_open,
        id_delete,
        id_rename,
        id_add,
        id_createsubfolder,
    };

    CProjectTree(CMainWnd* mainwnd, wxWindow* parent, int id, const wxPoint& pos, const wxSize& size, const long style)
        :   wxTreeCtrl(parent, id, pos, size, style)
    {
        pMainwnd=mainwnd;

        pSelected=0;
        
        filemenu=new wxMenu;
        filemenu->Append(id_open, "&Open", "");
        filemenu->Append(id_delete, "&Delete", "");
        filemenu->Append(id_rename, "&Rename", "");
        
        foldermenu=new wxMenu;
        foldermenu->Append(id_add, "&Add", "");
        foldermenu->Append(id_createsubfolder, "Create Sub&folder", "");
        foldermenu->Append(id_delete, "&Delete", "");
        foldermenu->Append(id_rename, "Rena&me", "");

        rootmenu=new wxMenu;
        rootmenu->Append(id_add, "&Add", "");
        rootmenu->Append(id_createsubfolder, "Create Sub&folder", "");
        bChanged=false;
    }

    
    void OnDoubleClick(wxMouseEvent& event)
    {
        wxTreeItemId id=HitTest(event.GetPosition());
        SelectItem(id);
        CLeaf* pSel=(CLeaf*)GetItemData(id);

        if (pSel->type==t_folder)
            Expand(id);
        else
            pMainwnd->Open(pSel->sName);
    }

    void OnRightClick(wxMouseEvent& event)
    {
        int flags=0;
        wxTreeItemId id=HitTest(event.GetPosition(), flags);
        SelectItem(id);
        pSelected=(CLeaf*)GetItemData(id);
        if (!pSelected) return;

        if (id==GetRootItem())
            PopupMenu(rootmenu, event.GetPosition());
        else if (pSelected->type==t_folder)
            PopupMenu(foldermenu, event.GetPosition());
        else
            PopupMenu(filemenu, event.GetPosition());
    }

    void OnEndEdit(wxTreeEvent& event)
    {
        wxTreeItemId id=event.GetItem();
        CLeaf* pData=(CLeaf*)GetItemData(event.GetItem());

        pData->sName=event.GetLabel().c_str();
    }

    void OnBeginDrag(wxTreeEvent& event)
    {
        if (event.GetItem()==GetRootItem())
            return;

        pSelected=(CLeaf*)GetItemData(event.GetItem());
        event.Allow();
    }

    void OnEndDrag(wxTreeEvent& event)
    {
        wxTreeItemId destid=event.GetItem();
        wxTreeItemId srcid=pSelected->GetId();
        
        if (srcid==destid)
            return;

        CLeaf* pDest=(CLeaf*)GetItemData(destid);
        
        if (pDest->type!=t_folder && destid!=GetRootItem())
            destid=GetParent(destid);

        MoveItem(srcid, destid);
    }
    
    void OnOpen(wxCommandEvent& event)
    {
        pMainwnd->Open(pSelected->sName.c_str());
    }
    
    void OnDelete(wxCommandEvent& event)
    {
        wxTreeItemId id=pSelected->GetId();
        
        Delete(id);
        SetChanged();
    }
    
    void OnRename(wxCommandEvent& event)
    {
        wxTreeItemId id=pSelected->GetId();
        
        EditLabel(id);
        SetChanged();
    }

    void OnAdd(wxCommandEvent& event)
    {
        // TODO: check for redundant entries

        wxTreeItemId id=pSelected->GetId();

        wxFileDialog dlg(
            this,
            "Open File",
            "",
            "",
            "All known|*.py;*.map;*.vsp;*.chr;*.fnt|"
            "Python Scripts (*.py)|*.py|"
            "Maps (*.map)|*.map|"
            "VSP Tilesets (*.vsp)|*.vsp|"
            "Sprites (*.chr)|*.chr|"
            "Fonts (*.fnt)|*.fnt|"
            "All files (*.*)|*.*",
            wxOPEN | wxMULTIPLE
            );

        int result=dlg.ShowModal();
        if (result==wxID_CANCEL)
            return;

        // TODO: Get the lowest-common-denominator path, so the project file can be moved around easier
        wxArrayString sNames;
        wxArrayString sPaths;
        dlg.GetFilenames(sNames);
        dlg.GetPaths(sPaths);
        
        for (uint i=0; i<sNames.Count(); i++)
            AddItem(id, sNames[i].c_str(), sPaths[i].c_str());
        Expand(id);
        Refresh();
        SetChanged();
    }
    
    void OnCreateSubfolder(wxCommandEvent& event)
    {
        wxTreeItemId id=pSelected->GetId();
        
        AppendItem(id, "New Folder", 0, 0, new CLeaf("New Folder", t_folder));
        Expand(id);
        Refresh();
        SetChanged();
    }

    // ------------------ Tree management ------------------

    void MoveItem(const wxTreeItemId& id, const wxTreeItemId& newparent) // Moves a tree item, and all its children, to newparent
    {
        // copy the node
        wxTreeItemId newid = AppendItem(
            newparent,
            GetItemText(id),
            GetItemImage(id),
            GetItemSelectedImage(id),
            new CLeaf( *(CLeaf*) GetItemData(id) )
            );

        int nChildren=GetChildrenCount(id, false);

        if (nChildren)                                                  // recursively move stuff, if this item has children
        {
            long cookie=-1;
            wxTreeItemId iid;

            for (int i=0; i<nChildren; i++)
            {
                iid=GetFirstChild(id, cookie);                           // note that we keep nuking the first child, so don't bother with getnextchild
                MoveItem(iid, newid);
            }
        }

        // delete the old item
        Delete(id);
        SetChanged();
    }

    wxTreeItemId AddFolder(const wxTreeItemId& parentid, const char* name)
    {
        SetChanged();

        return AppendItem(parentid, name, 0, 0, new CLeaf( name, t_folder));
        
    }

    wxTreeItemId AddItem(const wxTreeItemId& parentid, const string& name, const string& fname)
    {
        FileType t=pMainwnd->GetFileType(fname);
        int hIcon;
        switch (t)
        {
        case t_script:  hIcon=1;    break;
        case t_vsp:     hIcon=2;    break;
        case t_font:    hIcon=3;    break;
        case t_map:     hIcon=4;    break;
        default:        hIcon=-1;   break;
        }

        SetChanged();

        return AppendItem(parentid, name.c_str(), hIcon, hIcon, new CLeaf( fname, t));
    }

    DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(CProjectTree, wxTreeCtrl)
    
    EVT_LEFT_DCLICK(CProjectTree::OnDoubleClick)
    EVT_RIGHT_DOWN(CProjectTree::OnRightClick)
    EVT_TREE_END_LABEL_EDIT(CProjectTree::id_treectrl, CProjectTree::OnEndEdit)
    EVT_TREE_BEGIN_DRAG(CProjectTree::id_treectrl, CProjectTree::OnBeginDrag)
    EVT_TREE_END_DRAG(CProjectTree::id_treectrl, CProjectTree::OnEndDrag)

    EVT_MENU(CProjectTree::id_open, CProjectTree::OnOpen)
    EVT_MENU(CProjectTree::id_delete, CProjectTree::OnDelete)
    EVT_MENU(CProjectTree::id_rename, CProjectTree::OnRename)
    EVT_MENU(CProjectTree::id_add, CProjectTree::OnAdd)
    EVT_MENU(CProjectTree::id_createsubfolder, CProjectTree::OnCreateSubfolder)
END_EVENT_TABLE()

//////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(CProjectView, wxMDIChildFrame)
    EVT_CLOSE(CProjectView::OnClose)
    EVT_MENU(CProjectView::id_filesave, CProjectView::OnSave)
    EVT_MENU(CProjectView::id_filesaveas, CProjectView::OnSaveAs)
END_EVENT_TABLE()

CProjectView::CProjectView(CMainWnd* parent, const string& name)
    : IDocView(parent, name)
{
    pTreectrl=new CProjectTree(parent, this, CProjectTree::id_treectrl, wxDefaultPosition, wxDefaultSize, wxTR_EDIT_LABELS | wxTR_HAS_BUTTONS);

    if (name.length())
    {
        Load(name.c_str());
        sName=name;
    }

    else
    {
        wxTreeItemId root=pTreectrl->AddRoot("Project", -1, -1, new CLeaf("Project", t_folder));
        // default folders
        pTreectrl->AppendItem(root, "Scripts", 0, 0, new CLeaf("Scripts", t_folder));
        pTreectrl->AppendItem(root, "Maps"   , 0, 0, new CLeaf("Maps", t_folder)   );
        pTreectrl->AppendItem(root, "Tilesets", 0, 0, new CLeaf("Tilesets", t_folder));
        pTreectrl->AppendItem(root, "Fonts"  , 0, 0, new CLeaf("Fonts", t_folder)  );
        pTreectrl->AppendItem(root, "Sprites", 0, 0, new CLeaf("Sprites", t_folder));
    }

    // Icons
    pImagelist=new wxImageList(16, 16, true);
    pImagelist->Add(wxIcon("foldericon", wxBITMAP_TYPE_ICO_RESOURCE));
    pImagelist->Add(wxIcon("pyicon"    , wxBITMAP_TYPE_ICO_RESOURCE));
    pImagelist->Add(wxIcon("vspicon"   , wxBITMAP_TYPE_ICO_RESOURCE));
    pImagelist->Add(wxIcon("fonticon"  , wxBITMAP_TYPE_ICO_RESOURCE));
    pImagelist->Add(wxIcon("mapicon"   , wxBITMAP_TYPE_ICO_RESOURCE));
    // chr
    pTreectrl->SetImageList(pImagelist);

    pTreectrl->Expand(pTreectrl->GetRootItem());
    pTreectrl->Show();

    wxMenuBar* menu=parent->CreateBasicMenu();
    wxMenu* filemenu=menu->Remove(0);
    filemenu->Insert(2, new wxMenuItem(filemenu, id_filesave, "&Save Project", "Save the project."));
    filemenu->Insert(3, new wxMenuItem(filemenu, id_filesaveas, "Save Project &As...", "Save the project with a new name."));
    menu->Append(filemenu, "&File");
    SetMenuBar(menu);

    Show();
}

CProjectView::~CProjectView()
{
    delete pTreectrl;
    delete pImagelist;
}

void CProjectView::OnSave(wxCommandEvent& event)
{
    if (sName.length()==0)
    {
        OnSaveAs(event);
        return;
    }

    Save(sName.c_str());
    SetTitle(sName.c_str());
}

void CProjectView::OnSaveAs(wxCommandEvent& event)
{
    wxFileDialog dlg
    (
        this,
        "Save Project",
        "",
        "",
        "Project files (*.ikaprj)|*.ikaprj|"
        "All files (*.*)|*.*",
        wxSAVE
    );

    int result=dlg.ShowModal();
    if (result!=wxID_CANCEL)
    {
        sName=dlg.GetPath().c_str();
        Save(sName.c_str());
        SetTitle(sName.c_str());
    }
    
}

void CProjectView::Load(const char* fname)
{
    struct Local
    {
        static void ReadNode(const string& sProjectroot, FILE* f, CProjectTree* pTreectrl, const wxTreeItemId& parentid)
        {
            char s[1024];           

            while (1)
            {
                if ( feof(f) )  break;

                fgets(s, 1024, f);
                std::string sLine=s;

                int p;
                p=sLine.find("END");
                if (p!=std::string::npos)
                    break;

                p=sLine.rfind("FOLDER");
                if (p!=std::string::npos)
                {
                    std::string sName=Trim(sLine.substr(0, p-1));

                    wxTreeItemId id;

                    if (parentid==-1)
                        id=pTreectrl->AddRoot("Project", -1, -1, new CLeaf("Project", t_folder));
                    else
                        id=pTreectrl->AddFolder(parentid, sName.c_str());

                    ReadNode(sProjectroot, f, pTreectrl, id);

                    continue;
                }

                p=sLine.rfind("FILE");
                if (p!=std::string::npos)
                {
                    string sName=Trim(sLine.substr(0, p-1));
                    string sPath=Path::Directory(sName, sProjectroot).substr(1); // nuke the leading slash

                    pTreectrl->AddItem(parentid, sPath+Path::Filename(sName), sName);
                }
            }
        }
    };

    string sProjectroot=Path::Directory(fname);
    
    FILE* f=fopen(fname, "r");
    if (!f)
        return;

    pTreectrl->DeleteAllItems();
    Local::ReadNode(sProjectroot, f, pTreectrl, -1);
    fclose(f);
    pTreectrl->bChanged=false;
}

void CProjectView::Save(const char* fname)
{
    struct Local
    {
        static void WriteIndentation(FILE* f, int amount)
        {
            char* c=new char[amount+1];
            memset(c, 32, amount);
            c[amount]=0;
            fprintf(f, "%s", c);
            delete[] c;
        }

        static void WriteNode(FILE* f, CProjectTree* pTreectrl, wxTreeItemId id, int indentlevel=0)
        {
            CLeaf* pLeaf=(CLeaf*)pTreectrl->GetItemData(id);
            
            WriteIndentation(f, indentlevel);
            fprintf(f, "%s  ", pLeaf->sName.c_str());

            if (pLeaf->type!=t_folder)
                fprintf(f, "FILE\n");
            else
            {
                // wacky recursion
                fprintf(f, "FOLDER\n");

                int nChildren=pTreectrl->GetChildrenCount(id, false);
                if (nChildren)
                {
                    long l;
                    wxTreeItemId child=pTreectrl->GetFirstChild(id, l);
                    
                    int c=0;
                    while (c<nChildren)
                    {
                        WriteNode(f, pTreectrl, child, indentlevel+4);
                        c++;                        // YAY.  or something

                        child=pTreectrl->GetNextChild(id, l);
                    }
                }
                
                WriteIndentation(f, indentlevel);
                fprintf(f, "END\n");
            }
        }
    };

    // ---------

    FILE* f=fopen(fname, "w");
    if (!f)
        return;

    wxTreeItemId id=pTreectrl->GetRootItem();
    Local::WriteNode(f, pTreectrl, id);
    fclose(f);
    pTreectrl->bChanged=false;
}

void CProjectView::OnClose(wxCommandEvent& event)
{
    if (pTreectrl->bChanged)
    {
        wxMessageDialog msgdlg
            (
                this,
                "This project has been modified. Save?",
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