
#include "command.h"
#include "mainwindow.h"
#include "map.h"
#include "mapview.h"

SetTileCommand::SetTileCommand(uint tx, uint ty, uint li, uint ti)
    : _tileX(tx)
    , _tileY(ty)
    , _layerIndex(li)
    , _tileIndex(ti)
    , _oldTileIndex(0)
{}

void SetTileCommand::Do(MainWindow* m)
{
    Map* map = m->GetMap();

    _oldTileIndex = map->GetLayer(_layerIndex).tiles(_tileX, _tileY);
    map->GetLayer(_layerIndex).tiles(_tileX, _tileY) = _tileIndex;
    m->GetMapView()->Refresh();
}

void SetTileCommand::Undo(MainWindow* m)
{
    Map* map = m->GetMap();

    map->GetLayer(_layerIndex).tiles(_tileX, _tileY) = _oldTileIndex;
    m->GetMapView()->Refresh();
}

//-----------------------------------------------------------------------------

void CreateLayerCommand::Do(MainWindow* m)
{
    Map* map = m->GetMap();

    std::string layName = "New Layer";

    uint i = 1;
    while (map->LayerIndex(layName) != -1)
        layName = va("New Layer %i", ++i);

    _layerIndex = map->NumLayers();

    map->AddLayer(layName, 100, 100); // arbitrary initial size for now

    m->UpdateLayerList();
    m->Refresh();
}

void CreateLayerCommand::Undo(MainWindow* m)
{
    m->GetMap()->DestroyLayer(_layerIndex);

    m->UpdateLayerList();
    m->Refresh();
}

//-----------------------------------------------------------------------------

DestroyLayerCommand::DestroyLayerCommand(uint index)
    : _index(index)
    , _savedLayer(0)
{}

DestroyLayerCommand::~DestroyLayerCommand()
{
    delete _savedLayer;
}

void DestroyLayerCommand::Do(MainWindow* m)
{
    Map* map = m->GetMap();

    _savedLayer = new Map::Layer(map->GetLayer(_index)); // save the layer so we can restore it later
    map->DestroyLayer(_index);

    m->UpdateLayerList();
    m->GetMapView()->Refresh();
}

void DestroyLayerCommand::Undo(MainWindow* m)
{
    m->GetMap()->InsertLayer(_savedLayer, _index);
    _savedLayer = 0;

    m->UpdateLayerList();
    m->GetMapView()->Refresh();
}

//-----------------------------------------------------------------------------

SwapLayerCommand::SwapLayerCommand(uint i1, uint i2)
    : _index1(i1)
    , _index2(i2)
{}

void SwapLayerCommand::Do(MainWindow* m)
{
    Map* map = m->GetMap();
    MapView* mapView = m->GetMapView();

    mapView->Freeze();

    uint curLayer = mapView->GetCurLayer();
    if (curLayer == _index1)        mapView->SetCurLayer(_index2);
    else if (curLayer == _index2)   mapView->SetCurLayer(_index1);

    map->SwapLayers(_index1, _index2);
    
    mapView->Thaw();
    
    m->UpdateLayerList();
    mapView->Refresh();
}

void SwapLayerCommand::Undo(MainWindow* m)
{
    Do(m);  // muahaha
}

//-----------------------------------------------------------------------------

ResizeLayerCommand::ResizeLayerCommand(uint index, uint newWidth, uint newHeight)
    : _index(index)
    , _newWidth(newWidth)
    , _newHeight(newHeight)
    , _savedLayer(0)
{}

void ResizeLayerCommand::Do(MainWindow* m)
{
    Map::Layer* lay = &m->GetMap()->GetLayer(_index);

    if (!_savedLayer)
        _savedLayer = new Map::Layer(*lay);
    lay->Resize(_newWidth, _newHeight);

    m->GetMapView()->Refresh();
}

void ResizeLayerCommand::Undo(MainWindow* m)
{
    // Possible bottleneck here.
    m->GetMap()->GetLayer(_index) = *_savedLayer;

    m->GetMapView()->Refresh();
}

//-----------------------------------------------------------------------------

ChangeLayerPropertiesCommand::ChangeLayerPropertiesCommand(
    Map::Layer* layer,
    const std::string& label, 
    bool wrapx,
    bool wrapy,
    int x,
    int y)
    : _layer(layer)
    , _newProperties(label, wrapx, wrapy, x, y)
{}

void ChangeLayerPropertiesCommand::Do(MainWindow* m)
{
    // Save the old state
    _oldProperties = LayerProperties(_layer->label, _layer->wrapx, _layer->wrapy, _layer->x, _layer->y);

    _layer->label = _newProperties.label;
    _layer->wrapx = _newProperties.wrapx;
    _layer->wrapy = _newProperties.wrapy;
    _layer->x     = _newProperties.x;
    _layer->y     = _newProperties.y;

    m->UpdateLayerList();
    m->GetMapView()->Refresh();
}

void ChangeLayerPropertiesCommand::Undo(MainWindow* m)
{
    _layer->label = _oldProperties.label;
    _layer->wrapx = _oldProperties.wrapx;
    _layer->wrapy = _oldProperties.wrapy;
    _layer->x     = _oldProperties.x;
    _layer->y     = _oldProperties.y;

    m->UpdateLayerList();
    m->GetMapView()->Refresh();
}

//-----------------------------------------------------------------------------

ChangeMapPropertiesCommand::ChangeMapPropertiesCommand(const std::string& newTitle, uint newWidth, uint newHeight)
    : _newTitle(newTitle)
    , _newWidth(newWidth)
    , _newHeight(newHeight)
{}

void ChangeMapPropertiesCommand::Do(MainWindow* m)
{
    Map* map = m->GetMap();

    _oldTitle = map->title;     map->title = _newTitle;
    _oldWidth = map->width;     map->width = _newWidth;
    _oldHeight = map->height;   map->height = _newHeight;

    m->GetMapView()->UpdateScrollBars();
    m->UpdateTitle();
}

void ChangeMapPropertiesCommand::Undo(MainWindow* m)
{
    Map* map = m->GetMap();
    map->title = _oldTitle;
    map->width = _oldWidth;
    map->height = _oldHeight;

    m->GetMapView()->UpdateScrollBars();
    m->UpdateTitle();
}