#include "wxinc.h"
#include <wx/laywin.h>
#include <wx/filename.h>

#include <vector>
#include <sstream>

#include "mainwindow.h"

// Important UI element things
#include "mapview.h"
#include "tilesetview.h"
#include "layerlist.h"

// Dialogs
#include "newmapdlg.h"
#include "mapdlg.h"
#include "layerdlg.h"
#include "importtilesdlg.h"
#include "scriptdlg.h"
#include "tileanimdlg.h"

// Resources
#include "map.h"
#include "tileset.h"
#include "spriteset.h"
#include "canvas.h"

// Other stuff
#include "command.h"
#include "events.h"
#include "misc.h"

// Scripting
#include "scriptengine.h"
#include "script.h"

#define VERTICAL_FUN

namespace
{
    enum
    {
        id_filenew,
        id_fileopen,
        id_filesave,
        id_filesavemapas,
        id_fileloadtileset,
        id_filesavetilesetas,
        id_fileexporttiles,
        id_fileexit,

        id_editundo,
        id_editredo,
        id_editmapproperties,
        id_importtiles,
        id_edittileanim,
        id_clonelayer,

        id_zoommapin,
        id_zoommapout,
        id_zoommapnormal,
        id_zoomtilesetin,
        id_zoomtilesetout,
        id_zoomtilesetnormal,

        id_configurescripts,

        id_cursorup,
        id_cursordown,
        id_cursorleft,
        id_cursorright,

        id_tilepaint,
        id_copypaste,
        id_obstructionedit,
        id_zoneedit,
        id_waypointedit,
        id_entityedit,
        id_scripttool,

        id_newlayer,
        id_destroylayer,
        id_movelayerup,
        id_movelayerdown,

        id_layerlist,
        id_topbar,
        id_bottombar,
        id_sidebar,

        id_scriptlist,
        id_customscript,
        id_lastcustomscript = id_customscript + 1000, // Hopefully a reasonable limit. (sucks that I need a limit at all, but what can you do)
                                                     // (aside from pay more attention to the wx documentation, that is)
        id_dummy
    };
}

BEGIN_EVENT_TABLE(MainWindow, wxFrame)
    //EVT_CLOSE(MainWindow::OnClose)
    EVT_SIZE(MainWindow::OnSize)
    EVT_SASH_DRAGGED(id_bottombar, MainWindow::OnDragSash)
    EVT_SASH_DRAGGED(id_sidebar, MainWindow::OnDragSash)

    EVT_MENU(id_filenew, MainWindow::OnNewMap)
    EVT_MENU(id_fileopen, MainWindow::OnOpenMap)
    EVT_MENU(id_filesave, MainWindow::OnSaveMap)
    EVT_MENU(id_filesavemapas, MainWindow::OnSaveMapAs)
    EVT_MENU(id_fileloadtileset, MainWindow::OnLoadTileSet)
    EVT_MENU(id_filesavetilesetas, MainWindow::OnSaveTileSetAs)
    EVT_MENU(id_fileexit, MainWindow::OnExit)
    EVT_MENU(id_editundo, MainWindow::OnUndo)
    EVT_MENU(id_editredo, MainWindow::OnRedo)
    EVT_MENU(id_editmapproperties, MainWindow::OnEditMapProperties)
    EVT_MENU(id_importtiles, MainWindow::OnImportTiles)
    EVT_MENU(id_clonelayer, MainWindow::OnCloneLayer)
    EVT_MENU(id_edittileanim, MainWindow::OnEditTileAnim)

    EVT_MENU(id_zoommapin, MainWindow::OnZoomMapIn)
    EVT_MENU(id_zoommapout, MainWindow::OnZoomMapOut)
    EVT_MENU(id_zoommapnormal, MainWindow::OnZoomMapNormal)
    EVT_MENU(id_zoomtilesetin, MainWindow::OnZoomTileSetIn)
    EVT_MENU(id_zoomtilesetout, MainWindow::OnZoomTileSetOut)
    EVT_MENU(id_zoomtilesetnormal, MainWindow::OnZoomTileSetNormal)

    EVT_MENU(id_configurescripts, MainWindow::OnConfigureScripts)
    EVT_MENU_RANGE(id_customscript, id_lastcustomscript, MainWindow::OnSetCurrentScript)

    EVT_MENU(id_cursorup, MainWindow::OnCursorUp)
    EVT_MENU(id_cursordown, MainWindow::OnCursorDown)
    EVT_MENU(id_cursorleft, MainWindow::OnCursorLeft)
    EVT_MENU(id_cursorright, MainWindow::OnCursorRight)

    EVT_BUTTON(id_tilepaint, MainWindow::OnSetTilePaintState)
    EVT_BUTTON(id_copypaste, MainWindow::OnSetCopyPasteState)
    EVT_BUTTON(id_obstructionedit, MainWindow::OnSetObstructionState)
    EVT_BUTTON(id_zoneedit, MainWindow::OnSetZoneState)
    EVT_BUTTON(id_entityedit, MainWindow::OnSetEntityState)
    EVT_BUTTON(id_scripttool, MainWindow::OnSetScriptTool)

    EVT_BUTTON(id_newlayer, MainWindow::OnNewLayer)
    EVT_BUTTON(id_destroylayer, MainWindow::OnDestroyLayer)
    EVT_BUTTON(id_movelayerup, MainWindow::OnMoveLayerUp)
    EVT_BUTTON(id_movelayerdown, MainWindow::OnMoveLayerDown)
