#include "editstate.h"
#include "mainwindow.h"
#include "mapview.h"
#include "tilesetview.h"
#include "map.h"
#include "tileset.h"
#include "command.h"

EditState::EditState(MainWindow* mw)
    : _mainWindow(mw)
{
}

MainWindow* EditState::GetMainWindow() const { return _mainWindow; }
MapView* EditState::GetMapView() const { return _mainWindow->GetMapView(); }
TileSetView* EditState::GetTileSetView() const { return _mainWindow->GetTileSetView(); }
Map* EditState::GetMap() const { return _mainWindow->GetMap(); }
Map::Layer* EditState::GetCurLayer() const
{ 
    if (_mainWindow->GetMap()->NumLayers() == 0)
        return 0;
    else
        return _mainWindow->GetMap()->GetLayer(_mainWindow->GetMapView()->GetCurLayer());
}

uint EditState::GetCurLayerIndex() const
{
    return _mainWindow->GetMapView()->GetCurLayer();
}

TileSet* EditState::GetTileSet() const { return _mainWindow->GetTileSet(); }

void EditState::HandleCommand(::Command* cmd)
{
    _mainWindow->HandleCommand(cmd);
}