#include "projectview.h"
#include "main.h"


struct CLeaf : public wxTreeItemData
{
    std::string sName;                      // the filename
    FileType   type;                        // the kind of resource
    CLeaf(){}
    CLeaf(const CLeaf& l)
        : sName(l.sName),type(l.type)
    {}
    CLeaf(const std::string& n,FileType t)
        : sName(n),type(t)
    {}
};

//////////////////////////////////////////////////////////////////////

class CProjectTree : public wxTreeCtrl
{
    wxMenu* filemenu;
    wxMenu* foldermenu;

    CMainWnd*   pMainwnd;

    CLeaf*    pSelected;  // The context menus use this.

public:
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

    CProjectTree(CMainWnd* mainwnd,wxWindow* parent,int id,const wxPoint& pos,const wxSize& size,const long style)
        :   wxTreeCtrl(parent,id,pos,size,style)
    {
        pMainwnd=mainwnd;

        pSelected=0;
        
        filemenu=new wxMenu;
        filemenu->Append(id_open,"&Open","");
        filemenu->Append(id_delete,"&Delete","");
        filemenu->Append(id_rename,"&Rename","");
        
        foldermenu=new wxMenu;
        foldermenu->Append(id_add,"&Add","");
        foldermenu->Append(id_createsubfolder,"Create Sub&folder","");
        foldermenu->Append(id_delete,"&Delete","");
        foldermenu->Append(id_rename,"Rena&me","");
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
        wxTreeItemId id=HitTest(event.GetPosition(),flags);
        SelectItem(id);
        pSelected=(CLeaf*)GetItemData(id);
        
        if (pSelected->type==t_folder)
            PopupMenu(foldermenu,event.GetPosition());
        else
            PopupMenu(filemenu,event.GetPosition());
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

        MoveItem(srcid,destid);
    }
    
    void OnOpen(wxCommandEvent& event)
    {
        pMainwnd->Open(pSelected->sName.c_str());
    }
    
    void OnDelete(wxCommandEvent& event)
    {
        wxTreeItemId id=pSelected->GetId();
        
        Delete(id);
    }
    
    void OnRename(wxCommandEvent& event)
    {
        wxTreeItemId id=pSelected->GetId();
        
        EditLabel(id);
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
        
        for (int i=0; i<sNames.Count(); i++)
            AddItem(id,sNames[i].c_str(),sPaths[i].c_str());
        Expand(id);
        Refresh();
    }
    
    void OnCreateSubfolder(wxCommandEvent& event)
    {
        wxTreeItemId id=pSelected->GetId();
        
        AppendItem(id,"New Folder",0,0,new CLeaf("New Folder",t_folder));
        Expand(id);
        Refresh();
    }

    // ------------------ Tree management ------------------

    void MoveItem(const wxTreeItemId& id,const wxTreeItemId& newparent) // Moves a tree item, and all its children, to newparent
    {
        // copy the node
        wxTreeItemId newid = AppendItem(
            newparent,
            GetItemText(id),
            GetItemImage(id),
            GetItemSelectedImage(id),
            new CLeaf( *(CLeaf*) GetItemData(id) )
            );

        int nChildren=GetChildrenCount(id,false);

        if (nChildren)                                                  // recursively move stuff, if this item has children
        {
            long cookie=-1;
            wxTreeItemId iid;

            for (int i=0; i<nChildren; i++)
            {
                iid=GetFirstChild(id,cookie);                           // note that we keep nuking the first child, so don't bother with getnextchild
                MoveItem(iid,newid);
            }
        }

        // delete the old item
        Delete(id);
    }

    wxTreeItemId AddFolder(const wxTreeItemId& parentid,const char* name)
    {
        return AppendItem(parentid,name,0,0,new CLeaf( name, t_folder));
    }

    wxTreeItemId AddItem(const wxTreeItemId& parentid,const char* name,const char* fname)
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
        
        return AppendItem(parentid,name,hIcon,hIcon,new CLeaf( fname, t));
    }

    DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(CProjectTree,wxTreeCtrl)
    EVT_LEFT_DCLICK(CProjectTree::OnDoubleClick)
    EVT_RIGHT_DOWN(CProjectTree::OnRightClick)
    EVT_TREE_END_LABEL_EDIT(CProjectTree::id_treectrl,CProjectTree::OnEndEdit)
    EVT_TREE_BEGIN_DRAG(CProjectTree::id_treectrl,CProjectTree::OnBeginDrag)
    EVT_TREE_END_DRAG(CProjectTree::id_treectrl,CProjectTree::OnEndDrag)

    EVT_MENU(CProjectTree::id_open,CProjectTree::OnOpen)
    EVT_MENU(CProjectTree::id_delete,CProjectTree::OnDelete)
    EVT_MENU(CProjectTree::id_rename,CProjectTree::OnRename)
    EVT_MENU(CProjectTree::id_add,CProjectTree::OnAdd)
    EVT_MENU(CProjectTree::id_createsubfolder,CProjectTree::OnCreateSubfolder)
END_EVENT_TABLE()

//////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(CProjectWnd,wxMDIChildFrame)
    EVT_MENU(CProjectWnd::id_filesave,CProjectWnd::OnSave)
END_EVENT_TABLE()

