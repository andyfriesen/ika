#include <wx/wx.h>
#include <wx/imaglist.h>
#include <wx/filename.h>

#include "projectview.h"
#include "main.h"
#include "misc.h"

struct Leaf : public wxTreeItemData
{
    std::string name;                       // the filename
    FileType    type;                       // the kind of resource
    Leaf()
    {
    }
    
    Leaf(const Leaf& l)
        : name(l.name)
        , type(l.type)
    {
    }
    
    Leaf(const std::string& n, FileType t)
        : name(n)
        , type(t)
    {
    }
};

//////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(ProjectView, wxTreeCtrl)
    
    EVT_LEFT_DCLICK(ProjectView::OnDoubleClick)
    EVT_RIGHT_DOWN(ProjectView::OnRightClick)
    //EVT_CONTEXT_MENU(ProjectView::OnContextMenu)
    EVT_TREE_BEGIN_LABEL_EDIT(ProjectView::id_treectrl, ProjectView::OnBeginEdit)
    EVT_TREE_END_LABEL_EDIT(ProjectView::id_treectrl, ProjectView::OnEndEdit)
    EVT_TREE_BEGIN_DRAG(ProjectView::id_treectrl, ProjectView::OnBeginDrag)
    EVT_TREE_END_DRAG(ProjectView::id_treectrl, ProjectView::OnEndDrag)

    EVT_MENU(ProjectView::id_open, ProjectView::OnOpen)
    EVT_MENU(ProjectView::id_delete, ProjectView::OnDelete)
    EVT_MENU(ProjectView::id_rename, ProjectView::OnRename)
    EVT_MENU(ProjectView::id_add, ProjectView::OnAdd)
    EVT_MENU(ProjectView::id_createsubfolder, ProjectView::OnCreateSubfolder)
END_EVENT_TABLE()

//////////////////////////////////////////////////////////////////////

ProjectView::ProjectView(CMainWnd* parent, const wxPoint& pos, const wxSize& size)
    :   wxTreeCtrl(parent, id_treectrl, pos, size, wxTR_HAS_BUTTONS | wxTR_EDIT_LABELS)
{
    _parent = parent;

    _selected = 0;
    
    _fileMenu = new wxMenu;
    _fileMenu->Append(id_open, "&Open", "");
    _fileMenu->Append(id_delete, "Re&move", "");
    _fileMenu->Append(id_rename, "&Rename", "");
    
    _folderMenu = new wxMenu;
    _folderMenu->Append(id_add, "&Add", "");
    _folderMenu->Append(id_createsubfolder, "Create Sub&folder", "");
    _folderMenu->Append(id_delete, "&Delete", "");
    _folderMenu->Append(id_rename, "Rena&me", "");

    _rootMenu = new wxMenu;
    _rootMenu->Append(id_add, "&Add", "");
    _rootMenu->Append(id_createsubfolder, "Create Sub&folder", "");
    _rootMenu->Append(id_rename, "Rena&me", "");
    _changed = false;

    // Icons
    const char* names[] = { "appicon", "foldericon", "folderopenicon", "scripticon", "vspicon", "fonticon", "mapicon", "spriteicon"  };
    const int nameCount = sizeof names / sizeof (char*);

    _imageList = new wxImageList(16, 16, true);
    
    for (int i = 0; i < nameCount; i++)
        _imageList->Add(wxIcon(names[i], wxBITMAP_TYPE_ICO_RESOURCE));
    SetImageList(_imageList);

    Show();
}

ProjectView::~ProjectView()
{
    delete _imageList;
}

void ProjectView::OnDoubleClick(wxMouseEvent& event)
{
    wxTreeItemId id = HitTest(event.GetPosition());
    SelectItem(id);
    Leaf* sel=(Leaf*)GetItemData(id);

    if (!sel)
        return;

    if (sel->type==t_folder)
        Expand(id);
    else
        _parent->Open(sel->name);
}

void ProjectView::OnRightClick(wxMouseEvent& event)
{
    int flags = 0;
    wxTreeItemId id;
    wxPoint pos = event.GetPosition();

    if (pos != wxPoint(-1, -1))
    {
        id = HitTest(event.GetPosition(), flags);
        SelectItem(id);
    }
    else    // menu key
    {
        id = GetSelection();
        wxRect r;
        GetBoundingRect(id, r);
        pos = r.GetPosition();
    }

    _selected = (Leaf*)GetItemData(id);
    if (!_selected) return;

    if (id == GetRootItem())
        PopupMenu(_rootMenu, pos);
    else if (_selected->type == t_folder)
        PopupMenu(_folderMenu, pos);
    else
        PopupMenu(_fileMenu, pos);
}

