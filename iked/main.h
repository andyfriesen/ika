/*
    Main application window.
*/

#pragma once

#include "common/utility.h"
#include "wx/wx.h"
#include "wx/toolbar.h"
#include "wx/bitmap.h"
#include <set>

// Resource objects
#include "common/map.h"
#include "tileset.h"
#include "spriteset.h"
#include "controller.h"
#include "fontview.h"

//////////////////////////////////////////

namespace iked {

    // TODO: make this user configurable?
    enum FileType {
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

    struct DocumentPanel;
    struct ProjectView;
    struct TileSetPanel;

    //////////////////////////////////////////

    struct DocumentFactory {
        virtual std::string getName() = 0;
        virtual std::string getExtension() = 0;
        //virtual DocumentResource* createResource() = 0;
        //virtual DocumentResource* createResource(const std::string& fileName) = 0;
        virtual DocumentPanel* createPanel(MainWindow* parent, const std::string& filename) = 0;
    };

    struct MainWindow : public wxMDIParentFrame {
        std::set<DocumentPanel*> pDocuments;
        ProjectView* _project;

    public:    
        MainWindow(const std::vector<std::string>& args);
        ~MainWindow();
        
        void FileQuit(wxCommandEvent& event);
        void FileNew(wxCommandEvent& event);
        /*void NewProject(wxCommandEvent& event);
        void NewMap(wxCommandEvent& event);
        void NewScript(wxCommandEvent& event);
        void NewTileSet(wxCommandEvent& event);*/
        void NewSprite(wxCommandEvent& event);
        void OnOpen(wxCommandEvent& event);
        void OnSize(wxSizeEvent& event);
        void OnQuit(wxCloseEvent& event);

        void Open(const std::string& fname);
        void OpenDocument(DocumentPanel* newwnd);

        DocumentPanel* FindWindow(const void* rsrc) const;

        void OnSaveProject(wxCommandEvent& event);
        void OnSaveProjectAs(wxCommandEvent& event);

        void OnChildClose(DocumentPanel* child);

        wxMenuBar* CreateBasicMenu();

        /*
         * This is technically wrong, I think: std::vector doesn't stipulate that the elements
         * be sequential.  It merely promises random access at O(1).  
         * Fix if it becomes an issue. (unlikely)
         */
        std::vector<wxAcceleratorEntry>  CreateBasicAcceleratorTable(); 

    private:
        wxToolBar* CreateBasicToolBar();
        
        DECLARE_EVENT_TABLE()      

    public:
        std::map<std::string, DocumentFactory*> knownDocumentTypes;

        /*
         * public because I'm a lazy bitch and I don't feel like figuring out what classes should be friends with what other classes. ;P
         * This window owns all the maps, tilesets, sprites, etc... that get loaded.  It doles them out to child windows when they ask,
         * and nukes things that no longer need to be in memory
         */

        //Controller<Map> map;
        //Controller<Tileset> vsp;
        //Controller<SpriteSet> spriteset;
        // TODO: fonts, scripts, (?) anything else that comes to mind

    public:
        // "Helper" functions.
        FileType GetFileType(const std::string& fname);
    };

}
