
#include "wxinc.h"

#include <vector>
#include <sstream>

#include "mainwindow.h"

#include <wx/laywin.h>
// Important UI element things
#include "mapview.h"
#include "tilesetview.h"

// Dialogs
#include "newmapdlg.h"
#include "mapdlg.h"
#include "layerdlg.h"
#include "importtilesdlg.h"

// Other stuff
#include "tileset.h"
#include "command.h"

#define VERTICAL_FUN

//
const float MainWindow::_version = 0.1f;
//

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
        id_fileexit,

        id_editundo,
        id_editredo,
        id_editmapproperties,
        id_importtiles,

        id_zoommapin,
        id_zoommapout,
        id_zoommapnormal,
        id_zoomtilesetin,
        id_zoomtilesetout,
        id_zoomtilesetnormal,

        id_tilepaint,
        id_selecttiles,
        id_obstructionedit,
        id_zoneedit,
        id_waypointedit,
        id_entityedit,

        id_newlayer,
        id_destroylayer,
        id_movelayerup,
        id_movelayerdown,

        id_layerlist,
        id_topbar,
        id_bottombar,
        id_sidebar,
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

    EVT_MENU(id_zoommapin, MainWindow::OnZoomMapIn)
    EVT_MENU(id_zoommapout, MainWindow::OnZoomMapOut)
    EVT_MENU(id_zoommapnormal, MainWindow::OnZoomMapNormal)
    EVT_MENU(id_zoomtilesetin, MainWindow::OnZoomTileSetIn)
    EVT_MENU(id_zoomtilesetout, MainWindow::OnZoomTileSetOut)
    EVT_MENU(id_zoomtilesetnormal, MainWindow::OnZoomTileSetNormal)

    EVT_BUTTON(id_tilepaint, MainWindow::OnSetTilePaintState)
    EVT_BUTTON(id_obstructionedit, MainWindow::OnSetObstructionState)
    EVT_BUTTON(id_entityedit, MainWindow::OnSetEntityState)

    EVT_BUTTON(id_newlayer, MainWindow::OnNewLayer)
    EVT_BUTTON(id_destroylayer, MainWindow::OnDestroyLayer)
    EVT_BUTTON(id_movelayerup, MainWindow::OnMoveLayerUp)
    EVT_BUTTON(id_movelayerdown, MainWindow::OnMoveLayerDown)

    EVT_LISTBOX(id_layerlist, MainWindow::OnChangeCurrentLayer)
    EVT_LISTBOX_DCLICK(id_layerlist, MainWindow::OnShowLayerProperties)
    EVT_CHECKLISTBOX(id_layerlist, MainWindow::OnToggleLayer)
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
    : wxFrame(0, -1, va("ikaMap version %0.2f", _version), position, size, style)
    , _map(0)
    , _tileSet(0)
{
    _sideBar = new wxSashLayoutWindow(this, id_sidebar);
    _sideBar->SetAlignment(wxLAYOUT_LEFT);
    _sideBar->SetOrientation(wxLAYOUT_VERTICAL);
    _sideBar->SetSashVisible(wxSASH_RIGHT, true);
    _sideBar->SetDefaultSize(wxSize(150, 1000));

    wxPanel* sidePanel = new wxPanel(_sideBar);
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

    //*
    struct ToolButton
    {
        const char* iconName;
        uint id;
        const char* toolTip;
    } toolButtons[] = 
    {
        {   "brushicon",        id_tilepaint,       "Place individual tiles on the map."                            },
        {   "selecticon",       id_selecttiles,     "Select a group of tiles, and duplicate them elsewhere."        },
        {   "obstructionicon",  id_obstructionedit, "Edit obstructed areas."                                        },
        {   "zoneicon",         id_zoneedit,        "Edit zones."                                                   },
        {   "waypointicon",     id_waypointedit,    "Edit waypoints."                                               },
        {   "entityicon",       id_entityedit,      "Edit entities."                                                },
        {   "newicon",          id_newlayer,        "Create a brand new layer."                                     },
        {   "trashicon",        id_destroylayer,    "Destroy the currently selected layer."                         },
        {   "uparrowicon",      id_movelayerup,     "Move the current layer upwards. (beneath the previous layer)"  },
        {   "downarrowicon",    id_movelayerdown,   "Move the current layer downwards. (above the next layer)"      },
    };
    const int numToolButtons = sizeof toolButtons / sizeof toolButtons[0];

    {
        wxSizer* miniSizer = new wxGridSizer(6);

        for (uint i = 0; i < numToolButtons; i++)
        {
            //*
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

    sizer->Add(_layerList = new wxCheckListBox(sidePanel, id_layerlist), 1, wxEXPAND);
    sidePanel->SetSizer(sizer);

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

    wxMenu* viewMenu = new wxMenu;
    viewMenu->Append(id_zoommapin, "Zoom Map In\t+");
    viewMenu->Append(id_zoommapout, "Zoom Map Out\t-");
    viewMenu->Append(id_zoommapnormal, "Zoom Map to 100%\t=", "Stop zooming the map.");
    viewMenu->AppendSeparator();
    viewMenu->Append(id_zoomtilesetin, "Zoom Tileset In\tNumpad 8");
    viewMenu->Append(id_zoomtilesetout, "Zoom Tileset Out\tNumpad 2");
    viewMenu->Append(id_zoomtilesetnormal, "Zoom Tileset to 100%\tNumpad 5", "Stop zooming on the tileset.");

    wxMenuBar* menuBar = new wxMenuBar;
    menuBar->Append(fileMenu, "&File");
    menuBar->Append(editMenu, "&Edit");
    menuBar->Append(viewMenu, "&View");
    SetMenuBar(menuBar);

    // Set up hotkey stuff.
    const int tableSize = 11;
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
    wxAcceleratorTable table(tableSize, entries);
    SetAcceleratorTable(table);

    UpdateLayerList();
    UpdateTitle();
}

MainWindow::~MainWindow()
{
    delete _map;
    delete _tileSet;

    for (std::map<std::string, SpriteSet*>::iterator iter = _sprites.begin(); iter != _sprites.end(); iter++)
        delete iter->second;
    _sprites.clear();

    ClearList(_undoList);
    ClearList(_redoList);
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
        }
        catch (std::runtime_error& error)
        {
            wxMessageBox(va("Unable to create new map:\n%s", error.what()), "Error", wxOK | wxCENTER, this);
        }

        delete _map;        _map = newMap;
        delete _tileSet;    _tileSet = newTileSet;

        _mapView->UpdateScrollBars();       _mapView->Refresh();
        _tileSetView->UpdateScrollBars();   _tileSetView->Refresh();

        ClearList(_undoList);
        ClearList(_redoList);

        UpdateLayerList();
        UpdateTitle();
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
    UpdateLayerList();

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
        SetTitle(va("ikaMap version %0.2f - [ %s ]", _version, _curMapName.c_str()));
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
            bool result = ts->Load(dlg.GetPath().c_str());
            if (!result)    throw std::runtime_error(va("%s does not appear to be a valid tileset", dlg.GetFilename().c_str()));;

            // FIXME: paths aren't taken into consideration here.
            // Need to make sure that the tileset name is always relative to the map's position.  Sprites should be the same.

            HandleCommand(new ChangeTileSetCommand(ts, dlg.GetPath().c_str()));
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
    ImportTilesDlg dlg(this);

    if (dlg.ShowModal(_tileSet->Width(), _tileSet->Height()) == wxID_OK)
    {
        if (!dlg.append)
        {
            // This might start to get a bit RAM intensive.

            std::vector<::Command*> commands; // commands to be sent

            if (_tileSet->Count())  // if there are tiles to delete, delete them
                commands.push_back(new DeleteTilesCommand(0, _tileSet->Count() - 1));   // One copy
            commands.push_back(new ResizeTileSetCommand(dlg.width, dlg.height));        // Not much to store here, since there are no tiles to resize. (thus nothing to back up)
            commands.push_back(new InsertTilesCommand(0, dlg.tiles));                   // copy of each tile to be inserted.
            HandleCommand(new CompositeCommand(commands));
        }
        else
            HandleCommand(new InsertTilesCommand(_tileSet->Count(), dlg.tiles));
    }
}

void MainWindow::OnChangeCurrentLayer(wxCommandEvent& event)
{
    wxASSERT(_map != 0 && (uint)event.GetInt() < _map->NumLayers());
    _mapView->SetCurLayer(event.GetInt());

    _mapView->Render();
    _mapView->ShowPage();
}

void MainWindow::OnShowLayerProperties(wxCommandEvent& event)
{
    LayerDlg dlg(this, event.GetInt());

    int result = dlg.ShowModal();

    if (result == wxID_OK)
    {
        HandleCommand(new ChangeLayerPropertiesCommand(
            &_map->GetLayer(_mapView->GetCurLayer()),
            dlg.label,
            dlg.wrapx,
            dlg.wrapy,
            dlg.x,
            dlg.y));
    }
}

void MainWindow::OnZoomMapIn(wxCommandEvent&)           {   _mapView->IncZoom(-1);      _mapView->Refresh();        _mapView->UpdateScrollBars();   }
void MainWindow::OnZoomMapOut(wxCommandEvent&)          {   _mapView->IncZoom(+1);      _mapView->Refresh();        _mapView->UpdateScrollBars();   }
void MainWindow::OnZoomMapNormal(wxCommandEvent&)       {   _mapView->SetZoom(16);      _mapView->Refresh();        _mapView->UpdateScrollBars();   } // 16:16 == 100%
void MainWindow::OnZoomTileSetIn(wxCommandEvent&)       {   _tileSetView->IncZoom(-1);  _tileSetView->Refresh();    _tileSetView->UpdateScrollBars();   }
void MainWindow::OnZoomTileSetOut(wxCommandEvent&)      {   _tileSetView->IncZoom(+1);  _tileSetView->Refresh();    _tileSetView->UpdateScrollBars();   }
void MainWindow::OnZoomTileSetNormal(wxCommandEvent&)   {   _tileSetView->SetZoom(16);  _tileSetView->Refresh();    _tileSetView->UpdateScrollBars();   } // 16:16 == 100%

void MainWindow::OnToggleLayer(wxCommandEvent& event)
{
    //int layIdx = event.GetInt();
    //wxASSERT(layIdx >= 0 && layIdx < _map->NumLayers());

    _mapView->Render();
    _mapView->ShowPage();
}

void MainWindow::OnSetTilePaintState(wxCommandEvent&)
{
    _mapView->Cock();
}

void MainWindow::OnSetObstructionState(wxCommandEvent&)
{
    _mapView->SetObstructionState();
}

void MainWindow::OnSetEntityState(wxCommandEvent&)
{
    _mapView->SetEntityState();
}

void MainWindow::OnNewLayer(wxCommandEvent&)
{
    HandleCommand(new CreateLayerCommand);
}

void MainWindow::OnDestroyLayer(wxCommandEvent&)
{
    HandleCommand(new DestroyLayerCommand(_mapView->GetCurLayer()));
}

void MainWindow::OnMoveLayerUp(wxCommandEvent&)
{
    uint curLay = _mapView->GetCurLayer();

    if (curLay != 0)
        HandleCommand(new SwapLayerCommand(curLay, curLay - 1));
}

void MainWindow::OnMoveLayerDown(wxCommandEvent&)
{
    uint curLay = _mapView->GetCurLayer();

    if (curLay < _map->NumLayers() - 1)
        HandleCommand(new SwapLayerCommand(curLay, curLay + 1));
}

// FIXME: this unhides every layer.
void MainWindow::UpdateLayerList()
{
    int curLayer = _mapView->GetCurLayer();//_layerList->GetSelection();

    _layerList->Clear();
    _mapView->Freeze();                                         // stops the map view from refreshing every time we check something.

    for (uint i = 0; i < _map->NumLayers(); i++)
        _layerList->Append(_map->GetLayer(i).label.c_str());

    for (uint i = 0; i < _map->NumLayers(); i++)
        _layerList->Check(i);

    bool b = _map->NumLayers() > 0;

    if (b) _layerList->SetSelection(clamp<int>(curLayer, 0, _map->NumLayers()));

    _mapView->Thaw();                                           // Let the map view draw again.

    FindWindowById(id_destroylayer)->Enable(b);
    FindWindowById(id_movelayerup)->Enable(b);
    FindWindowById(id_movelayerdown)->Enable(b);
}

void MainWindow::UpdateTitle()
{
    std::string name = 
        _map->title.length() ?  _map->title :
        _curMapName.length() ?  _curMapName :
                                "Untitled Map";

    SetTitle(va("ikaMap version %0.2f - [ %s ]", _version, name.c_str()));
}

bool MainWindow::IsLayerVisible(uint index) const
{
    wxASSERT(index < _map->NumLayers());
    return _layerList->IsChecked(index);
}

void MainWindow::ShowLayer(uint index, bool show)
{
    wxASSERT(index < _map->NumLayers());

    _layerList->Check(index, show);
    _mapView->Render();
    _mapView->ShowPage();
}

Map* MainWindow::GetMap() const { return _map; }
TileSet* MainWindow::GetTileSet() const { return _tileSet; }
MapView* MainWindow::GetMapView() const { return _mapView; }
TileSetView* MainWindow::GetTileSetView() const { return _tileSetView; }

void MainWindow::HandleCommand(::Command* cmd)
{
    ClearList(_redoList);

    cmd->Do(this);
    _undoList.push(cmd);
}

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
    for (std::map<std::string, SpriteSet*>::iterator iter = _sprites.begin(); iter != _sprites.end(); iter++)
        delete iter->second;
    _sprites.clear();

    // Load spritesets needed by this map.
    /*for (std::map<std::string, Map::Entity>::iterator iter = newMap->entities.begin(); iter != newMap->entities.end(); iter++)
    {
        std::string spriteName = iter->second.spriteName;
        SpriteSet* ss = new SpriteSet;//_spriteController.Load(spriteName);
        ss->Load(spriteName);
        if (ss != 0)
            _sprites[spriteName] = ss;
    }*/

    UpdateLayerList();

    ClearList(_undoList);
    ClearList(_redoList);

    _mapView->UpdateScrollBars();
    _tileSetView->UpdateScrollBars();
    _mapView->Refresh();
    _tileSetView->Refresh();
}

SpriteSet* MainWindow::GetSprite(const std::string& fileName)
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