END_EVENT_TABLE()

void MainWindow::ClearList(std::stack<::Command*>& list)
{
    while (!list.empty())
    {
        ::Command* c = list.top();
        delete c;
        list.pop();
    }
}

MainWindow::MainWindow(const wxPoint& position, const wxSize& size, const long style)
    : wxFrame(0, -1, va("ikaMap version %s", IKA_VERSION), position, size, style)
    , _map(0)
    , _tileSet(0)
    , _curScript(0)
    , _curLayer(0)
    , _curTile(0)
    , _changed(false)
    , _layerVisibility(20)
{
    SetIcon(wxIcon("appicon", wxBITMAP_TYPE_ICO_RESOURCE));

    const int widths[] = { 100, -1, 100 };
    CreateStatusBar(lengthof(widths));
    _statusBar = GetStatusBar();
    _statusBar->SetStatusWidths(lengthof(widths), widths);

    _sideBar = new wxSashLayoutWindow(this, id_sidebar, wxDefaultPosition, wxDefaultSize, wxCLIP_CHILDREN);
    _sideBar->SetAlignment(wxLAYOUT_LEFT);
    _sideBar->SetOrientation(wxLAYOUT_VERTICAL);
    _sideBar->SetSashVisible(wxSASH_RIGHT, true);
    _sideBar->SetDefaultSize(wxSize(150, 1000));

    wxPanel* sidePanel = new wxPanel(_sideBar);
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

    struct ToolButton
    {
        const char* iconName;
        uint id;
        const char* toolTip;
    } toolButtons[] = 
    {
        {   "brushicon",        id_tilepaint,       "Place individual tiles on the map."                            },
        {   "selecticon",       id_copypaste,       "Select a group of tiles, and duplicate them elsewhere."        },
        {   "obstructionicon",  id_obstructionedit, "Edit obstructed areas."                                        },
        {   "zoneicon",         id_zoneedit,        "Edit zones."                                                   },
        {   "waypointicon",     id_waypointedit,    "Edit waypoints."                                               },
        {   "entityicon",       id_entityedit,      "Edit entities."                                                },
        {   "newicon",          id_newlayer,        "Create a brand new layer."                                     },
        {   "trashicon",        id_destroylayer,    "Destroy the currently selected layer."                         },
        {   "uparrowicon",      id_movelayerup,     "Move the current layer upwards. (beneath the previous layer)"  },
        {   "downarrowicon",    id_movelayerdown,   "Move the current layer downwards. (above the next layer)"      },
        {   "scripticon",       id_scripttool,      "Use a script tool."                                            },
    };
    // blah blah blah can't use templates with local structs gay gay gay blah gay. (I cry myself to sleep ;_;)
    const int numToolButtons = sizeof toolButtons / sizeof toolButtons[0];

    {
        wxSizer* miniSizer = new wxGridSizer(6);

        for (uint i = 0; i < numToolButtons; i++)
        {
            wxBitmapButton* b = new wxBitmapButton(
                sidePanel, 
                toolButtons[i].id, 
                wxIcon(toolButtons[i].iconName, wxBITMAP_TYPE_ICO_RESOURCE, 16, 16),
                wxDefaultPosition,
                wxDefaultSize,
                wxBU_EXACTFIT | wxBU_AUTODRAW
                );

            b->SetToolTip(toolButtons[i].toolTip);

            miniSizer->Add(b);
        }
        
        sizer->Add(miniSizer);
    }

    _layerList = new LayerList(this, sidePanel);
    sizer->Add(_layerList, 1, wxEXPAND);
    sidePanel->SetSizer(sizer);

    // GAY.  TODO: make things floatable, so this can vanish.
    _topBar = new wxSashLayoutWindow(this, id_topbar);
#ifdef VERTICAL_FUN
    _topBar->SetAlignment(wxLAYOUT_LEFT);
    _topBar->SetOrientation(wxLAYOUT_VERTICAL);
#else
    _topBar->SetAlignment(wxLAYOUT_TOP);
    _topBar->SetOrientation(wxLAYOUT_HORIZONTAL);
#endif

    _bottomBar = new wxSashLayoutWindow(this, id_bottombar);
    _bottomBar->SetDefaultSize(wxSize(330, 100));
#ifdef VERTICAL_FUN
    _bottomBar->SetAlignment(wxLAYOUT_RIGHT);
    _bottomBar->SetOrientation(wxLAYOUT_VERTICAL);
    _bottomBar->SetSashVisible(wxSASH_LEFT, true);
#else
    _bottomBar->SetAlignment(wxLAYOUT_BOTTOM);
    _bottomBar->SetOrientation(wxLAYOUT_HORIZONTAL);
    _bottomBar->SetSashVisible(wxSASH_TOP, true);
#endif

    _mapView = new MapView(this, _topBar);
    _tileSetView = new TileSetView(this, _bottomBar);

    _map = new Map;
    _tileSet = new TileSet;
    _tileSet->New(16, 16);

    /*
     * Time to bind components to events:
     * Basically, we're just hooking render/refresh methods up
     * to everything. :P
     * The event granularity is mainly so that actions that alter
     * more than one thing fire exactly one event.
     */
    {
        tilesSet.add            (_mapView, MapView::OnMapChange);
        obsSet.add              (_mapView, MapView::OnMapChange);
        zonesChanged.add        (_mapView, MapView::OnMapChange);

        layerCreated.add        (_mapView, MapView::OnMapChange);
        layerCreated.add        (_layerList, LayerList::OnMapLayersChanged);
        
        layerDestroyed.add      (_mapView, MapView::OnMapChange);
        layerDestroyed.add      (_layerList, LayerList::OnMapLayersChanged);

        layersReordered.add     (_mapView, MapView::OnMapChange);
        layersReordered.add     (_layerList, LayerList::OnMapLayersChanged);

        layerPropertiesChanged.add(_mapView, MapView::OnMapChange);
        layerPropertiesChanged.add(_layerList, LayerList::OnMapLayersChanged);

        layerResized.add        (_mapView, MapView::OnMapChange);

        entitiesChanged.add     (_mapView, MapView::OnMapChange);
        mapPropertiesChanged.add(_mapView, MapView::OnMapChange);

        tilesImported.add       (_tileSetView, TileSetView::OnTileSetChange);
        tileSetChanged.add      (_tileSetView, TileSetView::OnTileSetChange);

        mapLoaded.add           (bind(_mapView, MapView::OnMapChange));
        mapLoaded.add           (bind(_tileSetView, TileSetView::OnTileSetChange));
        mapLoaded.add           (bind(_layerList, LayerList::OnMapLayersChanged));

        mapVisibilityChanged.add(_mapView, MapView::OnMapChange);
        mapVisibilityChanged.add(_layerList, LayerList::OnVisibilityChanged);

        curLayerChanged.add     (_mapView, MapView::OnCurLayerChange);
        curLayerChanged.add     (_layerList, LayerList::OnLayerActivated);

        curTileChanged.add      (_tileSetView, TileSetView::OnCurrentTileChange);
    }

    // Create the menu.
    {
        wxMenu* fileMenu = new wxMenu;
        fileMenu->Append(id_filenew, "&New Map\tCtrl-N", "Open a fresh, blank map.");
        fileMenu->Append(id_fileopen, "&Open Map\tCtrl-O", "Open an existing map.");
        fileMenu->Append(id_filesave, "&Save\tCtrl-S", "Save the current map and tileset to disk.");
        fileMenu->Append(id_filesavemapas, "Save Map &As...", "Save the current map under a new filename.");
        fileMenu->AppendSeparator();
        fileMenu->Append(id_fileloadtileset, "&Load Tileset...", "Replace the map's tileset with a different, pre-existing tileset.");
        fileMenu->Append(id_filesavetilesetas, "Save &Tileset As...", "Save the current tileset under a new name.");
        fileMenu->AppendSeparator();
        fileMenu->Append(id_fileexit, "E&xit\tCtrl-Alt-Del", "Quit ikaMap");

        wxMenu* editMenu = new wxMenu;
        editMenu->Append(id_editundo, "&Undo\tCtrl-Z", "Undo the last action.");
        editMenu->Append(id_editredo, "&Redo\tCtrl-Y", "Redo the last undone action.");
        editMenu->AppendSeparator();
        editMenu->Append(id_editmapproperties, "Map &Properties...", "Edit the map's title, and dimensions.");
        editMenu->Append(id_importtiles, "Import &Tiles...", "Grab one or more tiles from an image file.");
        editMenu->Append(id_edittileanim, "Edit Tile &Animations...", "");
        editMenu->AppendSeparator();
        editMenu->Append(id_clonelayer, "Clone Layer", "Create a copy of the current layer.");

        wxMenu* viewMenu = new wxMenu;
        viewMenu->Append(id_zoommapin, "Zoom Map In\t+");
        viewMenu->Append(id_zoommapout, "Zoom Map Out\t-");
        viewMenu->Append(id_zoommapnormal, "Zoom Map to 100%\t=", "Stop zooming the map.");
        viewMenu->AppendSeparator();
        viewMenu->Append(id_zoomtilesetin, "Zoom Tileset In\tNumpad 8");
        viewMenu->Append(id_zoomtilesetout, "Zoom Tileset Out\tNumpad 2");
        viewMenu->Append(id_zoomtilesetnormal, "Zoom Tileset to 100%\tNumpad 5", "Stop zooming on the tileset.");

        wxMenu* toolMenu = new wxMenu;
        toolMenu->Append(id_configurescripts, "Configure Plugin &Scripts...", "Load and unload Python scripts");
        wxMenu* scriptMenu = new wxMenu;
        toolMenu->Append(id_scriptlist, "Scripts", scriptMenu);    // we fill this submenu later

        wxMenuBar* menuBar = new wxMenuBar;
        menuBar->Append(fileMenu, "&File");
        menuBar->Append(editMenu, "&Edit");
        menuBar->Append(viewMenu, "&View");
        menuBar->Append(toolMenu, "&Tools");
        SetMenuBar(menuBar);
    }

    // Set up hotkey stuff.
    const int tableSize = 15;
    int i = 0;
    wxAcceleratorEntry entries[tableSize]; // bleh
    entries[i++].Set(wxACCEL_CTRL, (int)'N', id_filenew);
    entries[i++].Set(wxACCEL_CTRL, (int)'O', id_fileopen);
    entries[i++].Set(wxACCEL_CTRL, (int)'S', id_filesave);
    entries[i++].Set(wxACCEL_CTRL, (int)'Z', id_editundo);
    entries[i++].Set(wxACCEL_CTRL, (int)'Y', id_editredo);
    entries[i++].Set(wxACCEL_NORMAL, WXK_ADD, id_zoommapin);
    entries[i++].Set(wxACCEL_NORMAL, WXK_SUBTRACT, id_zoommapout);
    entries[i++].Set(wxACCEL_NORMAL, (int)'=', id_zoommapnormal);
    entries[i++].Set(wxACCEL_NORMAL, WXK_NUMPAD8, id_zoomtilesetin);
    entries[i++].Set(wxACCEL_NORMAL, WXK_NUMPAD2, id_zoomtilesetout);
    entries[i++].Set(wxACCEL_NORMAL, WXK_NUMPAD5, id_zoomtilesetnormal);
    entries[i++].Set(wxACCEL_NORMAL, WXK_UP, id_cursorup);
    entries[i++].Set(wxACCEL_NORMAL, WXK_DOWN, id_cursordown);
    entries[i++].Set(wxACCEL_NORMAL, WXK_LEFT, id_cursorleft);
    entries[i++].Set(wxACCEL_NORMAL, WXK_RIGHT, id_cursorright);
    wxAcceleratorTable table(tableSize, entries);
    SetAcceleratorTable(table);

    UpdateTitle();
    UpdateScriptMenu();

    ::wxSafeYield();

    ScriptEngine::Init(this);
}