void ProjectView::OnBeginEdit(wxTreeEvent& event)
{
    wxTreeItemId id = event.GetItem();
    Leaf* leaf = (Leaf*)GetItemData(id);
    if (leaf->type == t_folder || leaf->type == t_project)
        event.Allow();
    else
        event.Veto();
}

void ProjectView::OnEndEdit(wxTreeEvent& event)
{
    wxTreeItemId id = event.GetItem();
    Leaf* leaf = (Leaf*)GetItemData(id);

    leaf->name = event.GetLabel().c_str();
}

void ProjectView::OnBeginDrag(wxTreeEvent& event)
{
    if (event.GetItem() == GetRootItem())
        return;

    _selected=(Leaf*)GetItemData(event.GetItem());
    event.Allow();
}

void ProjectView::OnEndDrag(wxTreeEvent& event)
{
    wxTreeItemId destid = event.GetItem();
    wxTreeItemId srcid = _selected->GetId();
    
    if (srcid==destid)
        return;

    Leaf* pDest=(Leaf*)GetItemData(destid);
    
    if (pDest->type!=t_folder && destid!=GetRootItem())
        destid = GetParent(destid);

    MoveItem(srcid, destid);
}

void ProjectView::OnOpen(wxCommandEvent& event)
{
    _parent->Open(_selected->name.c_str());
}

void ProjectView::OnDelete(wxCommandEvent& event)
{
    wxTreeItemId id = _selected->GetId();
    
    Delete(id);
    _changed = true;
}

void ProjectView::OnRename(wxCommandEvent& event)
{
    wxTreeItemId id = _selected->GetId();
    Leaf* leaf = (Leaf*)GetItemData(id);
    if (leaf->type == t_folder || leaf->type == t_project)    
    {
        EditLabel(id);
        _changed = true;
    }
}

void ProjectView::OnAdd(wxCommandEvent& event)
{
    // TODO: check for redundant entries

    wxTreeItemId id = _selected->GetId();

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

    int result = dlg.ShowModal();
    if (result==wxID_CANCEL)
        return;

    wxArrayString names;
    wxArrayString paths;
    dlg.GetFilenames(names);
    dlg.GetPaths(paths);
    
    for (uint i = 0; i < names.Count(); i++)
        AddItem(id, names[i].c_str(), paths[i].c_str());

    Expand(id);
    Refresh();
    _changed = true;
}

void ProjectView::OnCreateSubfolder(wxCommandEvent& event)
{
    wxTreeItemId id = _selected->GetId();
    
    AppendItem(id, "New Folder", 0, 0, new Leaf("New Folder", t_folder));
    Expand(id);
    Refresh();
    _changed = true;
}

Leaf* ProjectView::FindItem(const std::string& fileName)
{
    struct Local
    {
        static Leaf* FindItem(ProjectView* tree, const std::string& fileName, const wxTreeItemId& id)
        {
            long cookie;
            wxTreeItemId child = tree->GetFirstChild(id, cookie);

            while (child.IsOk())
            {
                Leaf* leaf = (Leaf*)tree->GetItemData(child);
                if (leaf->type == t_folder)
                {
                    leaf = FindItem(tree, fileName, child);
                    if (leaf != 0)
                        return leaf;
                }
                else if (leaf->type != t_project && leaf->name == fileName)
                    return leaf;

                child = tree->GetNextChild(child, cookie);
            }

            return 0;
        }
    };
    
    wxFileName name(fileName.c_str());
    std::string p = GetProjectPath();
    if (name.IsAbsolute())
        name.MakeRelativeTo(GetProjectPath().c_str());
    std::string n = name.GetFullPath().c_str();

    return Local::FindItem(this, name.GetFullPath().c_str(), GetRootItem());
}

void ProjectView::MoveItem(const wxTreeItemId& id, const wxTreeItemId& newparent) // Moves a tree item, and all its children, to newparent
{
    // copy the node
    wxTreeItemId newid = AppendItem(
        newparent,
        GetItemText(id),
        GetItemImage(id),
        GetItemSelectedImage(id),
        new Leaf( *(Leaf*) GetItemData(id) )
        );

    int nChildren = GetChildrenCount(id, false);

    if (nChildren)                                                  // recursively move stuff, if this item has children
    {
        long cookie=-1;
        wxTreeItemId iid;

        for (int i = 0; i < nChildren; i++)
        {
            iid = GetFirstChild(id, cookie);                           // note that we keep nuking the first child, so don't bother with getnextchild
            MoveItem(iid, newid);
        }
    }

    // delete the old item
    Delete(id);
    _changed = true;
}

wxTreeItemId ProjectView::AddFolder(const wxTreeItemId& parentid, const char* name)
{
    _changed = true;

    return AppendItem(parentid, name, 1, 2, new Leaf( name, t_folder));
}

