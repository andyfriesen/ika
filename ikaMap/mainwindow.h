
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <wx/wx.h>
#include <stack>
#include <map>

#include "spriteset.h"
#include "controller.h"

class wxSashLayoutWindow;
class wxSashEvent;
class wxCheckListBox;
class Command;
class MapView;
class TileSetView;
struct Map;
class TileSet;

class MainWindow : public wxFrame
{
private:
    wxSashLayoutWindow* _topBar;
    wxSashLayoutWindow* _bottomBar;
    wxSashLayoutWindow* _sideBar;
    wxCheckListBox*     _layerList;
    MapView* _mapView;
    TileSetView* _tileSetView;

    Map* _map;
    TileSet* _tileSet;
    std::map<std::string, SpriteSet*> _sprites;
    
    std::stack<::Command*> _undoList;
    std::stack<::Command*> _redoList;

    static const float _version;
    std::string _curMapName;
    bool _modified;

    // lil helper function
    static void ClearList(std::stack<::Command*>& list);

public:
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
    void OnExit(wxCommandEvent&);
    void OnUndo(wxCommandEvent&);
    void OnRedo(wxCommandEvent&);
    void OnEditMapProperties(wxCommandEvent&);
    void OnChangeCurrentLayer(wxCommandEvent& event);
    void OnShowLayerProperties(wxCommandEvent& event);
    void OnToggleLayer(wxCommandEvent& event);
    void OnSetTilePaintState(wxCommandEvent&);
    void OnSetObstructionState(wxCommandEvent&);

    void OnNewLayer(wxCommandEvent&);
    void OnDestroyLayer(wxCommandEvent&);
    void OnMoveLayerUp(wxCommandEvent&);
    void OnMoveLayerDown(wxCommandEvent&);

    void UpdateLayerList();
    void UpdateTitle();

    bool IsLayerVisible(uint index) const;
    void ShowLayer(uint index, bool show = true);
    void HideLayer(uint index) { ShowLayer(index, false); }

    Map* GetMap() const;
    TileSet* GetTileSet() const;
    MapView* GetMapView() const;
    TileSetView* GetTileSetView() const;

    void HandleCommand(::Command* cmd);
    void Undo();
    void Redo();

    void LoadMap(const std::string& fileName);

    SpriteSet* GetSprite(const std::string& fileName);

    DECLARE_EVENT_TABLE()
};

#endif