MainWindow::~MainWindow()
{
    for (SpriteMap::iterator iter = _sprites.begin(); iter != _sprites.end(); iter++)
        delete iter->second;
    _sprites.clear();

    ClearList(_undoList);
    ClearList(_redoList);

    delete _map;
    delete _tileSet;

    for (uint i = 0; i < _scripts.size(); i++)
        delete _scripts[i];
    _scripts.clear();

    ScriptEngine::ShutDown();
}

void MainWindow::OnClose(wxCloseEvent&)
{}

void MainWindow::OnSize(wxSizeEvent&)
{
    wxLayoutAlgorithm algo;
    algo.LayoutWindow(this, _topBar);
}

void MainWindow::OnDragSash(wxSashEvent& event)
{
    switch (event.GetId())
    {
        case id_bottombar:  _bottomBar->SetDefaultSize(event.GetDragRect().GetSize());  break;
        case id_sidebar:    _sideBar->SetDefaultSize(event.GetDragRect().GetSize());    break;
        default:
            wxASSERT(false);
    }

    wxLayoutAlgorithm().LayoutFrame(this, _topBar);
}

void MainWindow::OnNewMap(wxCommandEvent&)
{
    NewMapDlg dlg(this);
    int result = dlg.ShowModal();

    if (result == wxID_OK)
    {
        Map* newMap;
        TileSet* newTileSet;
        try
        {
            newMap = new Map();
            newMap->width = dlg.width;
            newMap->height = dlg.height;
            newMap->tileSetName = dlg.tileSetName;

            newTileSet = new TileSet();
            if (!dlg.newTileSet)
                newTileSet->Load(dlg.tileSetName);
            else
                newTileSet->New(dlg.tileWidth, dlg.tileHeight);

            _curMapName = "";
            _changed = false;
            UpdateTitle();

            delete _map;        _map = newMap;
            delete _tileSet;    _tileSet = newTileSet;

            _layerVisibility.resize(_map->NumLayers());
            std::fill(_layerVisibility.begin(), _layerVisibility.end(), true);

            _mapView->UpdateScrollBars();       _mapView->Refresh();
            _tileSetView->UpdateScrollBars();   _tileSetView->Refresh();

            ClearList(_undoList);
            ClearList(_redoList);
        }
        catch (std::runtime_error& error)
        {
            wxMessageBox(va("Unable to create new map:\n%s", error.what()), "Error", wxOK | wxCENTER, this);
        }
    }
}