void ProjectView::AddItem(const wxTreeItemId& parentid, const std::string& name, const std::string& fname)
{
    Leaf* leaf = FindItem(fname);
    if (leaf != 0)
        return;

    FileType t = _parent->GetFileType(fname);
    int hIcon;
    switch (t)
    {
    case t_script:  hIcon = 3;  break;
    case t_vsp:     hIcon = 4;  break;
    case t_font:    hIcon = 5;  break;
    case t_map:     hIcon = 6;  break;
    case t_chr:     hIcon = 7;  break;
    default:        hIcon = -1; break;
    }

    _changed = true;

    AppendItem(parentid, name.c_str(), hIcon, hIcon, new Leaf( name, t));
}

void ProjectView::Save(const std::string& fname)
{
    struct Local
    {
        static void WriteIndentation(FILE* f, const int amount)
        {
            char* c = new char[amount + 1];
            memset(c, 32, amount);
            c[amount]=0;
            fprintf(f, "%s", c);
            delete[] c;
        }

        static void WriteNode(ProjectView* tree, const wxTreeItemId& item, FILE* f, const int indent = 0)
        {
            Leaf* leaf = (Leaf*)tree->GetItemData(item);
            WriteIndentation(f, indent);

            if (leaf->type == t_folder)
            {
                fprintf(f, "%s   FOLDER\n", leaf->name.c_str());

                long cookie;
                wxTreeItemId child = tree->GetFirstChild(item, cookie);

                while (child.IsOk())
                {
                    WriteNode(tree, child, f, indent + 4);
                    child = tree->GetNextSibling(child);
                }

                WriteIndentation(f, indent);
                fprintf(f, "END\n");
            }
            else
            {
                wxFileName name(leaf->name.c_str());
                if (name.IsAbsolute())
                    name.MakeRelativeTo(tree->GetProjectPath().c_str());

                fprintf(f, "%s   FILE\n", name.GetFullPath().c_str());
            }
        }
    };

    // ---------

    if (!fname.empty())
        _fileName = fname;

    FILE* f = fopen(_fileName.c_str(), "w");
    if (!f) return;

    Local::WriteNode(this, GetRootItem(), f);
    fclose(f);
    _changed = false;
}

void ProjectView::Load(const std::string& fname)
{
    struct Local
    {
        static void ReadNode(FILE* f, ProjectView* tree, const wxTreeItemId& parentid)
        {
            char s[1024];           

            while (1)
            {
                if ( feof(f) )  break;

                fgets(s, 1024, f);
                std::string sLine = s;

                int p;
                p = sLine.find("END");
                if (p!=std::string::npos)
                    break;

                p = sLine.rfind("FOLDER");
                if (p!=std::string::npos)
                {
                    std::string name = Trim(sLine.substr(0, p - 1));

                    wxTreeItemId id;

                    if (!tree->GetRootItem().IsOk())
                        id = tree->AddRoot(name.c_str(), 0, 0, new Leaf(name.c_str(), t_folder));
                    else
                        id = tree->AddFolder(parentid, name.c_str());

                    ReadNode(f, tree, id);

                    continue;
                }

                p = sLine.rfind("FILE");
                if (p!=std::string::npos)
                {
                    std::string name = Trim(sLine.substr(0, p - 1));

                    wxFileName fullName(name.c_str());
                    if (fullName.IsAbsolute())
                        fullName.InsertDir(0, tree->GetProjectPath().c_str());

                    tree->AddItem(parentid, /*Path::Filename(name)*/ fullName.GetFullPath().c_str(), fullName.GetFullPath().c_str());
                }
            }
        }
    };

    wxFileName::SetCwd(wxFileName(fname.c_str()).GetPath());
    
    _fileName = fname;
    FILE* f = fopen(_fileName.c_str(), "r");
    if (!f)
        return;

    DeleteAllItems();

    Local::ReadNode(f, this, -1);
    fclose(f);
    _changed = false;
    
    Expand(GetRootItem());
}

void ProjectView::New(const std::string& fname)
{
    DeleteAllItems();

    wxTreeItemId root = AddRoot("Project", 0, 0, new Leaf("Project", t_folder));
    // default folders
    AddFolder(root, "Scripts");
    AddFolder(root, "Maps");
    AddFolder(root, "Tilesets");
    AddFolder(root, "Fonts");
    AddFolder(root, "Sprites");
    Expand(root);

    Save(fname);
}

std::string ProjectView::GetProjectPath() const
{
    if (_fileName.empty())
        return (wxFileName::GetCwd() + "/").c_str();
    else
    {
        wxFileName projectPath(_fileName.c_str());
        //projectPath.MakeRelativeTo(wxFileName::GetCwd());
        return projectPath.GetPath(true).c_str();
    }
}