#ifndef COMMAND_H
#define COMMAND_H

#include "common/utility.h"
#include "common/map.h"
#include "common/Canvas.h"
#include "common/matrix.h"

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
struct Command {
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
    CompositeCommand();
    CompositeCommand(const std::vector<Command*>& commands);
    ~CompositeCommand();

    int GetCount() const;
    Command* GetIndex(uint i);
    void Append(Command* cmd);

    virtual void Do(Executor* e);
    virtual void Undo(Executor* e);

private:
    std::vector<Command*> _commands;
};

struct SetBrushCommand : Command {
    SetBrushCommand(uint tx, uint ty, uint li, const Matrix<uint>& brush);
    virtual void Do(Executor* e);
    virtual void Undo(Executor* e);

private:
    uint _tileX, _tileY;
    uint _layerIndex;
    Matrix<uint> _curBrush;
    Matrix<uint> _oldBrush;
};


/**
 * Real easy; a command to set a single tile somewhere.
 */
struct SetTileCommand : Command {
    SetTileCommand(uint tx, uint ty, uint li, uint ti);

    virtual void Do(Executor* e);
    virtual void Undo(Executor* e);

private:
    uint _tileX, _tileY;
    uint _layerIndex;
    uint _tileIndex;
    uint _oldTileIndex;
};

struct PasteTilesCommand : Command {
    PasteTilesCommand(int x, int y, uint layerIndex, const Matrix<uint>& tiles);

    virtual void Do(Executor* e);
    virtual void Undo(Executor* e);

private:
    int _x, _y;
    uint _layerIndex;
    Matrix<uint> _tiles;    // we swap this with the old tiles when undoing.
};

struct SetObstructionCommand : Command {
    SetObstructionCommand(uint x, uint y, uint layerIndex, u8 set);

    virtual void Do(Executor* e);
    virtual void Undo(Executor* e);

private:
    uint _x, _y;
    uint _layerIndex;
    u8  _set;
    u8  _oldValue;
};

struct CreateLayerCommand : Command {
    virtual void Do(Executor* e);
    virtual void Undo(Executor* e);

private:
    uint _layerIndex;           // The index of the layer we created.
};

struct DestroyLayerCommand : Command {
    DestroyLayerCommand(uint index);
    virtual ~DestroyLayerCommand();

    virtual void Do(Executor* e);
    virtual void Undo(Executor* e);

private:
    Map::Layer* _savedLayer;        // a copy of the erased layer
    uint _index;                    // index of the layer to be destroyed
};

// Used for both moving layers up and down, since they're the same thing
struct SwapLayerCommand : Command {
    SwapLayerCommand(uint i1, uint i2);

    virtual void Do(Executor* e);
    virtual void Undo(Executor* e);

private:
    uint _index1;
    uint _index2;
};

struct CloneLayerCommand: Command {
    CloneLayerCommand(uint index);

    virtual void Do(Executor* e);
    virtual void Undo(Executor* e);

private:
    uint _index;    // index of the layer to clone.  The new layer is put directly after this one.
};

struct ResizeLayerCommand : Command {
    ResizeLayerCommand(uint index, uint newWidth, uint newHeight);

    virtual void Do(Executor* e);
    virtual void Undo(Executor* e);

private:
    uint _index;
    uint _newWidth;
    uint _newHeight;
    Map::Layer* _savedLayer;    // copy of the layer, so that we can restore it later
};

struct ChangeLayerPropertiesCommand : Command {
    ChangeLayerPropertiesCommand(
        Map::Layer* layer,
        const std::string& label,
        bool wrapx,
        bool wrapy,
        int x,
        int y);

    virtual void Do(Executor* e);
    virtual void Undo(Executor* e);

private:
    struct LayerProperties {
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
};

struct ChangeMapPropertiesCommand : Command {
    ChangeMapPropertiesCommand(const std::string& newTitle, uint newWidth, uint newHeight);

    virtual void Do(Executor* e);
    virtual void Undo(Executor* e);

private:
    std::string _oldTitle, _newTitle;
    uint _oldWidth, _newWidth;
    uint _oldHeight, _newHeight;
};

struct ChangeEntityPropertiesCommand : Command {
    ChangeEntityPropertiesCommand(uint layerIndex, uint entityIndex, Map::Entity newData);