void MainWindow::OnOpenMap(wxCommandEvent&)
{
    wxFileDialog dlg(
        this,
        "Open File",
        "",
        "",
        //"All known (*.ika-map;*.map)|*.ika-map;*.map|"
        "Maps (*.ika-map)|*.ika-map|"
        "All files (*.*)|*.*",
        wxOPEN | wxCHANGE_DIR | wxFILE_MUST_EXIST
        );

    int result = dlg.ShowModal();
    if (result == wxID_CANCEL)
        return;

    std::string filename = dlg.GetPath().c_str();

    LoadMap(filename);

    _changed = false;
    UpdateTitle();
}

void MainWindow::OnSaveMap(wxCommandEvent& event)
{
    if (_curMapName.length() != 0)
    {
        try
        {
            _map->Save(_curMapName);
            _tileSet->Save(_map->tileSetName);
            _changed = false;
            UpdateTitle();
        }
        catch (std::runtime_error err)
        {
            wxMessageBox(va("Unable to write %s:\n%s", _curMapName.c_str(), err.what()), "Error", wxOK | wxCENTER, this);
        }
    }
    else
        OnSaveMapAs(event);
}

void MainWindow::OnSaveMapAs(wxCommandEvent&)
{
    wxFileDialog dlg(
        this,
        "Save Map As",
        "",
        "",
        "ika maps (*.ika-map)|*.ika-map|"
        "All files (*.*)|*.*",
        wxSAVE | wxOVERWRITE_PROMPT
        );

    int result = dlg.ShowModal();
    if (result == wxID_CANCEL)
        return;

    std::string name = dlg.GetPath().c_str();

    try
    {
        _map->Save(name);
        _curMapName = name;
        _changed = false;
        UpdateTitle();
    }
    catch (std::runtime_error err)
    {
        wxMessageBox(va("Unable to write %s:\n%s", name.c_str(), err.what()), "Error", wxOK | wxCENTER, this);
    }
}

