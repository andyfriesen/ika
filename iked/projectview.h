#ifndef PROJECTVIEW_H
#define PROJECTVIEW_H

#include "types.h"
#include <wx\treectrl.h>
#include "docview.h"

class CMainWnd; // in main.cpp / h

class ProjectView : public wxTreeCtrl
{
private:
    wxMenu*      _fileMenu;
    wxMenu*      _folderMenu;
    wxMenu*      _rootMenu;

    CMainWnd*    _parent;

    wxImageList* _imageList;
    struct Leaf* _selected;  // The context menus use this. 

    bool         _changed;
    std::string  _fileName;

public:

    enum
    {
        id_filler = 100,
        id_treectrl,

        id_open,
        id_delete,
        id_rename,
        id_add,
        id_createsubfolder,
    };

    ProjectView(CMainWnd* parent, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize);
    ~ProjectView();

    void OnDoubleClick(wxMouseEvent& event);
    void OnRightClick(wxMouseEvent& event);
    void OnBeginEdit(wxTreeEvent& event);
    void OnEndEdit(wxTreeEvent& event);
    void OnBeginDrag(wxTreeEvent& event);
    void OnEndDrag(wxTreeEvent& event);
    void OnOpen(wxCommandEvent& event);
    void OnDelete(wxCommandEvent& event);
    void OnRename(wxCommandEvent& event);
    void OnAdd(wxCommandEvent& event);
    void OnCreateSubfolder(wxCommandEvent& event);

    // ------------------ Tree management ------------------

    struct Leaf* FindItem(const std::string& fileName);
    void MoveItem(const wxTreeItemId& id, const wxTreeItemId& newparent);
    wxTreeItemId AddFolder(const wxTreeItemId& parentid, const char* name);
    void AddItem(const wxTreeItemId& parentid, const string& name, const string& fname);

    // I/O
    void Save(const std::string& fname = "");
    void Load(const std::string& fname);
    void New(const std::string& fname);

    const std::string& GetFileName() const { return _fileName; }
    std::string GetProjectPath() const;

    DECLARE_EVENT_TABLE()
};

#endif