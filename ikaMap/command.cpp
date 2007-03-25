
#include "command.h"
#include "executor.h"
#include "mapview.h"
#include "tilesetview.h"

#include "common/log.h"

#include "common/map.h"
#include "common/matrix.h"
#include "tileset.h"

CompositeCommand::CompositeCommand()
{}

CompositeCommand::CompositeCommand(const std::vector<Command*>& commands)
    : _commands(commands)
{}

CompositeCommand::~CompositeCommand() {
    for (uint i = 0; i < _commands.size(); i++) {
        delete _commands[i];
    }
}

int CompositeCommand::GetCount() const {
    return _commands.size();
}

Command* CompositeCommand::GetIndex(uint i) {
    if (i < 0 || i >= _commands.size()) {
        return 0;
    } else {
        return _commands[i];
    }
}

void CompositeCommand::Append(Command* cmd) {
    _commands.push_back(cmd);
}

void CompositeCommand::Do(Executor* e) {
    for (uint i = 0; i < _commands.size(); i++) {
        _commands[i]->Do(e);
    }
}

void CompositeCommand::Undo(Executor* e) {
    uint i = _commands.size() - 1;
    do {
        _commands[i]->Undo(e);
    } while (i-- != 0);
}
//-----------------------------------------------------------------------------



SetBrushCommand::SetBrushCommand(uint tx, uint ty, uint li, const Matrix<uint>& brush)
    : _tileX(tx)
    , _tileY(ty)
    , _layerIndex(li)
    , _curBrush(brush)
    , _oldBrush(brush.Width(), brush.Height())
{}

void SetBrushCommand::Do(Executor* e) {
    Map* map = e->GetMap();
    Map::Layer* layer = map->GetLayer(_layerIndex);

    for (uint y = 0; y < _curBrush.Height(); y++) {
        for (uint x = 0; x < _curBrush.Width(); x++) {
            _oldBrush(x, y) = layer->tiles(_tileX + x, _tileY + y);
            layer->tiles(_tileX + x, _tileY + y) = _curBrush(x, y);
        }
    }

    e->tilesSet.fire(MapEvent(map, _layerIndex));
}

void SetBrushCommand::Undo(Executor* e) {
    Map* map = e->GetMap();
    Map::Layer* layer = map->GetLayer(_layerIndex);
    for (uint y = 0; y < _oldBrush.Height(); y ++) {
        for (uint x = 0; x < _oldBrush.Width(); x ++) {
            layer->tiles(_tileX + x, _tileY + y) = _oldBrush(x, y);
        }
    }

    e->tilesSet.fire(MapEvent(map, _layerIndex));
}



//-----------------------------------------------------------------------------

SetTileCommand::SetTileCommand(uint tx, uint ty, uint li, uint ti)
    : _tileX(tx)
    , _tileY(ty)
    , _layerIndex(li)
    , _tileIndex(ti)
    , _oldTileIndex(0)
{}

void SetTileCommand::Do(Executor* e) {
    Map* map = e->GetMap();

    _oldTileIndex = map->GetLayer(_layerIndex)->tiles(_tileX, _tileY);
    map->GetLayer(_layerIndex)->tiles(_tileX, _tileY) = _tileIndex;
    e->tilesSet.fire(MapEvent(map, _layerIndex));
}

void SetTileCommand::Undo(Executor* e) {
    Map* map = e->GetMap();

    map->GetLayer(_layerIndex)->tiles(_tileX, _tileY) = _oldTileIndex;
    e->tilesSet.fire(MapEvent(map, _layerIndex));
}

//-----------------------------------------------------------------------------

PasteBrushCommand::PasteBrushCommand(int x, int y, uint layerIndex, const Brush& brush)
    : _x(x)
    , _y(y)
    , _layerIndex(layerIndex)
    , _brush(brush)
{}