void MainWindow::OnLoadTileSet(wxCommandEvent&)
{
    wxFileDialog dlg(
        this,
        "Load tileset",
        "",
        "",
        "VSP tilesets (*.vsp)|*.vsp|"
        "All files (*.*)|*.*",
        wxOPEN | wxFILE_MUST_EXIST
        );

    if (dlg.ShowModal() == wxID_OK)
    {
        TileSet* ts = new TileSet();
        try
        {
            wxFileName fName(dlg.GetPath());
            if (fName.IsAbsolute())
                fName.MakeRelativeTo();

            bool result = ts->Load(fName.GetFullPath().c_str());
            if (!result)    throw std::runtime_error(va("%s does not appear to be a valid tileset", dlg.GetFilename().c_str()));;

            // FIXME: paths aren't taken into consideration here.
            // Need to make sure that the tileset name is always relative to the map's position.  Sprites should be the same.

            HandleCommand(new ChangeTileSetCommand(ts, fName.GetFullPath().c_str()));
        }
        catch (std::runtime_error err)
        {
            delete ts;
            wxMessageBox(va("Unable to load tileset %s:\n%s", dlg.GetPath().c_str(), err.what()), "ERROR", wxOK | wxCENTER, this);
        }
    }
}

void MainWindow::OnSaveTileSetAs(wxCommandEvent&)
{
    wxFileDialog dlg(
        this,
        "Save Tileset As",
        "",
        "",
        "VSP Tilesets (*.vsp)|*.vsp|"
        "All files (*.*)|*.*",
        wxSAVE | wxOVERWRITE_PROMPT
        );

    int result = dlg.ShowModal();
    if (result != wxID_OK)
        return;

    _map->tileSetName = dlg.GetFilename().c_str();
    _tileSet->Save(dlg.GetPath().c_str());
}

void MainWindow::OnExportTileSet(wxCommandEvent&)
{
    wxFileDialog dlg(
        this,
        "Export Tileset to PNG",
        "",
        "",
        "PNG images (*.png)|*.png|"
        "All files (*.*)|*.*",
        wxSAVE | wxOVERWRITE_PROMPT
        );

    if (dlg.ShowModal() == wxID_OK)
    {
        const bool pad = true;
        const int colSize = 18; // gay arbitrary constant until I get around to making a dialog.

        int numRows = max<int>(1, _tileSet->Count() / colSize);
        int tileWidth = _tileSet->Width() + (pad ? 1 : 0);
        int tileHeight = _tileSet->Height() + (pad ? 1 : 0);

        int width = tileWidth * colSize;
        int height = tileHeight * numRows;

        Canvas bigImage(width, height);
        uint i = 0;
        for (int y = 0; y < numRows; y++)
        {
            for (int x = 0; x < colSize; x++)
            {
                CBlitter<Opaque>::Blit(_tileSet->Get(i), bigImage,
                    -(x * tileWidth),
                    -(y * tileHeight));

                i++;
                if (i >= _tileSet->Count())
                    goto breakLoop;
            }
        }

        bigImage.Save(dlg.GetPath().c_str());

breakLoop:;
    }
}

void MainWindow::OnExit(wxCommandEvent&)
{
    Close(true);
}

void MainWindow::OnUndo(wxCommandEvent&)
{
    Undo();
}