    virtual void Do(Executor* e);
    virtual void Undo(Executor* e);

private:
    uint _layerIndex;
    uint _entityIndex;
    Map::Entity _newData;

    Map::Entity _oldData;
};

struct CreateEntityCommand : Command {
    CreateEntityCommand(uint layerIndex, int x, int y);

    virtual void Do(Executor* e);
    virtual void Undo(Executor* e);

private:
    uint _layerIndex;
    uint _entityIndex;
    int  _x, _y;
};

struct DestroyEntityCommand : Command {
    DestroyEntityCommand(uint layerIndex, uint entityIndex);

    virtual void Do(Executor* e);
    virtual void Undo(Executor* e);

public:
    uint _layerIndex;
    uint _entityIndex;

    Map::Entity _oldData;
};

struct ChangeTilesetCommand : Command {
    ChangeTilesetCommand(struct Tileset* tileset, const std::string& fileName);
    virtual ~ChangeTilesetCommand();

    virtual void Do(Executor* e);
    virtual void Undo(Executor* e);

private:
    struct Tileset* _tileset;
    std::string _fileName;
};

struct InsertTilesCommand : Command {
    // FIXME?  All this canvas copying may get a bit slow.
    // Start throwing pointers around instead?
    InsertTilesCommand(uint startPos, std::vector<Canvas>& tiles);

    virtual void Do(Executor* e);
    virtual void Undo(Executor* e);

private:
    std::vector<Canvas> _tiles;
    uint _startPos;
};

struct DeleteTilesCommand : Command {
    DeleteTilesCommand(uint firstTile, uint lastTile);

    virtual void Do(Executor* e);
    virtual void Undo(Executor* e);

private:
    std::vector<Canvas> _savedTiles;
    uint _firstTile;    // first tile to nuke.
    uint _lastTile;     // last tile to nuke. (inclusive)
};

struct ResizeTilesetCommand : Command {
    ResizeTilesetCommand(uint newWidth, uint newHeight);

    virtual void Do(Executor* e);
    virtual void Undo(Executor* e);

private:
    std::vector<Canvas> _savedTiles;
    uint _oldWidth;
    uint _oldHeight;
    uint _newWidth;
    uint _newHeight;
};

struct DefineZoneBluePrintCommand : Command {
    // I am a bad, bad man.  But this allows me to pretend that references
    // behave like java or python, and pass 0 as a "null reference".  Hurray for
    // self delusion.
    DefineZoneBluePrintCommand(Map::Zone& newZone, Map::Zone& oldZone);
    DefineZoneBluePrintCommand(Map::Zone& newZone, int);
    DefineZoneBluePrintCommand(int, Map::Zone& oldZone);

    virtual void Do(Executor* e);
    virtual void Undo(Executor* e);

private:
    enum Mode {
        create,
        destroy,
        update
    };

    Map::Zone _newZone;
    Map::Zone _oldZone;

    Mode _mode;
};

struct PlaceZoneCommand : Command {
    PlaceZoneCommand(const Rect& position, uint layerIndex, const std::string& bluePrint);

    virtual void Do(Executor* e);
    virtual void Undo(Executor* e);

private:
    Rect        _position;
    std::string _bluePrint;
    uint        _layerIndex;
};

struct ChangeZoneCommand : Command {
    ChangeZoneCommand(uint layerIndex, uint zoneIndex, const Map::Layer::Zone& newZone);

    virtual void Do(Executor* e);
    virtual void Undo(Executor* e);

private:
    uint _layerIndex;
    uint _zoneIndex;

    Map::Layer::Zone _oldZone;
    Map::Layer::Zone _newZone;
};

//-----------------------------------------------------------------------------

struct DestroyZoneCommand : Command {
    DestroyZoneCommand(uint layerIndex, uint zoneIndex);

    virtual void Do(Executor* e);
    virtual void Undo(Executor* e);

private:
    uint _layerIndex;
    uint _zoneIndex;

    Map::Layer::Zone _oldZone;
};

//-----------------------------------------------------------------------------

#include "common/vsp.h"

struct UpdateTileAnimStrandCommand : Command {
    UpdateTileAnimStrandCommand(uint index, const VSP::AnimState& newStrand);

    virtual void Do(Executor* m);
    virtual void Undo(Executor* m);

private:
    VSP::AnimState _newStrand;
    VSP::AnimState _oldStrand;
    uint _index;
};

#endif
