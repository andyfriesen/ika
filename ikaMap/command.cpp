
#include "command.h"
#include "mainwindow.h"
#include "mapview.h"
#include "tilesetview.h"

#include "map.h"
#include "tileset.h"

CompositeCommand::CompositeCommand(const std::vector<Command*>& commands)
    : _commands(commands)
{}

CompositeCommand::~CompositeCommand()
{
    for (uint i = 0; i < _commands.size(); i++)
        delete _commands[i];
}

void CompositeCommand::Do(MainWindow* m)
{
    for (uint i = 0; i < _commands.size(); i++)
        _commands[i]->Do(m);
}

void CompositeCommand::Undo(MainWindow* m)
{
    uint i = _commands.size() - 1;
    do
    {
        _commands[i]->Undo(m);
    } while (i-- != 0);
}

//-----------------------------------------------------------------------------

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

    _oldTileIndex = map->GetLayer(_layerIndex)->tiles(_tileX, _tileY);
    map->GetLayer(_layerIndex)->tiles(_tileX, _tileY) = _tileIndex;
    m->GetMapView()->Refresh();
}

void SetTileCommand::Undo(MainWindow* m)
{
    Map* map = m->GetMap();

    map->GetLayer(_layerIndex)->tiles(_tileX, _tileY) = _oldTileIndex;
    m->GetMapView()->Refresh();
}

//-----------------------------------------------------------------------------

PasteTilesCommand::PasteTilesCommand(int x, int y, uint layerIndex, const Matrix<uint>& tiles)
    : _x(x)
    , _y(y)
    , _layerIndex(layerIndex)
    , _tiles(tiles)
{}

void PasteTilesCommand::Do(MainWindow* m)
{
    Matrix<uint>& dest = m->GetMap()->GetLayer(m->GetMapView()->GetCurLayer())->tiles;

    for (uint y = 0; y < _tiles.Height(); y++)
    {
        const uint destY = y + _y;
        uint destX = _x;

        for (uint x = 0; x < _tiles.Width(); x++)
        {
            swap(_tiles(x, y), dest(destX, destY));
            destX++;
        }
    }

    m->GetMapView()->Refresh();
}

void PasteTilesCommand::Undo(MainWindow* m)
{
    // teehee.  Swapping is neat.
    Do(m);
}

//-----------------------------------------------------------------------------

SetObstructionCommand::SetObstructionCommand(uint x, uint y, uint layerIndex, u8 set)
    : _x(x)
    , _y(y)
    , _layerIndex(layerIndex)
    , _set(set)
{}

void SetObstructionCommand::Do(MainWindow* m)
{
    Matrix<u8>& obs = m->GetMap()->GetLayer(_layerIndex)->obstructions;

    _oldValue = obs(_x, _y);
    obs(_x, _y) = _set;
    m->GetMapView()->Refresh();
}