void MainWindow::OnRedo(wxCommandEvent&)
{
    Redo();
}

void MainWindow::OnEditMapProperties(wxCommandEvent&)
{
    MapDlg dlg(this);

    int result = dlg.ShowModal();

    if (result == wxID_OK)
    {
        HandleCommand(new ChangeMapPropertiesCommand(dlg.title, dlg.width, dlg.height));
    }
}

void MainWindow::OnImportTiles(wxCommandEvent&)
{
    if (!_importTilesDlg)
        _importTilesDlg = new ImportTilesDlg(this);

    if (_importTilesDlg->ShowModal(_tileSet->Width(), _tileSet->Height()) == wxID_OK)
    {
        if (!_importTilesDlg->_append)
        {
            // This might start to get a bit RAM intensive.

            std::vector<::Command*> commands; // commands to be sent

            if (_tileSet->Count())  // if there are tiles to delete, delete them
                commands.push_back(new DeleteTilesCommand(0, _tileSet->Count() - 1));   // One copy
            commands.push_back(new ResizeTileSetCommand(_importTilesDlg->_width, _importTilesDlg->_height));      // Not much to store here, since there are no tiles to resize. (thus nothing to back up)
            commands.push_back(new InsertTilesCommand(0, _importTilesDlg->tiles));                   // copy of each tile to be inserted.
            HandleCommand(new CompositeCommand(commands));
        }
        else
            HandleCommand(new InsertTilesCommand(_tileSet->Count(), _importTilesDlg->tiles));
    }
}

void MainWindow::OnEditTileAnim(wxCommandEvent&)
{
    TileAnimDlg dlg(this);

    ::Command* cmd = dlg.Execute(_tileSet->GetAnim());

    if (cmd)
        HandleCommand(cmd);
}

void MainWindow::OnCloneLayer(wxCommandEvent&)
{
    if (!_map->NumLayers())
        return;

    uint curLayer = GetCurrentLayer();
    wxASSERT(curLayer < _map->NumLayers());

    HandleCommand(new CloneLayerCommand(curLayer));
}

void MainWindow::OnShowLayerProperties(wxCommandEvent& event)
{
    uint layerIndex = uint(event.GetInt());
    LayerDlg dlg(this, layerIndex);

    int result = dlg.ShowModal();

    if (result == wxID_OK)
    {
        HandleCommand(new ChangeLayerPropertiesCommand(
            _map->GetLayer(layerIndex),
            dlg.label,
            dlg.wrapx,
            dlg.wrapy,
            dlg.x,
            dlg.y));
    }
}

void MainWindow::OnShowLayerContextMenu(wxMouseEvent& event)
{

}

void MainWindow::OnZoomMapIn(wxCommandEvent&)           {   _mapView->IncZoom(-1);      _mapView->Refresh();        _mapView->UpdateScrollBars();   }
void MainWindow::OnZoomMapOut(wxCommandEvent&)          {   _mapView->IncZoom(+1);      _mapView->Refresh();        _mapView->UpdateScrollBars();   }
void MainWindow::OnZoomMapNormal(wxCommandEvent&)       {   _mapView->SetZoom(16);      _mapView->Refresh();        _mapView->UpdateScrollBars();   } // 16:16 == 100%
void MainWindow::OnZoomTileSetIn(wxCommandEvent&)       {   _tileSetView->IncZoom(-1);  _tileSetView->Refresh();    _tileSetView->UpdateScrollBars();   }
void MainWindow::OnZoomTileSetOut(wxCommandEvent&)      {   _tileSetView->IncZoom(+1);  _tileSetView->Refresh();    _tileSetView->UpdateScrollBars();   }
void MainWindow::OnZoomTileSetNormal(wxCommandEvent&)   {   _tileSetView->SetZoom(16);  _tileSetView->Refresh();    _tileSetView->UpdateScrollBars();   } // 16:16 == 100%

void MainWindow::OnConfigureScripts(wxCommandEvent& e)
{
    ScriptDlg dlg(this);

    dlg.ShowModal();
    // easy. :D

    // We gave the script state a pointer, remember.  Need to make sure that it's nuked.
    // Dirty hack, I know. -_-
    // Maybe I should give ScriptState a reference to the list of scripts and an index
    // Then it could get the pointer straight from the horse's mouth, so to speak.
    OnSetTilePaintState(e);

    UpdateScriptMenu();
}

void MainWindow::OnSetCurrentScript(wxCommandEvent& event)
{
    uint id = event.GetId() - id_customscript;
    wxASSERT(id >= 0 && id < _scripts.size());

    _curScript = id;
    HighlightToolButton(id_scripttool);
    _mapView->SetScriptTool(_scripts[_curScript]);
}

void MainWindow::OnCursorUp(wxCommandEvent&)
{
    wxScrollWinEvent evt(wxEVT_SCROLLWIN_PAGEUP, 0, wxVERTICAL);
    _mapView->ProcessEvent(evt);
}

void MainWindow::OnCursorDown(wxCommandEvent&)
{
    wxScrollWinEvent evt(wxEVT_SCROLLWIN_PAGEDOWN, 0, wxVERTICAL);
    _mapView->ProcessEvent(evt);
}

