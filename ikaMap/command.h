#ifndef COMMAND_H
#define COMMAND_H

#include "types.h"
#include "map.h"
#include "Canvas.h"

struct Executor;

/**
 * Encapsulates some operation that changes the map somehow.
 * MainWindow holds a list of these to implement undo and redo.
 * It works pretty well.
 *
 * If the number of commands grows too much, move them all into 
 * their own separate source files, so we don't wind up with a
 * single monstrous source file.
 *
 * I wonder if I should put this all in a namespace.
 */
struct Command
{
public:
    virtual void Do(Executor* e) = 0;
    virtual void Undo(Executor* e) = 0;

    virtual ~Command(){}
};

/**
 * A group of commands.  Used to pack more than one command into a single
 * undo step.
 */
struct CompositeCommand : Command
{
private:
    std::vector<Command*> _commands;

public:
    CompositeCommand();
    CompositeCommand(const std::vector<Command*>& commands);
    ~CompositeCommand();

    int GetCount() const;
    Command* GetIndex(uint i);
    void Append(Command* cmd);

    virtual void Do(Executor* e);
    virtual void Undo(Executor* e);
};

/**
 * Real easy; a command to set a single tile somewhere.
 */
struct SetTileCommand : Command
{
private:
    uint _tileX, _tileY;
    uint _layerIndex;
    uint _tileIndex;
    uint _oldTileIndex;

public:
    SetTileCommand(uint tx, uint ty, uint li, uint ti);

    virtual void Do(Executor* e);
    virtual void Undo(Executor* e);
};

struct PasteTilesCommand : Command
{
private:
    int _x, _y;
    uint _layerIndex;
    Matrix<uint> _tiles;    // we swap this with the old tiles when undoing.

public:
    PasteTilesCommand(int x, int y, uint layerIndex, const Matrix<uint>& tiles);

    virtual void Do(Executor* e);
    virtual void Undo(Executor* e);
};

struct SetObstructionCommand : Command
{
private:
    uint _x, _y;
    uint _layerIndex;
    u8  _set;
    u8  _oldValue;

public:
    SetObstructionCommand(uint x, uint y, uint layerIndex, u8 set);

    virtual void Do(Executor* e);
    virtual void Undo(Executor* e);
};

struct CreateLayerCommand : Command
{
private:
    uint _layerIndex;           // The index of the layer we created.

public:

    virtual void Do(Executor* e);
    virtual void Undo(Executor* e);
};

struct DestroyLayerCommand : Command
{
private:
    Map::Layer* _savedLayer;        // a copy of the erased layer
    uint _index;                    // index of the layer to be destroyed

public:
    DestroyLayerCommand(uint index);
    virtual ~DestroyLayerCommand();

    virtual void Do(Executor* e);
    virtual void Undo(Executor* e);
};

// Used for both moving layers up and down, since they're the same thing
struct SwapLayerCommand : Command
{
private:
    uint _index1;
    uint _index2;

public:
    SwapLayerCommand(uint i1, uint i2);

    virtual void Do(Executor* e);
    virtual void Undo(Executor* e);
};

struct CloneLayerCommand: Command
{
private:
    uint _index;    // index of the layer to clone.  The new layer is put directly after this one.

public:
    CloneLayerCommand(uint index);

    virtual void Do(Executor* e);
    virtual void Undo(Executor* e);
};

struct ResizeLayerCommand : Command
{
private:
    uint _index;
    uint _newWidth;
    uint _newHeight;
    Map::Layer* _savedLayer;    // copy of the layer, so that we can restore it later

public:
    ResizeLayerCommand(uint index, uint newWidth, uint newHeight);

    virtual void Do(Executor* e);
    virtual void Undo(Executor* e);
};

struct ChangeLayerPropertiesCommand : Command
{
    struct LayerProperties
    {
        std::string label;
        int x;
        int y;
        bool wrapx;
        bool wrapy;

        LayerProperties(const std::string& l = "", bool wx = false, bool wy = false, int X = 0, int Y = 0)
            : label(l)
            , wrapx(wx)
            , wrapy(wy)
            , x(X)
            , y(Y)
        {}
    };

    Map::Layer* _layer;
    LayerProperties _newProperties;
    LayerProperties _oldProperties;

public:
    ChangeLayerPropertiesCommand(
        Map::Layer* layer,
        const std::string& label,
        bool wrapx,
        bool wrapy,
        int x,
        int y);

    virtual void Do(Executor* e);
    virtual void Undo(Executor* e);
};

struct ChangeMapPropertiesCommand : Command
{
    std::string _oldTitle, _newTitle;
    uint _oldWidth, _newWidth;
    uint _oldHeight, _newHeight;

public:
    ChangeMapPropertiesCommand(const std::string& newTitle, uint newWidth, uint newHeight);