void PasteBrushCommand::Do(Executor* e) {
    Map* map = e->GetMap();
    Map::Layer* lay = map->GetLayer(_layerIndex);
    Matrix<uint>& tiles = lay->tiles;
    Matrix<u8>& obs = lay->obstructions;

    for (uint y = 0; y < _brush.Height(); y++) {
        for (uint x = 0; x < _brush.Width(); x++) {
            Brush::Tile& tile = _brush.tiles(x, y);
            if (tile.mask) {
                uint index = tile.index;
                u8 obstruct = tile.obstruct;

                tile.index = tiles(x + _x, y + _y);
                tiles(x + _x, y + _y) = index;

                tile.obstruct = obs(x + _x, y + _y);
                obs(x + _x, y + _y) = obstruct;

                //swap(tile.index, tiles(x + _x, y + _y));
                //swap(tile.obstruct, obs(x + _x, y + _y));
            }
        }
    }

    e->tilesSet.fire(MapEvent(map, _layerIndex));
}

void PasteBrushCommand::Undo(Executor* e) {
    // yay swappage
    Do(e);
}

//-----------------------------------------------------------------------------

PasteTilesCommand::PasteTilesCommand(int x, int y, uint layerIndex, const Matrix<uint>& tiles)
    : _x(x)
    , _y(y)
    , _layerIndex(layerIndex)
    , _tiles(tiles)
{}

void PasteTilesCommand::Do(Executor* e) {
    Matrix<uint>& dest = e->GetMap()->GetLayer(e->GetCurrentLayer())->tiles;

    for (uint y = 0; y < _tiles.Height(); y++) {
        const uint destY = y + _y;
        uint destX = _x;

        for (uint x = 0; x < _tiles.Width(); x++) {
            // Our tile buffer becomes the undo buffer.
            swap(_tiles(x, y), dest(destX, destY));
            destX++;
        }
    }

    e->tilesSet.fire(MapEvent(e->GetMap(), _layerIndex));
}

void PasteTilesCommand::Undo(Executor* e) {
    // Swapping undoes a swap
    Do(e);
}

//-----------------------------------------------------------------------------

SetObstructionCommand::SetObstructionCommand(uint x, uint y, uint layerIndex, u8 set)
    : _x(x)
    , _y(y)
    , _layerIndex(layerIndex)
    , _set(set)
{}

void SetObstructionCommand::Do(Executor* e) {
    Matrix<u8>& obs = e->GetMap()->GetLayer(_layerIndex)->obstructions;

    _oldValue = obs(_x, _y);
    obs(_x, _y) = _set;
    e->tilesSet.fire(MapEvent(e->GetMap(), _layerIndex));
}

void SetObstructionCommand::Undo(Executor* e) {
    e->GetMap()->GetLayer(_layerIndex)->obstructions(_x, _y) = _oldValue;
    e->tilesSet.fire(MapEvent(e->GetMap(), _layerIndex));
}

//-----------------------------------------------------------------------------

void CreateLayerCommand::Do(Executor* e) {
    Map* map = e->GetMap();

    std::string layName = "New Layer";

    // Make sure we have a unique layer name.
    uint i = 1;
    while (map->LayerIndex(layName) != -1) {
        layName = va("New Layer %i", ++i);
    }

    _layerIndex = map->NumLayers();

    // Get the currently selected layer's width and height to use for the new layer.
    Map::Layer* layer = map->GetLayer(e->GetCurrentLayer());
    uint w = layer->Width();
    uint h = layer->Height();
    map->AddLayer(layName, w, h); // arbitrary initial size for now

    e->layerCreated.fire(MapEvent(map, _layerIndex));
}

void CreateLayerCommand::Undo(Executor* e) {
    e->GetMap()->DestroyLayer(_layerIndex);

    e->layerDestroyed.fire(MapEvent(e->GetMap(), _layerIndex));
}

//-----------------------------------------------------------------------------

DestroyLayerCommand::DestroyLayerCommand(uint index)
    : _index(index)
    , _savedLayer(0)
{}

DestroyLayerCommand::~DestroyLayerCommand() {
    delete _savedLayer;
}

void DestroyLayerCommand::Do(Executor* e) {
    Map* map = e->GetMap();

    _savedLayer = new Map::Layer(*map->GetLayer(_index)); // save the layer so we can restore it later
    map->DestroyLayer(_index);

    e->layerDestroyed.fire(MapEvent(map, _index));
}