void MainWindow::OnCursorLeft(wxCommandEvent&)
{
    wxScrollWinEvent evt(wxEVT_SCROLLWIN_PAGEUP, 0, wxHORIZONTAL);
    _mapView->ProcessEvent(evt);
}

void MainWindow::OnCursorRight(wxCommandEvent&)
{
    wxScrollWinEvent evt(wxEVT_SCROLLWIN_PAGEDOWN, 0, wxHORIZONTAL);
    _mapView->ProcessEvent(evt);
}

void MainWindow::OnSetTilePaintState(wxCommandEvent&)
{
    HighlightToolButton(id_tilepaint);
    _mapView->Cock();
}

void MainWindow::OnSetCopyPasteState(wxCommandEvent&)
{
    HighlightToolButton(id_copypaste);
    _mapView->SetCopyPasteState();
}

void MainWindow::OnSetObstructionState(wxCommandEvent&)
{
    HighlightToolButton(id_obstructionedit);
    _mapView->SetObstructionState();
}

void MainWindow::OnSetZoneState(wxCommandEvent&)
{
    HighlightToolButton(id_zoneedit);
    _mapView->SetZoneState();
}

void MainWindow::OnSetEntityState(wxCommandEvent&)
{
    HighlightToolButton(id_entityedit);
    _mapView->SetEntityState();
}

void MainWindow::OnSetScriptTool(wxCommandEvent&)
{
    wxASSERT(_curScript < _scripts.size());
    if (!_scripts.empty() && _scripts[_curScript]->IsTool())
    {
        HighlightToolButton(id_scripttool);
        _mapView->SetScriptTool(_scripts[_curScript]);
    }
}

void MainWindow::OnNewLayer(wxCommandEvent&)
{
    HandleCommand(new CreateLayerCommand);
}

void MainWindow::OnDestroyLayer(wxCommandEvent&)
{
    if (GetCurrentLayer() < _map->NumLayers())
        HandleCommand(new DestroyLayerCommand(GetCurrentLayer()));

    if (GetCurrentLayer() >= _map->NumLayers())
        SetCurrentLayer(_map->NumLayers() - 1);
}

void MainWindow::OnMoveLayerUp(wxCommandEvent&)
{
    uint curLay = GetCurrentLayer();

    if (curLay != 0 && curLay < _map->NumLayers())
        HandleCommand(new SwapLayerCommand(curLay, curLay - 1));
}

void MainWindow::OnMoveLayerDown(wxCommandEvent&)
{
    const uint curLay = GetCurrentLayer();

    if (curLay < _map->NumLayers() - 1)
        HandleCommand(new SwapLayerCommand(curLay, curLay + 1));
}

void MainWindow::UpdateTitle()
{
    std::string name = 
        _map->title.length() ?  _map->title :
        _curMapName.length() ?  _curMapName :
                                "Untitled Map";

    const char* asterisk = _changed ? "* " : "";

    SetTitle(va("ikaMap %s - %s[ %s ]", IKA_VERSION, asterisk, name.c_str()));
}

void MainWindow::UpdateScriptMenu()
{
    wxMenu* scriptMenu = new wxMenu;
    if (!_scripts.empty())
    {
        for (uint i = 0; i < _scripts.size(); i++)
        {
            scriptMenu->AppendRadioItem(id_customscript + i, _scripts[i]->GetName().c_str());
            scriptMenu->Enable(id_customscript + i, _scripts[i]->IsTool());
        }
    }
    else
    {
        wxMenuItem* item = new wxMenuItem(scriptMenu, id_dummy, "(None)");
        scriptMenu->Append(item);
        item->Enable(false);
    }

    int id = GetMenuBar()->FindMenu("Tools");
    wxMenu* toolMenu = GetMenuBar()->GetMenu(id);
    wxASSERT(toolMenu);
    toolMenu->Destroy(id_scriptlist);
    toolMenu->Append(id_scriptlist, "&Scripts", scriptMenu);
    _curScript = 0;
}

void MainWindow::SetChanged(bool changed)
{
    if (changed != _changed)
    {
        _changed = changed;
        UpdateTitle();
    }
}

void MainWindow::HighlightToolButton(uint buttonId)
{
    static const uint ids[] =
    {
        id_tilepaint,
        id_copypaste,
        id_obstructionedit,
        id_zoneedit,
        id_waypointedit,
        id_entityedit,
        id_scripttool
    };
    
    for (uint i = 0; i < lengthof(ids); i++)
    {
        uint id = ids[i];
        wxButton* button = wxStaticCast(FindWindowById(id, this), wxButton);
        if (button)
        {
            button->Enable(id != buttonId); // the current button is disabled, since I have no mechanism to keep it pressed at present. (suck)
        }
    }
}

MapView* MainWindow::GetMapView() const { return _mapView; }
TileSetView* MainWindow::GetTileSetView() const { return _tileSetView; }

