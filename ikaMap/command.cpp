
#include "command.h"
#include "executor.h"
#include "mapview.h"
#include "tilesetview.h"

#include "map.h"
#include "tileset.h"

CompositeCommand::CompositeCommand()
{}

CompositeCommand::CompositeCommand(const std::vector<Command*>& commands)
    : _commands(commands)
{}

CompositeCommand::~CompositeCommand()
{
    for (uint i = 0; i < _commands.size(); i++)
        delete _commands[i];
}

int CompositeCommand::GetCount() const
{
    return _commands.size();
}

Command* CompositeCommand::GetIndex(uint i)
{
    if (i < 0 || i >= _commands.size())
        return 0;
    else
        return _commands[i];
}

void CompositeCommand::Append(Command* cmd)
{
    _commands.push_back(cmd);
}

void CompositeCommand::Do(Executor* e)
{
    for (uint i = 0; i < _commands.size(); i++)
        _commands[i]->Do(e);
}

void CompositeCommand::Undo(Executor* e)
{
    uint i = _commands.size() - 1;
    do
    {
        _commands[i]->Undo(e);
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

void SetTileCommand::Do(Executor* e)
{
    Map* map = e->GetMap();

    _oldTileIndex = map->GetLayer(_layerIndex)->tiles(_tileX, _tileY);
    map->GetLayer(_layerIndex)->tiles(_tileX, _tileY) = _tileIndex;
    e->tilesSet.fire(MapEvent(map, _layerIndex));
}

void SetTileCommand::Undo(Executor* e)
{
    Map* map = e->GetMap();

    map->GetLayer(_layerIndex)->tiles(_tileX, _tileY) = _oldTileIndex;
    e->tilesSet.fire(MapEvent(map, _layerIndex));
}

//-----------------------------------------------------------------------------

PasteTilesCommand::PasteTilesCommand(int x, int y, uint layerIndex, const Matrix<uint>& tiles)
    : _x(x)
    , _y(y)
    , _layerIndex(layerIndex)
    , _tiles(tiles)
{}

void PasteTilesCommand::Do(Executor* e)
{
    Matrix<uint>& dest = e->GetMap()->GetLayer(e->GetCurrentLayer())->tiles;

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

    e->tilesSet.fire(MapEvent(e->GetMap(), _layerIndex));
}

void PasteTilesCommand::Undo(Executor* e)
{
    // teehee.  Swapping is neat.
    Do(e);
}

//-----------------------------------------------------------------------------

SetObstructionCommand::SetObstructionCommand(uint x, uint y, uint layerIndex, u8 set)
    : _x(x)
    , _y(y)
    , _layerIndex(layerIndex)
    , _set(set)
{}

void SetObstructionCommand::Do(Executor* e)
{
    Matrix<u8>& obs = e->GetMap()->GetLayer(_layerIndex)->obstructions;

    _oldValue = obs(_x, _y);
    obs(_x, _y) = _set;
    e->tilesSet.fire(MapEvent(e->GetMap(), _layerIndex));
}

void SetObstructionCommand::Undo(Executor* e)
{
    e->GetMap()->GetLayer(_layerIndex)->obstructions(_x, _y) = _oldValue;
    e->tilesSet.fire(MapEvent(e->GetMap(), _layerIndex));
}

//-----------------------------------------------------------------------------

void CreateLayerCommand::Do(Executor* e)
{
    Map* map = e->GetMap();

    std::string layName = "New Layer";

    uint i = 1;
    while (map->LayerIndex(layName) != -1)
        layName = va("New Layer %i", ++i);

    _layerIndex = map->NumLayers();

    map->AddLayer(layName, 100, 100); // arbitrary initial size for now

    e->layerCreated.fire(MapEvent(map, _layerIndex));
}

void CreateLayerCommand::Undo(Executor* e)
{
    e->GetMap()->DestroyLayer(_layerIndex);

    e->layerDestroyed.fire(MapEvent(e->GetMap(), _layerIndex));
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

void DestroyLayerCommand::Do(Executor* e)
{
    Map* map = e->GetMap();

    _savedLayer = new Map::Layer(*map->GetLayer(_index)); // save the layer so we can restore it later
    map->DestroyLayer(_index);

    e->layerDestroyed.fire(MapEvent(map, _index));
}

void DestroyLayerCommand::Undo(Executor* e)
{
    e->GetMap()->InsertLayer(_savedLayer, _index);
    _savedLayer = 0;

    e->layerCreated.fire(MapEvent(e->GetMap(), _index));
}

//-----------------------------------------------------------------------------

SwapLayerCommand::SwapLayerCommand(uint i1, uint i2)
    : _index1(i1)
    , _index2(i2)
{}

void SwapLayerCommand::Do(Executor* e)
{
    Map* map = e->GetMap();
    MapView* mapView = e->GetMapView();

    mapView->Freeze();

    uint curLayer = e->GetCurrentLayer();
    if (curLayer == _index1)        e->SetCurrentLayer(_index2);
    else if (curLayer == _index2)   e->SetCurrentLayer(_index1);

    map->SwapLayers(_index1, _index2);
    
    mapView->Thaw();
    
    e->layersReordered.fire(MapEvent(map));
}

void SwapLayerCommand::Undo(Executor* e)
{
    Do(e);  // muahaha
}

//-----------------------------------------------------------------------------

CloneLayerCommand::CloneLayerCommand(uint index)
    : _index(index)
{}

void CloneLayerCommand::Do(Executor* e)
{
    Map* map = e->GetMap();

    Map::Layer* newLay = new Map::Layer(*map->GetLayer(_index));
    map->InsertLayer(newLay, _index + 1);

    e->layerCreated.fire(MapEvent(map, _index));
}

void CloneLayerCommand::Undo(Executor* e)
{
    e->GetMap()->DestroyLayer(_index + 1);

    e->layerDestroyed.fire(MapEvent(e->GetMap(), _index));
}

//-----------------------------------------------------------------------------

ResizeLayerCommand::ResizeLayerCommand(uint index, uint newWidth, uint newHeight)
    : _index(index)
    , _newWidth(newWidth)
    , _newHeight(newHeight)
    , _savedLayer(0)
{}

void ResizeLayerCommand::Do(Executor* e)
{
    Map::Layer* lay = e->GetMap()->GetLayer(_index);

    if (!_savedLayer)
        _savedLayer = new Map::Layer(*lay);
    lay->Resize(_newWidth, _newHeight);

    e->layerResized.fire(MapEvent(e->GetMap(), _index));
}

void ResizeLayerCommand::Undo(Executor* e)
{
    // nontrivial.  heh.
    *e->GetMap()->GetLayer(_index) = *_savedLayer;

    e->layerResized.fire(MapEvent(e->GetMap(), _index));
}

//-----------------------------------------------------------------------------

ChangeLayerPropertiesCommand::ChangeLayerPropertiesCommand(
    Map::Layer* layer,
    const std::string& label, 
    bool wrapx,
    bool wrapy,
    int x,
    int y,
    float parallax_x,
    float parallax_y)
    : _layer(layer)
    , _newProperties(label, wrapx, wrapy, x, y, parallax_x, parallax_y)
{}

void ChangeLayerPropertiesCommand::Do(Executor* e)
{
    // Save the old state
    _oldProperties = LayerProperties(_layer->label, _layer->wrapx, _layer->wrapy, _layer->x, _layer->y,
        (float)_layer->parallax.mulx / (float)_layer->parallax.divx, 
        (float)_layer->parallax.muly / (float)_layer->parallax.divy);

    _layer->label         = _newProperties.label;
    _layer->wrapx         = _newProperties.wrapx;
    _layer->wrapy         = _newProperties.wrapy;
    _layer->x             = _newProperties.x;
    _layer->y             = _newProperties.y;
    _layer->parallax.divx = 1000;
    _layer->parallax.divy = 1000;
    _layer->parallax.mulx = _newProperties.parallax_x * 1000;
    _layer->parallax.muly = _newProperties.parallax_y * 1000;

    e->layerPropertiesChanged.fire(MapEvent(e->GetMap()));
}

void ChangeLayerPropertiesCommand::Undo(Executor* e)
{
    _layer->label = _oldProperties.label;
    _layer->wrapx = _oldProperties.wrapx;
    _layer->wrapy = _oldProperties.wrapy;
    _layer->x     = _oldProperties.x;
    _layer->y     = _oldProperties.y;
    _layer->parallax.divx = 1000;
    _layer->parallax.divy = 1000;
    _layer->parallax.mulx = _oldProperties.parallax_x * 1000;
    _layer->parallax.muly = _oldProperties.parallax_y * 1000;

    e->layerPropertiesChanged.fire(MapEvent(e->GetMap()));
}

//-----------------------------------------------------------------------------

ChangeMapPropertiesCommand::ChangeMapPropertiesCommand(const std::string& newTitle, uint newWidth, uint newHeight)
    : _newTitle(newTitle)
    , _newWidth(newWidth)
    , _newHeight(newHeight)
{}

void ChangeMapPropertiesCommand::Do(Executor* e)
{
    Map* map = e->GetMap();

    _oldTitle = map->title;     map->title = _newTitle;
    _oldWidth = map->width;     map->width = _newWidth;
    _oldHeight = map->height;   map->height = _newHeight;

    e->mapPropertiesChanged.fire(MapEvent(e->GetMap()));
}

void ChangeMapPropertiesCommand::Undo(Executor* e)
{
    Map* map = e->GetMap();
    map->title = _oldTitle;
    map->width = _oldWidth;
    map->height = _oldHeight;

    e->mapPropertiesChanged.fire(MapEvent(e->GetMap()));
}

//-----------------------------------------------------------------------------

ChangeEntityPropertiesCommand::ChangeEntityPropertiesCommand(
    uint layerIndex, uint entityIndex, Map::Entity newData)
    : _layerIndex(layerIndex)
    , _entityIndex(entityIndex)
    , _newData(newData)
{}

void ChangeEntityPropertiesCommand::Do(Executor* e)
{
    Map* map = e->GetMap();
    Map::Layer* lay = map->GetLayer(_layerIndex);

    _oldData = lay->entities[_entityIndex];

    lay->entities[_entityIndex] = _newData;

    e->entitiesChanged.fire(MapEvent(map, _layerIndex));
}

void ChangeEntityPropertiesCommand::Undo(Executor* e)
{
    Map* map = e->GetMap();
    Map::Layer* lay = map->GetLayer(_layerIndex);

    lay->entities[_entityIndex] = _oldData;

    e->entitiesChanged.fire(MapEvent(map, _layerIndex));
}

//-----------------------------------------------------------------------------

CreateEntityCommand::CreateEntityCommand(uint layerIndex, int x, int y)
    : _layerIndex(layerIndex)
    , _x(x)
    , _y(y)
{}

void CreateEntityCommand::Do(Executor* e)
{
    Map::Layer* lay = e->GetMap()->GetLayer(_layerIndex);
    _entityIndex = lay->entities.size();
    lay->entities.push_back(Map::Entity());
    lay->entities[_entityIndex].x = _x;
    lay->entities[_entityIndex].y = _y;

    e->entitiesChanged.fire(MapEvent(e->GetMap(), _layerIndex));
}

void CreateEntityCommand::Undo(Executor* e)
{
    Map::Layer* lay = e->GetMap()->GetLayer(_layerIndex);
    lay->entities.erase(lay->entities.begin() + _entityIndex);

    e->entitiesChanged.fire(MapEvent(e->GetMap(), _layerIndex));
}

//-----------------------------------------------------------------------------

DestroyEntityCommand::DestroyEntityCommand(uint layerIndex, uint entityIndex)
    : _layerIndex(layerIndex)
    , _entityIndex(entityIndex)
{}

void DestroyEntityCommand::Do(Executor* e)
{
    Map::Layer* lay = e->GetMap()->GetLayer(_layerIndex);
    
    _oldData = lay->entities[_entityIndex];
    
    lay->entities.erase(lay->entities.begin() + _entityIndex);

    e->entitiesChanged.fire(MapEvent(e->GetMap(), _layerIndex));
}

void DestroyEntityCommand::Undo(Executor* e)
{
    Map::Layer* lay = e->GetMap()->GetLayer(_layerIndex);
    lay->entities.insert(lay->entities.begin() + _entityIndex, _oldData);

    e->entitiesChanged.fire(MapEvent(e->GetMap(), _layerIndex));
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

void ChangeTileSetCommand::Do(Executor* e)
{
    std::string s = _fileName;
    TileSet* t    = _tileSet;

    _tileSet  = e->GetTileSet();
    _fileName = e->GetMap()->tileSetName;

    e->SwitchTileSet(t);
    e->GetMap()->tileSetName = s;
}

void ChangeTileSetCommand::Undo(Executor* e)
{
    Do(e);  // a swap undoes a swap
}

//-----------------------------------------------------------------------------

InsertTilesCommand::InsertTilesCommand(uint pos, std::vector<Canvas >& tiles)
    : _startPos(pos)
{
    _tiles.reserve(tiles.size());
    for (uint i = 0; i < tiles.size(); i++)
        _tiles.push_back(tiles[i]);
}

void InsertTilesCommand::Do(Executor* e)
{
    TileSet* ts = e->GetTileSet();

    for (uint i = 0; i < _tiles.size(); i++)
        ts->InsertTile(_startPos + i, _tiles[i]);

    e->tileSetChanged.fire(TileSetEvent(e->GetTileSet()));
}

void InsertTilesCommand::Undo(Executor* e)
{
    TileSet* ts = e->GetTileSet();

    for (uint i = 0; i < _tiles.size(); i++)
        ts->DeleteTile(ts->Count() - 1);

    e->tileSetChanged.fire(TileSetEvent(e->GetTileSet()));
}

//-----------------------------------------------------------------------------

DeleteTilesCommand::DeleteTilesCommand(uint firstTile, uint lastTile)
    : _firstTile(firstTile)
    , _lastTile(lastTile)
{
    assert(firstTile <= lastTile);
}

void DeleteTilesCommand::Do(Executor* e)
{
    TileSet* ts = e->GetTileSet();

    _savedTiles.reserve(_lastTile - _firstTile + 1);

    for (uint i = _firstTile; i <= _lastTile; i++)
    {
        _savedTiles.push_back(ts->Get(_firstTile));     // save the tile
        ts->DeleteTile(_firstTile);                     // then nuke it
    }

    e->tileSetChanged.fire(TileSetEvent(e->GetTileSet()));
}

void DeleteTilesCommand::Undo(Executor* e)
{
    TileSet* ts = e->GetTileSet();

    for (uint i = _firstTile; i <= _lastTile; i++)
    {
        ts->InsertTile(i, _savedTiles[i]);
    }

    e->tileSetChanged.fire(TileSetEvent(e->GetTileSet()));
}

//-----------------------------------------------------------------------------

ResizeTileSetCommand::ResizeTileSetCommand(uint newWidth, uint newHeight)
    : _newWidth(newWidth)
    , _newHeight(newHeight)
{}

void ResizeTileSetCommand::Do(Executor* e)
{
    TileSet* ts = e->GetTileSet();

    _oldWidth = ts->Width();
    _oldHeight = ts->Height();

    _savedTiles.clear();
    _savedTiles.reserve(ts->Count());
    for (uint i = 0; i < ts->Count(); i++)
    {
        _savedTiles.push_back(ts->Get(i));
    }

    ts->New(_newWidth, _newHeight);

    e->tileSetChanged.fire(TileSetEvent(e->GetTileSet()));
}

void ResizeTileSetCommand::Undo(Executor* e)
{
    TileSet* ts = e->GetTileSet();

    ts->New(_oldWidth, _oldHeight);
    for (uint i = 0; i < _savedTiles.size(); i++)
        ts->AppendTile(_savedTiles[i]);

    _savedTiles.clear();    // free up all that memory.  We don't need it anymore anyway.

    e->tileSetChanged.fire(TileSetEvent(e->GetTileSet()));
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

void DefineZoneBluePrintCommand::Do(Executor* e)
{
    switch (_mode)
    {
    case create:    // fallthrough
    case update:    e->GetMap()->zones[_newZone.label] = _newZone;    break;  // already have the old zone data
    case destroy:   e->GetMap()->zones.erase(_oldZone.label);   break;        
    }
}

void DefineZoneBluePrintCommand::Undo(Executor* e)
{
    switch (_mode)
    {
    case create:    e->GetMap()->zones.erase(_newZone.label);       break;
    case update:    // fallthrough
    case destroy:   e->GetMap()->zones[_oldZone.label] = _oldZone;  break;
    }
}

//-----------------------------------------------------------------------------

PlaceZoneCommand::PlaceZoneCommand(const Rect& position, uint layerIndex, const std::string& bluePrint)
    : _position(position)
    , _layerIndex(layerIndex)
    , _bluePrint(bluePrint)
{}

void PlaceZoneCommand::Do(Executor* e)
{
    Map::Layer::Zone z;
    z.position = _position;

    e->GetMap()->GetLayer(_layerIndex)->zones.push_back(z);
    
    e->zonesChanged.fire(MapEvent(e->GetMap(), _layerIndex));
}

void PlaceZoneCommand::Undo(Executor* e)
{
    e->GetMap()->GetLayer(_layerIndex)->zones.pop_back();

    e->zonesChanged.fire(MapEvent(e->GetMap(), _layerIndex));
}

//-----------------------------------------------------------------------------

ChangeZoneCommand::ChangeZoneCommand(uint layerIndex, uint zoneIndex, const Map::Layer::Zone& newZone)
    : _layerIndex(layerIndex)
    , _zoneIndex(zoneIndex)
    , _newZone(newZone)
{}

void ChangeZoneCommand::Do(Executor* e)
{
    Map::Layer* layer = e->GetMap()->GetLayer(_layerIndex);
    _oldZone = layer->zones[_zoneIndex];
    layer->zones[_zoneIndex] = _newZone;
    e->zonesChanged.fire(MapEvent(e->GetMap(), _layerIndex));
}

void ChangeZoneCommand::Undo(Executor* e)
{
    e->GetMap()->GetLayer(_layerIndex)->zones[_zoneIndex] = _oldZone;
    e->zonesChanged.fire(MapEvent(e->GetMap(), _layerIndex));
}

//-----------------------------------------------------------------------------

DestroyZoneCommand::DestroyZoneCommand(uint layerIndex, uint zoneIndex)
    : _layerIndex(layerIndex)
    , _zoneIndex(zoneIndex)
{}

void DestroyZoneCommand::Do(Executor* e)
{
    Map::Layer* layer = e->GetMap()->GetLayer(_layerIndex);

    _oldZone = layer->zones[_zoneIndex];
    layer->zones.erase(layer->zones.begin() + _zoneIndex);
    e->zonesChanged.fire(MapEvent(e->GetMap(), _layerIndex));
}

void DestroyZoneCommand::Undo(Executor* e)
{
    Map::Layer* layer = e->GetMap()->GetLayer(_layerIndex);

    layer->zones.insert(layer->zones.begin() + _zoneIndex, _oldZone);
    e->zonesChanged.fire(MapEvent(e->GetMap(), _layerIndex));
}

//-----------------------------------------------------------------------------

UpdateTileAnimStrandCommand::UpdateTileAnimStrandCommand(uint index, const VSP::AnimState& newStrand)
    : _newStrand(newStrand)
    , _index(index)
{}

void UpdateTileAnimStrandCommand::Do(Executor* e)
{
    VSP::AnimState& strand = e->GetTileSet()->GetAnim()[_index];
    _oldStrand = strand;
    strand = _newStrand;
}

void UpdateTileAnimStrandCommand::Undo(Executor* e)
{
    e->GetTileSet()->GetAnim()[_index] = _oldStrand;
}