CProjectWnd::CProjectWnd(CMainWnd* parent,const wxString& title,const wxPoint& position,const wxSize& size,const long style,const char* fname)
                        : wxMDIChildFrame(parent,-1,title,position,size,style)
{
    pTreectrl=new CProjectTree(parent,this,CProjectTree::id_treectrl,wxDefaultPosition,wxDefaultSize,wxTR_EDIT_LABELS | wxTR_HAS_BUTTONS);

    if (fname)
        Load(fname);
    else
    {
        wxTreeItemId root=pTreectrl->AddRoot("Project",-1,-1,new CLeaf("Project",t_folder));
        // default folders
        pTreectrl->AppendItem(root,"Scripts",0,0,new CLeaf("Scripts",t_folder));
        pTreectrl->AppendItem(root,"Maps"   ,0,0,new CLeaf("Maps",t_folder)   );
        pTreectrl->AppendItem(root,"Tilesets",0,0,new CLeaf("Tilesets",t_folder));
        pTreectrl->AppendItem(root,"Fonts"  ,0,0,new CLeaf("Fonts",t_folder)  );
        pTreectrl->AppendItem(root,"Sprites",0,0,new CLeaf("Sprites",t_folder));
    }

    // Icons
    pImagelist=new wxImageList(16,16,true);
    pImagelist->Add(wxIcon("foldericon",wxBITMAP_TYPE_ICO_RESOURCE));
    pImagelist->Add(wxIcon("pyicon"    ,wxBITMAP_TYPE_ICO_RESOURCE));
    pImagelist->Add(wxIcon("vspicon"   ,wxBITMAP_TYPE_ICO_RESOURCE));
    pImagelist->Add(wxIcon("fonticon"  ,wxBITMAP_TYPE_ICO_RESOURCE));
    pImagelist->Add(wxIcon("mapicon"   ,wxBITMAP_TYPE_ICO_RESOURCE));
    // chr
    pTreectrl->SetImageList(pImagelist);

    pTreectrl->Expand(pTreectrl->GetRootItem());
    pTreectrl->Show();

    wxMenuBar* menu=parent->CreateBasicMenu();
    wxMenu* filemenu=menu->Remove(0);
    filemenu->Insert(2,new wxMenuItem(filemenu,id_filesave,"&Save","Save the project."));
    menu->Append(filemenu,"&File");
    SetMenuBar(menu);

    Show();
}

CProjectWnd::~CProjectWnd()
{
    delete pTreectrl;
    delete pImagelist;
}

void CProjectWnd::OnSave(wxCommandEvent& event)
{
    wxFileDialog dlg(
        this,
        "Open File",
        "",
        "",
        "Project files (*.ikaprj)|*.ikaprj|"
        "All files (*.*)|*.*",
        wxSAVE
        );

    int result=dlg.ShowModal();
    if (result!=wxID_CANCEL)
        Save(dlg.GetFilename().c_str());
}

void CProjectWnd::Load(const char* fname)
{
    struct Local
    {
        static void ReadNode(FILE* f,CProjectTree* pTreectrl,const wxTreeItemId& parentid)
        {
            char name[100];
            char type[100];

            while (1)
            {
                fscanf(f,"%s",name);
                if (!stricmp(name,"END"))
                    break;

                fscanf(f,"%s",type);
                
                if (!stricmp(type,"FOLDER"))
                {
                    wxTreeItemId id;

                    if (parentid==-1)
                        id=pTreectrl->AddRoot("Project",-1,-1,new CLeaf("Project",t_folder));
                    else
                        id=pTreectrl->AddFolder(parentid,name);

                    ReadNode(f,pTreectrl,id);
                }
                else if (!stricmp(type,"FILE"))
                {
                    pTreectrl->AddItem(parentid,name,name);
                }

                if ( feof(f) )
                    break;
            }
        }
    };

    FILE* f=fopen(fname,"r");
    if (!f)
        return;

    pTreectrl->DeleteAllItems();
    Local::ReadNode(f,pTreectrl,-1);
    fclose(f);
}

void CProjectWnd::Save(const char* fname)
{
    struct Local
    {
        static void WriteIndentation(FILE* f,int amount)
        {
            char* c=new char[amount+1];
            memset(c,32,amount);
            c[amount]=0;
            fprintf(f,"%s",c);
            delete[] c;
        }

        static void WriteNode(FILE* f,CProjectTree* pTreectrl,wxTreeItemId id,int indentlevel=0)
        {
            CLeaf* pLeaf=(CLeaf*)pTreectrl->GetItemData(id);
            
            WriteIndentation(f,indentlevel);
            fprintf(f,"%s  ",pLeaf->sName.c_str());

            if (pLeaf->type!=t_folder)
                fprintf(f,"FILE\n");
            else
            {
                // wacky recursion
                fprintf(f,"FOLDER\n");

                int nChildren=pTreectrl->GetChildrenCount(id,false);
                if (nChildren)
                {
                    long l;
                    wxTreeItemId child=pTreectrl->GetFirstChild(id,l);
                    
                    int c=0;
                    while (c<nChildren)
                    {
                        WriteNode(f,pTreectrl,child,indentlevel+4);
                        c++;                        // YAY.  or something

                        child=pTreectrl->GetNextChild(id,l);
                    }
                }
                
                WriteIndentation(f,indentlevel);
                fprintf(f,"END\n");
            }
        }
    };

    // ---------

    FILE* f=fopen(fname,"w");
    if (!f)
        return;

    wxTreeItemId id=pTreectrl->GetRootItem();
    Local::WriteNode(f,pTreectrl,id);
    fclose(f);
}