void MainWindow::Undo()
{
    if (!_undoList.empty())
    {
        ::Command* c = _undoList.top();
        _undoList.pop();

        c->Undo(this);
        _redoList.push(c);
    }
}

void MainWindow::Redo()
{
    if (!_redoList.empty())
    {
        ::Command* c = _redoList.top();
        _redoList.pop();

        c->Do(this);
        _undoList.push(c);
    }
}

void MainWindow::LoadMap(const std::string& fileName)
{
    Map* newMap = new Map;
    bool result = newMap->Load(fileName);

    if (!result)
    {
        ::wxMessageBox(va("Unable to load map %s.\n"
                          "The file may be corrupted, or an unrecognized format.", fileName.c_str()), 
                       "DANGER WILL ROBINSON", 
                       wxOK | wxCENTRE | wxICON_ERROR, 
                       this);
        return;
    }

    TileSet* ts = new TileSet;
    result = ts->Load(newMap->tileSetName.c_str());

    if (!result)
    {
        ::wxMessageBox(va("Unable to load tileset %s.", newMap->tileSetName.c_str()), "Error loading tileset.", wxOK | wxCENTRE | wxICON_ERROR, this);
        delete newMap;
        return;
    }

    _curMapName = fileName;

    delete _map;        _map = newMap;
    delete _tileSet;    _tileSet = ts;

    // Free spritesets used by the old map.
    for (SpriteMap::iterator iter = _sprites.begin(); iter != _sprites.end(); iter++)
        delete iter->second;
    _sprites.clear();

    ClearList(_undoList);
    ClearList(_redoList);

    _layerVisibility.resize(_map->NumLayers());
    std::fill(_layerVisibility.begin(), _layerVisibility.end(), true);

    // Not firing events here because several components listen to more than one
    // event.  This would cause things to redraw themselves more than is sane.
    /*_mapView->UpdateScrollBars();
    _tileSetView->UpdateScrollBars();
    _mapView->Refresh();
    _tileSetView->Refresh();
    _layerList->Update(_map);*/
    mapLoaded.fire(MapTileSetEvent(_map, _tileSet));

    _changed = false;
    UpdateTitle();
}

std::vector<Script*>& MainWindow::GetScripts()
{
    return _scripts;
}

// Executor:
void MainWindow::HandleCommand(::Command* cmd)
{
    ClearList(_redoList);

    cmd->Do(this);
    _undoList.push(cmd);
    SetChanged(true);
}

bool MainWindow::IsLayerVisible(uint index)
{
    wxASSERT(index < _map->NumLayers());
    return _layerVisibility[index];
}

void MainWindow::ShowLayer(uint index, bool show)
{
    wxASSERT(index < _map->NumLayers());

    if (index != _curLayer || show) // disallow hiding the current layer in all cases
    {
        _layerVisibility[index] = show;

        mapVisibilityChanged.fire(MapEvent(_map, index));
    }
}

void MainWindow::EditLayerProperties(uint index)
{
    wxASSERT(index < _map->NumLayers());

    LayerDlg dlg(this, index);

    int result = dlg.ShowModal();

    if (result == wxID_OK)
    {
        HandleCommand(new ChangeLayerPropertiesCommand(
            _map->GetLayer(index),
            dlg.label,
            dlg.wrapx,
            dlg.wrapy,
            dlg.x,
            dlg.y));
    }
}

uint MainWindow::GetCurrentTile()
{
    return _curTile;
}

void MainWindow::SetCurrentTile(uint i)
{
    if (i > _tileSet->Count())
        i = 0;

    _curTile = i;
    curTileChanged.fire(i);
}

uint MainWindow::GetCurrentLayer()
{
    return _curLayer;
}

void MainWindow::SetCurrentLayer(uint i)
{
    wxASSERT(i < _map->NumLayers());

    _curLayer = i;
    _layerVisibility[i] = true; // hack.  Don't want things to respond to effectively the same event twice.
    curLayerChanged.fire(i);
}

void MainWindow::SetStatusBar(const std::string& text, int field)
{
    GetStatusBar()->SetStatusText(text.c_str(), field);
}

Map* MainWindow::GetMap()         { return _map; }
TileSet* MainWindow::GetTileSet() { return _tileSet; }

SpriteSet* MainWindow::GetSpriteSet(const std::string& fileName)
{
    if (_sprites.count(fileName))
        return _sprites[fileName];
    else
    {
        SpriteSet* ss = new SpriteSet();
        bool result = ss->Load(fileName);

        if (!result)
        {
            delete ss;
            ss = 0; // put this away in the cache, so that we know that we tried to load it.
        }
        _sprites[fileName] = ss;
        return ss;
    }
}

MapView* MainWindow::GetMapView()
{
    return _mapView;
}

TileSetView* MainWindow::GetTileSetView()
{
    return _tileSetView;
}

wxWindow* MainWindow::GetParentWindow()
{
    return this;
}

void MainWindow::SwitchTileSet(TileSet* ts)
{
    _tileSet = ts;
    tileSetChanged.fire(MapTileSetEvent(_map, _tileSet));
}
