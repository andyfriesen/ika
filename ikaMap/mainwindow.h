
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <wx/wx.h>

#include <stdexcept>
#include <stack>
#include <map>

#include "common/utility.h"
#include "executor.h"
#include "common/listener.h"

class wxSashLayoutWindow;
class wxSashEvent;
class wxCheckListBox;
struct Command;
struct CompositeCommand;

struct MapView;
struct TileSetView;
struct LayerList;

struct ImportTilesDlg;

struct Map;
struct Tileset;
struct SpriteSet;
struct Script;

/**
 * The main application frame.
 * The ultimate source of all the actual map resources. (sprites, the tileset, and the map itself)
 * Also deals with the layer list, tool buttons, drop down menu, and the Command interface used
 * to actually modify the map.
 */
struct MainWindow : public wxFrame, Executor
{
    friend struct ScriptDlg;

    MainWindow(
        const wxPoint& position = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        const long style = wxDEFAULT_FRAME_STYLE);
    ~MainWindow();

    void OnClose(wxCloseEvent&);
    void OnSize(wxSizeEvent&);
    void OnDragSash(wxSashEvent& event);

    void OnNewMap(wxCommandEvent&);
    void OnOpenMap(wxCommandEvent&);
    void OnSaveMap(wxCommandEvent&);
    void OnSaveMapAs(wxCommandEvent&);
    void OnLoadTileSet(wxCommandEvent&);
    void OnSaveTileSetAs(wxCommandEvent&);
    void OnExportTileSet(wxCommandEvent&);
    void OnExit(wxCommandEvent&);
    void OnUndo(wxCommandEvent&);
    void OnRedo(wxCommandEvent&);
    void OnEditMapProperties(wxCommandEvent&);
    void OnImportTiles(wxCommandEvent&);
    void OnEditTileAnim(wxCommandEvent&);
    void OnCloneLayer(wxCommandEvent&);

    void OnShowLayerContextMenu(wxMouseEvent& event);
    void OnZoomMapIn(wxCommandEvent&);
    void OnZoomMapOut(wxCommandEvent&);
    void OnZoomMapNormal(wxCommandEvent&);
    void OnZoomTileSetIn(wxCommandEvent&);
    void OnZoomTileSetOut(wxCommandEvent&);
    void OnZoomTileSetNormal(wxCommandEvent&);

    void OnConfigureScripts(wxCommandEvent&);
    void OnSetCurrentScript(wxCommandEvent& event);

    void OnCursorDown(wxCommandEvent&);
    void OnCursorUp(wxCommandEvent&);
    void OnCursorLeft(wxCommandEvent&);
    void OnCursorRight(wxCommandEvent&);

    void OnSetTilePaintState(wxCommandEvent&);
    void OnSetCopyPasteState(wxCommandEvent&);
    void OnSetObstructionState(wxCommandEvent&);
    void OnSetZoneState(wxCommandEvent&);
    void OnSetEntityState(wxCommandEvent&);
    void OnSetScriptTool(wxCommandEvent&);

    void OnNewLayer(wxCommandEvent&);
    void OnDestroyLayer(wxCommandEvent&);
    void OnMoveLayerUp(wxCommandEvent&);
    void OnMoveLayerDown(wxCommandEvent&);

    void UpdateTitle();
    void UpdateScriptMenu();

    void SetChanged(bool changed);

    void HighlightToolButton(uint buttonId);

    Map* GetMap() const;
    Tileset* GetTileSet() const;
    MapView* GetMapView() const;
    TileSetView* GetTileSetView() const;

    void LoadMap(const std::string& fileName);

    // Executor:
    virtual void HandleCommand(::Command* cmd); ///< executes a command and adds it to the undo stack
    virtual void AddCommand(::Command* cmd);    ///< adds the command to the undo stack without executing it.

    virtual void Undo();
    virtual void Redo();

    virtual bool IsLayerVisible(uint index);
    virtual void ShowLayer(uint index, bool show);

    virtual void EditLayerProperties(uint index);

    virtual uint GetCurrentTile();
    virtual void SetCurrentTile(uint i);

    virtual uint GetCurrentLayer();
    virtual void SetCurrentLayer(uint i);

    virtual void SetStatusBar(const std::string& text, int field);
    void SetZoom(uint factor);
    void SetZoomRelative(int factor);

    virtual Map* GetMap();
    virtual Tileset* GetTileSet();
    virtual SpriteSet* GetSpriteSet(const std::string& fileName);

    virtual MapView* GetMapView();
    virtual TileSetView* GetTileSetView();
    virtual wxWindow* GetParentWindow();

    virtual void SwitchTileSet(Tileset* ts);

protected:
    std::vector<Script*>& GetScripts();

    Map*     _map;
    Tileset* _tileSet;

    DECLARE_EVENT_TABLE()

private:
    wxStatusBar*        _statusBar;
    wxSashLayoutWindow* _topBar;
    wxSashLayoutWindow* _bottomBar;
    wxSashLayoutWindow* _sideBar;
    LayerList*          _layerList;
    MapView*            _mapView;
    TileSetView*        _tileSetView;

    // Store and reuse the dialog so that it can remember its previous values.
    ScopedPtr<ImportTilesDlg> _importTilesDlg;
    
    typedef std::map<std::string, SpriteSet*> SpriteMap;
    SpriteMap _sprites;

    std::vector<Script*> _scripts;
    uint _curScript;    // the currently active tool script
    
    std::stack< ::Command*> _undoList;
    std::stack< ::Command*> _redoList;

    std::vector<bool>      _layerVisibility;

    std::string _curMapName;
    bool _changed;

    uint _curTile;
    uint _curLayer;

    // helper function for clearing the undo or redo list.  Deletes Commands as it does so, to avoid leaks.
    static void ClearList(std::stack< ::Command*>& list);
};

#endif