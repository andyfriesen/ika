
#ifndef MAIN_H
#define MAIN_H

#include <string>
#include <wx/wx.h>

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
};

//////////////////////////////////////////

class CApp : public wxApp
{
public:
    virtual bool OnInit();
};

class CMainWnd : public wxMDIParentFrame
{
public:
    enum
    {
        id_filequit=1,
        id_filenewproject,
        id_filenewmap,
        id_filenewscript,
        id_fileopen,
    };
    
    CMainWnd(wxWindow* parent,const wxWindowID id,const wxString& title,
        const wxPoint& position,const wxSize& size,const long style);
    
    void FileQuit(wxCommandEvent& event);
    void NewProject(wxCommandEvent& event);
    void NewMap(wxCommandEvent& event);
    void NewScript(wxCommandEvent& event);
    void OnOpen(wxCommandEvent& event);
    void Bleh(wxCommandEvent& event);

    void Open(const std::string& fname);

    wxMenuBar*  CreateBasicMenu();
private:
    DECLARE_EVENT_TABLE()
        
        
private:
    // This window owns all the maps, VSPs, CHrs, etc... that get loaded.  It doles them out to child windows when they ask,
    // and nukes things that no longer need to be in memory
    /*
    CResourceController<Map> mapcontroller;
    CResourceController<VSP> vspcontroller;*/
    // TODO: fonts, sprites, scripts, anything else that comes to mind
public:
    // "Helper" functions.
    FileType GetFileType(const std::string& fname);
};

#endif