void SetObstructionCommand::Undo(MainWindow* m)
{
    m->GetMap()->GetLayer(_layerIndex)->obstructions(_x, _y) = _oldValue;
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

    _savedLayer = new Map::Layer(*map->GetLayer(_index)); // save the layer so we can restore it later
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

CloneLayerCommand::CloneLayerCommand(uint index)
    : _index(index)
{}

void CloneLayerCommand::Do(MainWindow* m)
{
    Map::Layer* newLay = new Map::Layer(*m->GetMap()->GetLayer(_index));
    m->GetMap()->InsertLayer(newLay, _index + 1);

    m->UpdateLayerList();
    m->GetMapView()->Refresh();
}

void CloneLayerCommand::Undo(MainWindow* m)
{
    m->GetMap()->DestroyLayer(_index + 1);

    m->UpdateLayerList();
    m->GetMapView()->Refresh();
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
    Map::Layer* lay = m->GetMap()->GetLayer(_index);

    if (!_savedLayer)
        _savedLayer = new Map::Layer(*lay);
    lay->Resize(_newWidth, _newHeight);

    m->GetMapView()->Refresh();
}

void ResizeLayerCommand::Undo(MainWindow* m)
{
    // Possible bottleneck here.
    *m->GetMap()->GetLayer(_index) = *_savedLayer;

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

//-----------------------------------------------------------------------------

ChangeEntityPropertiesCommand::ChangeEntityPropertiesCommand(
    uint layerIndex, uint entityIndex, Map::Entity newData)
    : _layerIndex(layerIndex)
    , _entityIndex(entityIndex)
    , _newData(newData)
{}

void ChangeEntityPropertiesCommand::Do(MainWindow* m)
{
    Map* map = m->GetMap();
    Map::Layer* lay = map->GetLayer(_layerIndex);

    _oldData = lay->entities[_entityIndex];

    lay->entities[_entityIndex] = _newData;

    m->GetMapView()->Refresh();
}

void ChangeEntityPropertiesCommand::Undo(MainWindow* m)
{
    Map* map = m->GetMap();
    Map::Layer* lay = map->GetLayer(_layerIndex);

    lay->entities[_entityIndex] = _oldData;

    m->GetMapView()->Refresh();
}

//-----------------------------------------------------------------------------

CreateEntityCommand::CreateEntityCommand(uint layerIndex, int x, int y)
    : _layerIndex(layerIndex)
    , _x(x)
    , _y(y)
{}

void CreateEntityCommand::Do(MainWindow* m)
{
    Map::Layer* lay = m->GetMap()->GetLayer(_layerIndex);
    _entityIndex = lay->entities.size();
    lay->entities.push_back(Map::Entity());
    lay->entities[_entityIndex].x = _x;
    lay->entities[_entityIndex].y = _y;

    m->GetMapView()->Refresh();
}

void CreateEntityCommand::Undo(MainWindow* m)
{
    Map::Layer* lay = m->GetMap()->GetLayer(_layerIndex);
    lay->entities.erase(lay->entities.begin() + _entityIndex);

    m->GetMapView()->Refresh();
}

//-----------------------------------------------------------------------------

DestroyEntityCommand::DestroyEntityCommand(uint layerIndex, uint entityIndex)
    : _layerIndex(layerIndex)
    , _entityIndex(entityIndex)
{}

void DestroyEntityCommand::Do(MainWindow* m)
{
    Map::Layer* lay = m->GetMap()->GetLayer(_layerIndex);
    
    _oldData = lay->entities[_entityIndex];
    
    lay->entities.erase(lay->entities.begin() + _entityIndex);

    m->GetMapView()->Refresh();
}

void DestroyEntityCommand::Undo(MainWindow* m)
{
    Map::Layer* lay = m->GetMap()->GetLayer(_layerIndex);
    lay->entities.insert(lay->entities.begin() + _entityIndex, _oldData);

    m->GetMapView()->Refresh();
}

//-----------------------------------------------------------------------------

ChangeTileSetCommand::ChangeTileSetCommand(TileSet* tileSet, const std::string& fileName)
    : _tileSet(tileSet)
    , _fileName(fileName)
{}

ChangeTileSetCommand::~ChangeTileSetCommand()
{
    delete _tileSet;
}

void ChangeTileSetCommand::Do(MainWindow* m)
{
    std::string s = _fileName;
    TileSet* t    = _tileSet;

    _tileSet  = m->_tileSet;
    _fileName = m->GetMap()->tileSetName;

    m->_tileSet              = t;
    m->GetMap()->tileSetName = s;

    m->GetMapView()->Refresh();
    m->GetTileSetView()->Refresh();
}

void ChangeTileSetCommand::Undo(MainWindow* m)
{
    Do(m);  // a swap undoes a swap
}

//-----------------------------------------------------------------------------

InsertTilesCommand::InsertTilesCommand(uint pos, std::vector<Canvas >& tiles)
    : _startPos(pos)
{
    _tiles.reserve(tiles.size());
    for (uint i = 0; i < tiles.size(); i++)
        _tiles.push_back(tiles[i]);
}

void InsertTilesCommand::Do(MainWindow* m)
{
    TileSet* ts = m->GetTileSet();

    for (uint i = 0; i < _tiles.size(); i++)
        ts->InsertTile(_startPos + i, _tiles[i]);

    m->GetTileSetView()->Refresh();
    m->GetMapView()->Refresh();
}

void InsertTilesCommand::Undo(MainWindow* m)
{
    TileSet* ts = m->GetTileSet();

    for (uint i = 0; i < _tiles.size(); i++)
        ts->DeleteTile(ts->Count() - 1);

    m->GetTileSetView()->Refresh();
    m->GetMapView()->Refresh();
}

//-----------------------------------------------------------------------------

DeleteTilesCommand::DeleteTilesCommand(uint firstTile, uint lastTile)
    : _firstTile(firstTile)
    , _lastTile(lastTile)
{
    assert(firstTile <= lastTile);
}

void DeleteTilesCommand::Do(MainWindow* m)
{
    TileSet* ts = m->GetTileSet();

    _savedTiles.reserve(_lastTile - _firstTile + 1);

    for (uint i = _firstTile; i <= _lastTile; i++)
    {
        _savedTiles.push_back(ts->Get(_firstTile));     // save the tile
        ts->DeleteTile(_firstTile);                     // then nuke it
    }

    m->GetMapView()->Refresh();
    m->GetTileSetView()->Refresh();
}

void DeleteTilesCommand::Undo(MainWindow* m)
{
    TileSet* ts = m->GetTileSet();

    for (uint i = _firstTile; i <= _lastTile; i++)
    {
        ts->InsertTile(i, _savedTiles[i]);
    }

    m->GetMapView()->Refresh();
    m->GetTileSetView()->Refresh();
}

//-----------------------------------------------------------------------------

ResizeTileSetCommand::ResizeTileSetCommand(uint newWidth, uint newHeight)
    : _newWidth(newWidth)
    , _newHeight(newHeight)
{}

void ResizeTileSetCommand::Do(MainWindow* m)
{
    TileSet* ts = m->GetTileSet();

    _oldWidth = ts->Width();
    _oldHeight = ts->Height();

    _savedTiles.clear();
    _savedTiles.reserve(ts->Count());
    for (uint i = 0; i < ts->Count(); i++)
    {
        _savedTiles.push_back(ts->Get(i));
    }

    ts->New(_newWidth, _newHeight);
}

void ResizeTileSetCommand::Undo(MainWindow* m)
{
    TileSet* ts = m->GetTileSet();

    ts->New(_oldWidth, _oldHeight);
    for (uint i = 0; i < _savedTiles.size(); i++)
        ts->AppendTile(_savedTiles[i]);

    _savedTiles.clear();    // free up all that memory.  We don't need it anymore anyway.
}

//-----------------------------------------------------------------------------

DefineZoneBluePrintCommand::DefineZoneBluePrintCommand(Map::Zone& newZone, Map::Zone& oldZone)
    : _newZone(newZone)
    , _oldZone(oldZone)
    , _mode(update)
{}

DefineZoneBluePrintCommand::DefineZoneBluePrintCommand(int, Map::Zone& oldZone)
    : _oldZone(oldZone)
    , _mode(destroy)
{}

DefineZoneBluePrintCommand::DefineZoneBluePrintCommand(Map::Zone& newZone, int)
    : _newZone(newZone)
    , _mode(create)
{}

void DefineZoneBluePrintCommand::Do(MainWindow* m)
{
    switch (_mode)
    {
    case create:    // fallthrough
    case update:    m->GetMap()->zones[_newZone.label] = _newZone;    break;  // already have the old zone data
    case destroy:   m->GetMap()->zones.erase(_oldZone.label);   break;        
    }
}

void DefineZoneBluePrintCommand::Undo(MainWindow* m)
{
    switch (_mode)
    {
    case create:    m->GetMap()->zones.erase(_newZone.label);       break;
    case update:    // fallthrough
    case destroy:   m->GetMap()->zones[_oldZone.label] = _oldZone;  break;
    }
}

//-----------------------------------------------------------------------------

PlaceZoneCommand::PlaceZoneCommand(const Rect& position, uint layerIndex, const std::string& bluePrint)
    : _position(position)
    , _layerIndex(layerIndex)
    , _bluePrint(bluePrint)
{}

void PlaceZoneCommand::Do(MainWindow* m)
{
    Map::Layer::Zone z;
    z.position = _position;

    m->GetMap()->GetLayer(_layerIndex)->zones.push_back(z);
    m->GetMapView()->Refresh();
}

void PlaceZoneCommand::Undo(MainWindow* m)
{
    m->GetMap()->GetLayer(_layerIndex)->zones.pop_back();
    m->GetMapView()->Refresh();
}

//-----------------------------------------------------------------------------

ChangeZoneCommand::ChangeZoneCommand(uint layerIndex, uint zoneIndex, const Map::Layer::Zone& newZone)
    : _layerIndex(layerIndex)
    , _zoneIndex(zoneIndex)
    , _newZone(newZone)
{}

void ChangeZoneCommand::Do(MainWindow* m)
{
    Map::Layer* layer = m->GetMap()->GetLayer(_layerIndex);
    _oldZone = layer->zones[_zoneIndex];
    layer->zones[_zoneIndex] = _newZone;
    m->GetMapView()->Refresh();
}

void ChangeZoneCommand::Undo(MainWindow* m)
{
    m->GetMap()->GetLayer(_layerIndex)->zones[_zoneIndex] = _oldZone;
    m->GetMapView()->Refresh();
}

//-----------------------------------------------------------------------------

DestroyZoneCommand::DestroyZoneCommand(uint layerIndex, uint zoneIndex)
    : _layerIndex(layerIndex)
    , _zoneIndex(zoneIndex)
{}

void DestroyZoneCommand::Do(MainWindow* m)
{
    Map::Layer* layer = m->GetMap()->GetLayer(_layerIndex);

    _oldZone = layer->zones[_zoneIndex];
    layer->zones.erase(layer->zones.begin() + _zoneIndex);
    m->GetMapView()->Refresh();
}

void DestroyZoneCommand::Undo(MainWindow* m)
{
    Map::Layer* layer = m->GetMap()->GetLayer(_layerIndex);

    layer->zones.insert(layer->zones.begin() + _zoneIndex, _oldZone);
    m->GetMapView()->Refresh();
}