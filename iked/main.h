/*
    Main application window.
*/

#ifndef MAIN_H
#define MAIN_H

#include "types.h"
#include "wx/wx.h"
#include "wx/toolbar.h"
#include "wx/bitmap.h"
#include <set>

// Resource objects
#include "map.h"
#include "tileset.h"
#include "spriteset.h"
#include "controller.h"
#include "fontview.h"

class IDocView;
class ProjectView;

//////////////////////////////////////////

// TODO: make this user configurable?
enum FileType
{
    t_unknown,
    t_folder,
    t_project,
    t_script,
    t_vsp,
    t_chr,
    t_font,
    t_map,
    t_config,
    t_text,
    t_dat
};

//////////////////////////////////////////

class CApp : public wxApp
{
public:
    virtual bool OnInit();
};

class CMainWnd : public wxMDIParentFrame
{
    std::set < IDocView*> pDocuments;
    ProjectView* _project;

public:    
    CMainWnd(wxWindow* parent, const wxWindowID id, const wxString& title,
        const wxPoint& position, const wxSize& size, const long style);
    ~CMainWnd();
    
    void FileQuit(wxCommandEvent& event);
    void NewProject(wxCommandEvent& event);
    void NewMap(wxCommandEvent& event);
    void NewScript(wxCommandEvent& event);
    void NewSprite(wxCommandEvent& event);
    void NewTileSet(wxCommandEvent& event);
    void OnOpen(wxCommandEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnQuit(wxCloseEvent& event);

    void Open(const std::string& fname);
    void OpenDocument(IDocView* newwnd);

    IDocView* FindWindow(const void* rsrc) const;

    void OnSaveProject(wxCommandEvent& event);
    void OnSaveProjectAs(wxCommandEvent& event);

    void OnChildClose(IDocView* child);

    wxMenuBar*  CreateBasicMenu();
/*

    This is technically wrong, as std::vector doesn't stipulate that the entries be sequential.
    It merely promises random access at O(1).  Fix if it becomes an issue. (unlikely)

*/
    vector < wxAcceleratorEntry>  CreateBasicAcceleratorTable(); 

private:
    wxToolBar* CreateBasicToolBar();
    
    DECLARE_EVENT_TABLE()      
        
public:
/*
     public because I'm a lazy bitch and I don't feel like figuring out what classes should be friends with what other classes. ;P
     This window owns all the maps, VSPs, CHrs, etc... that get loaded.  It doles them out to child windows when they ask,
     and nukes things that no longer need to be in memory
*/

    CController < Map> map;
    CController < CTileSet> vsp;
    CController < CSpriteSet> spriteset;
    // TODO: fonts, scripts, (?) anything else that comes to mind

public:
    // "Helper" functions.
    FileType GetFileType(const std::string& fname);
};

#endif