void DestroyLayerCommand::Undo(Executor* e) {
    e->GetMap()->InsertLayer(_savedLayer, _index);
    _savedLayer = 0;

    e->layerCreated.fire(MapEvent(e->GetMap(), _index));
}

//-----------------------------------------------------------------------------

SwapLayerCommand::SwapLayerCommand(uint i1, uint i2)
    : _index1(i1)
    , _index2(i2)
{}

void SwapLayerCommand::Do(Executor* e) {
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

void SwapLayerCommand::Undo(Executor* e) {
    Do(e);  // muahaha
}

//-----------------------------------------------------------------------------

CloneLayerCommand::CloneLayerCommand(uint index)
    : _index(index)
{}

void CloneLayerCommand::Do(Executor* e) {
    Map* map = e->GetMap();

    Map::Layer* newLay = new Map::Layer(*map->GetLayer(_index));
    map->InsertLayer(newLay, _index + 1);

    e->layerCreated.fire(MapEvent(map, _index));
}

void CloneLayerCommand::Undo(Executor* e) {
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

void ResizeLayerCommand::Do(Executor* e) {
    Map::Layer* lay = e->GetMap()->GetLayer(_index);

    if (!_savedLayer)
        _savedLayer = new Map::Layer(*lay);
    lay->Resize(_newWidth, _newHeight);

    e->layerResized.fire(MapEvent(e->GetMap(), _index));
}

void ResizeLayerCommand::Undo(Executor* e) {
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
    int y)
    : _layer(layer)
    , _newProperties(label, wrapx, wrapy, x, y)
{}

void ChangeLayerPropertiesCommand::Do(Executor* e) {
    // Save the old state
    _oldProperties = LayerProperties(_layer->label, _layer->wrapx, _layer->wrapy, _layer->x, _layer->y);

    _layer->label = _newProperties.label;
    _layer->wrapx = _newProperties.wrapx;
    _layer->wrapy = _newProperties.wrapy;
    _layer->x     = _newProperties.x;
    _layer->y     = _newProperties.y;

    e->layerPropertiesChanged.fire(MapEvent(e->GetMap()));
}

void ChangeLayerPropertiesCommand::Undo(Executor* e) {
    _layer->label = _oldProperties.label;
    _layer->wrapx = _oldProperties.wrapx;
    _layer->wrapy = _oldProperties.wrapy;
    _layer->x     = _oldProperties.x;
    _layer->y     = _oldProperties.y;

    e->layerPropertiesChanged.fire(MapEvent(e->GetMap()));
}

//-----------------------------------------------------------------------------

ChangeMapPropertiesCommand::ChangeMapPropertiesCommand(const std::string& newTitle, uint newWidth, uint newHeight)
    : _newTitle(newTitle)
    , _newWidth(newWidth)
    , _newHeight(newHeight)
{}

void ChangeMapPropertiesCommand::Do(Executor* e) {
    Map* map = e->GetMap();

    _oldTitle = map->title;     map->title = _newTitle;
    _oldWidth = map->width;     map->width = _newWidth;
    _oldHeight = map->height;   map->height = _newHeight;

    e->mapPropertiesChanged.fire(MapEvent(e->GetMap()));
}

void ChangeMapPropertiesCommand::Undo(Executor* e) {
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

void ChangeEntityPropertiesCommand::Do(Executor* e) {
    Map* map = e->GetMap();
    Map::Layer* lay = map->GetLayer(_layerIndex);

    _oldData = lay->entities[_entityIndex];

    lay->entities[_entityIndex] = _newData;

    e->entitiesChanged.fire(MapEvent(map, _layerIndex));
}

void ChangeEntityPropertiesCommand::Undo(Executor* e) {
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

void CreateEntityCommand::Do(Executor* e) {
    Map::Layer* lay = e->GetMap()->GetLayer(_layerIndex);
    _entityIndex = lay->entities.size();
    lay->entities.push_back(Map::Entity());
    lay->entities[_entityIndex].x = _x;
    lay->entities[_entityIndex].y = _y;

    e->entitiesChanged.fire(MapEvent(e->GetMap(), _layerIndex));
}

void CreateEntityCommand::Undo(Executor* e) {
    Map::Layer* lay = e->GetMap()->GetLayer(_layerIndex);
    lay->entities.erase(lay->entities.begin() + _entityIndex);

    e->entitiesChanged.fire(MapEvent(e->GetMap(), _layerIndex));
}

//-----------------------------------------------------------------------------

DestroyEntityCommand::DestroyEntityCommand(uint layerIndex, uint entityIndex)
    : _layerIndex(layerIndex)
    , _entityIndex(entityIndex)
{}

void DestroyEntityCommand::Do(Executor* e) {
    Map::Layer* lay = e->GetMap()->GetLayer(_layerIndex);

    _oldData = lay->entities[_entityIndex];

    lay->entities.erase(lay->entities.begin() + _entityIndex);

    e->entitiesChanged.fire(MapEvent(e->GetMap(), _layerIndex));
}

void DestroyEntityCommand::Undo(Executor* e) {
    Map::Layer* lay = e->GetMap()->GetLayer(_layerIndex);
    lay->entities.insert(lay->entities.begin() + _entityIndex, _oldData);

    e->entitiesChanged.fire(MapEvent(e->GetMap(), _layerIndex));
}

//-----------------------------------------------------------------------------

ChangeTilesetCommand::ChangeTilesetCommand(Tileset* tileset, const std::string& fileName)
    : _tileset(tileset)
    , _fileName(fileName)
{}

ChangeTilesetCommand::~ChangeTilesetCommand() {
    delete _tileset;
}

void ChangeTilesetCommand::Do(Executor* e) {
    std::string s = _fileName;
    Tileset* t    = _tileset;

    _tileset  = e->GetTileset();
    _fileName = e->GetMap()->tilesetName;

    e->SwitchTileset(t);
    e->GetMap()->tilesetName = s;
}

void ChangeTilesetCommand::Undo(Executor* e) {
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

void InsertTilesCommand::Do(Executor* e) {
    Tileset* ts = e->GetTileset();

    for (uint i = 0; i < _tiles.size(); i++)
        ts->InsertTile(_startPos + i, _tiles[i]);

    e->tilesetChanged.fire(TilesetEvent(e->GetTileset()));
}

void InsertTilesCommand::Undo(Executor* e) {
    Tileset* ts = e->GetTileset();

    for (uint i = 0; i < _tiles.size(); i++)
        ts->DeleteTile(ts->Count() - 1);

    e->tilesetChanged.fire(TilesetEvent(e->GetTileset()));
}

//-----------------------------------------------------------------------------

DeleteTilesCommand::DeleteTilesCommand(uint firstTile, uint lastTile)
    : _firstTile(firstTile)
    , _lastTile(lastTile)
{
    assert(firstTile <= lastTile);
}

void DeleteTilesCommand::Do(Executor* e) {
    Tileset* ts = e->GetTileset();

    _savedTiles.reserve(_lastTile - _firstTile + 1);

    for (uint i = _firstTile; i <= _lastTile; i++) {
        _savedTiles.push_back(ts->Get(_firstTile));     // save the tile
        ts->DeleteTile(_firstTile);                     // then nuke it
    }

    e->tilesetChanged.fire(TilesetEvent(e->GetTileset()));
}

void DeleteTilesCommand::Undo(Executor* e) {
    Tileset* ts = e->GetTileset();

    for (uint i = _firstTile; i <= _lastTile; i++) {
        ts->InsertTile(i, _savedTiles[i]);
    }

    e->tilesetChanged.fire(TilesetEvent(e->GetTileset()));
}

//-----------------------------------------------------------------------------

ResizeTilesetCommand::ResizeTilesetCommand(uint newWidth, uint newHeight)
    : _newWidth(newWidth)
    , _newHeight(newHeight)
{}

void ResizeTilesetCommand::Do(Executor* e) {
    Tileset* ts = e->GetTileset();

    _oldWidth = ts->Width();
    _oldHeight = ts->Height();

    _savedTiles.clear();
    _savedTiles.reserve(ts->Count());
    for (uint i = 0; i < ts->Count(); i++) {
        _savedTiles.push_back(ts->Get(i));
    }

    ts->New(_newWidth, _newHeight);

    e->tilesetChanged.fire(TilesetEvent(e->GetTileset()));
}

void ResizeTilesetCommand::Undo(Executor* e) {
    Tileset* ts = e->GetTileset();

    ts->New(_oldWidth, _oldHeight);
    for (uint i = 0; i < _savedTiles.size(); i++)
        ts->AppendTile(_savedTiles[i]);

    _savedTiles.clear();    // free up all that memory.  We don't need it anymore anyway.

    e->tilesetChanged.fire(TilesetEvent(e->GetTileset()));
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

void DefineZoneBluePrintCommand::Do(Executor* e) {
    switch (_mode) {
    case create:    // fallthrough
    case update:    e->GetMap()->zones[_newZone.label] = _newZone;    break;  // already have the old zone data
    case destroy:   e->GetMap()->zones.erase(_oldZone.label);   break;
    }
}

void DefineZoneBluePrintCommand::Undo(Executor* e) {
    switch (_mode) {
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

void PlaceZoneCommand::Do(Executor* e) {
    Map::Layer::Zone z;
    z.position = _position;

    e->GetMap()->GetLayer(_layerIndex)->zones.push_back(z);

    e->zonesChanged.fire(MapEvent(e->GetMap(), _layerIndex));
}

void PlaceZoneCommand::Undo(Executor* e) {
    e->GetMap()->GetLayer(_layerIndex)->zones.pop_back();

    e->zonesChanged.fire(MapEvent(e->GetMap(), _layerIndex));
}

//-----------------------------------------------------------------------------

ChangeZoneCommand::ChangeZoneCommand(uint layerIndex, uint zoneIndex, const Map::Layer::Zone& newZone)
    : _layerIndex(layerIndex)
    , _zoneIndex(zoneIndex)
    , _newZone(newZone)
{}

void ChangeZoneCommand::Do(Executor* e) {
    Map::Layer* layer = e->GetMap()->GetLayer(_layerIndex);
    _oldZone = layer->zones[_zoneIndex];
    layer->zones[_zoneIndex] = _newZone;
    e->zonesChanged.fire(MapEvent(e->GetMap(), _layerIndex));
}

void ChangeZoneCommand::Undo(Executor* e) {
    e->GetMap()->GetLayer(_layerIndex)->zones[_zoneIndex] = _oldZone;
    e->zonesChanged.fire(MapEvent(e->GetMap(), _layerIndex));
}

//-----------------------------------------------------------------------------

DestroyZoneCommand::DestroyZoneCommand(uint layerIndex, uint zoneIndex)
    : _layerIndex(layerIndex)
    , _zoneIndex(zoneIndex)
{}

void DestroyZoneCommand::Do(Executor* e) {
    Map::Layer* layer = e->GetMap()->GetLayer(_layerIndex);

    _oldZone = layer->zones[_zoneIndex];
    layer->zones.erase(layer->zones.begin() + _zoneIndex);
    e->zonesChanged.fire(MapEvent(e->GetMap(), _layerIndex));
}

void DestroyZoneCommand::Undo(Executor* e) {
    Map::Layer* layer = e->GetMap()->GetLayer(_layerIndex);

    layer->zones.insert(layer->zones.begin() + _zoneIndex, _oldZone);
    e->zonesChanged.fire(MapEvent(e->GetMap(), _layerIndex));
}

//-----------------------------------------------------------------------------

UpdateTileAnimStrandCommand::UpdateTileAnimStrandCommand(uint index, const VSP::AnimState& newStrand)
    : _newStrand(newStrand)
    , _index(index)
{}

void UpdateTileAnimStrandCommand::Do(Executor* e) {
    VSP::AnimState& strand = e->GetTileset()->GetAnim()[_index];
    _oldStrand = strand;
    strand = _newStrand;
}

void UpdateTileAnimStrandCommand::Undo(Executor* e) {
    e->GetTileset()->GetAnim()[_index] = _oldStrand;
}