    virtual void Do(Executor* e);
    virtual void Undo(Executor* e);
};

struct ChangeEntityPropertiesCommand : Command
{
    uint _layerIndex;
    uint _entityIndex;
    Map::Entity _newData;

    Map::Entity _oldData;

public:
    ChangeEntityPropertiesCommand(uint layerIndex, uint entityIndex, Map::Entity newData);

    virtual void Do(Executor* e);
    virtual void Undo(Executor* e);
};

struct CreateEntityCommand : Command
{
    uint _layerIndex;
    uint _entityIndex;
    int  _x, _y;

public:
    CreateEntityCommand(uint layerIndex, int x, int y);

    virtual void Do(Executor* e);
    virtual void Undo(Executor* e);
};

struct DestroyEntityCommand : Command
{
    uint _layerIndex;
    uint _entityIndex;

    Map::Entity _oldData;

public:
    DestroyEntityCommand(uint layerIndex, uint entityIndex);

    virtual void Do(Executor* e);
    virtual void Undo(Executor* e);
};

struct ChangeTileSetCommand : Command
{
private:
    struct TileSet* _tileSet;
    std::string _fileName;

public:
    ChangeTileSetCommand(struct TileSet* tileSet, const std::string& fileName);
    virtual ~ChangeTileSetCommand();

    virtual void Do(Executor* e);
    virtual void Undo(Executor* e);
};

struct InsertTilesCommand : Command
{
private:
    std::vector<Canvas> _tiles;
    uint _startPos;

public:
    // FIXME?  All this canvas copying may get a bit slow.
    // Start throwing pointers around instead?
    InsertTilesCommand(uint startPos, std::vector<Canvas>& tiles);

    virtual void Do(Executor* e);
    virtual void Undo(Executor* e);
};

struct DeleteTilesCommand : Command
{
private:
    std::vector<Canvas> _savedTiles;
    uint _firstTile;    // first tile to nuke.
    uint _lastTile;     // last tile to nuke. (inclusive)

public:
    DeleteTilesCommand(uint firstTile, uint lastTile);

    virtual void Do(Executor* e);
    virtual void Undo(Executor* e);
};

struct ResizeTileSetCommand : Command
{
private:
    std::vector<Canvas> _savedTiles;
    uint _oldWidth;
    uint _oldHeight;
    uint _newWidth;
    uint _newHeight;

public:
    ResizeTileSetCommand(uint newWidth, uint newHeight);

    virtual void Do(Executor* e);
    virtual void Undo(Executor* e);
};

struct DefineZoneBluePrintCommand : Command
{
private:
    enum Mode
    {
        create,
        destroy,
        update
    };

    Map::Zone _newZone;
    Map::Zone _oldZone;

    Mode _mode;

public:
    // I am a bad, bad man.  But this allows me to pretend that references
    // behave like java or python, and pass 0 as a "null reference".  Hurray for
    // self delusion.
    DefineZoneBluePrintCommand(Map::Zone& newZone, Map::Zone& oldZone);
    DefineZoneBluePrintCommand(Map::Zone& newZone, int);
    DefineZoneBluePrintCommand(int, Map::Zone& oldZone);

    virtual void Do(Executor* e);
    virtual void Undo(Executor* e);
};

struct PlaceZoneCommand : Command
{
private:
    Rect        _position;
    std::string _bluePrint;
    uint        _layerIndex;

public:
    PlaceZoneCommand(const Rect& position, uint layerIndex, const std::string& bluePrint);

    virtual void Do(Executor* e);
    virtual void Undo(Executor* e);
};

struct ChangeZoneCommand : Command
{
private:
    uint _layerIndex;
    uint _zoneIndex;

    Map::Layer::Zone _oldZone;
    Map::Layer::Zone _newZone;

public:
    ChangeZoneCommand(uint layerIndex, uint zoneIndex, const Map::Layer::Zone& newZone);

    virtual void Do(Executor* e);
    virtual void Undo(Executor* e);
};

//-----------------------------------------------------------------------------

struct DestroyZoneCommand : Command
{
private:
    uint _layerIndex;
    uint _zoneIndex;

    Map::Layer::Zone _oldZone;

public:
    DestroyZoneCommand(uint layerIndex, uint zoneIndex);

    virtual void Do(Executor* e);
    virtual void Undo(Executor* e);
};

//-----------------------------------------------------------------------------

#include "vsp.h"

struct UpdateTileAnimStrandCommand : Command
{
public:
    UpdateTileAnimStrandCommand(uint index, const VSP::AnimState& newStrand);

    virtual void Do(Executor* m);
    virtual void Undo(Executor* m);

private:
    VSP::AnimState _newStrand;
    VSP::AnimState _oldStrand;
    uint _index;
};

#endif