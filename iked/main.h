/*
    Main application window.
*/

#ifndef MAIN_H
#define MAIN_H

#include "types.h"
#include <wx/wx.h>
#include <wx/toolbar.h>
#include <wx/bitmap.h>
#include <set>

// Resource objects
#include "map.h"
#include "tileset.h"
#include "spriteset.h"
#include "controller.h"
#include "fontview.h"

class IDocView;

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
    std::set<IDocView*> pDocuments;

public:
    enum
    {
        id_filequit=1,
        id_filenewproject,
        id_filenewmap,
        id_filenewscript,
        id_fileopen,

        id_tool,                // Have to keep this low.  IDs above 100 are reserved for individual application windows.  (though they may overlap with each other, they must not overlap with these)
        id_toolnewscript,
        id_toolnewmap,
        id_toolopen
        
    };
    
    CMainWnd(wxWindow* parent,const wxWindowID id,const wxString& title,
        const wxPoint& position,const wxSize& size,const long style);
    ~CMainWnd();
    
    void FileQuit(wxCommandEvent& event);
    void NewProject(wxCommandEvent& event);
    void NewMap(wxCommandEvent& event);
    void NewScript(wxCommandEvent& event);
    void OnOpen(wxCommandEvent& event);
    void OnQuit(wxCloseEvent& event);

    void Open(const std::string& fname);
    void OnToolBarNewScript(wxCommandEvent& event);
    void OnToolBarOpen(wxCommandEvent& event);
    void OnToolBarNewMap(wxCommandEvent& event);
    void OnToolLeftClick(wxCommandEvent& event);

    void OnChildClose(IDocView* child);

    wxMenuBar*  CreateBasicMenu();
/*

    This isn't quite as "correct" as it should be.
    Instead of using a C array, I stuff it in a std::vector, and assume that vectors store things
    in a sequential, continuous stream.  Should be okay, provided that the accelerator entry is copy
    safe. ^_^  Personally, I think it's quite elegant, but it is skirting the rules a bit.

*/
    vector<wxAcceleratorEntry>  CreateBasicAcceleratorTable(); 

private:
    wxToolBar* CreateBasicToolBar();
    
    DECLARE_EVENT_TABLE()      
        
public:
    // public because I'm a lazy bitch and I don't feel like figuring out what classes should be friends with what other classes. ;P
    // This window owns all the maps, VSPs, CHrs, etc... that get loaded.  It doles them out to child windows when they ask,
    // and nukes things that no longer need to be in memory

    CController<Map> map;
    CController<CTileSet> vsp;
    CController<CSpriteSet> spriteset;
    // TODO: fonts, sprites, scripts, anything else that comes to mind

public:
    // "Helper" functions.
    FileType GetFileType(const std::string& fname);
};

#endif