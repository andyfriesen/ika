#ifndef COMMAND_H
#define COMMAND_H

#include "types.h"
#include "map.h"

class MainWindow; // bleh. ;P

/**
 * Encapsulates some operation that changes the map somehow.
 * MainWindow holds a list of these to implement undo and redo.
 * It works pretty well.
 *
 * If the number of commands grows too much, move them all into 
 * their own separate source files, so we don't wind up with a
 * single monstrous source file.
 */
class Command
{
public:
    virtual void Do(MainWindow* m) = 0;
    virtual void Undo(MainWindow* m) = 0;

    virtual ~Command(){}
};

/**
 * Real easy; a command to set a single tile somewhere.
 */
class SetTileCommand : public Command
{
private:
    uint _tileX, _tileY;
    uint _layerIndex;
    uint _tileIndex;
    uint _oldTileIndex;

public:
    SetTileCommand(uint tx, uint ty, uint li, uint ti);

    virtual void Do(MainWindow* m);
    virtual void Undo(MainWindow* m);
};

class SetObstructionCommand : public Command
{
private:
    uint _x, _y;
    uint _layerIndex;
    u8  _set;
    u8  _oldValue;

public:
    SetObstructionCommand(uint x, uint y, uint layerIndex, u8 set);

    virtual void Do(MainWindow* m);
    virtual void Undo(MainWindow* m);
};

class CreateLayerCommand : public Command
{
private:
    uint _layerIndex;           // The index of the layer we created.

public:

    virtual void Do(MainWindow* m);
    virtual void Undo(MainWindow* m);
};

class DestroyLayerCommand : public Command
{
private:
    Map::Layer* _savedLayer;        // a copy of the erased layer
    uint _index;                    // index of the layer to be destroyed

public:
    DestroyLayerCommand(uint index);
    virtual ~DestroyLayerCommand();

    virtual void Do(MainWindow* m);
    virtual void Undo(MainWindow* m);
};

// Used for both moving layers up and down, since they're the same thing
class SwapLayerCommand : public Command
{
private:
    uint _index1;
    uint _index2;

public:
    SwapLayerCommand(uint i1, uint i2);

    virtual void Do(MainWindow* m);
    virtual void Undo(MainWindow* m);
};

class ResizeLayerCommand : public Command
{
private:
    uint _index;
    uint _newWidth;
    uint _newHeight;
    Map::Layer* _savedLayer;    // copy of the layer, so that we can restore it later

public:
    ResizeLayerCommand(uint index, uint newWidth, uint newHeight);

    virtual void Do(MainWindow* m);
    virtual void Undo(MainWindow* m);
};

class ChangeLayerPropertiesCommand : public Command
{
    struct LayerProperties
    {
        std::string label;
        bool wrapx;
        bool wrapy;
        int x;
        int y;

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

    virtual void Do(MainWindow* m);
    virtual void Undo(MainWindow* m);
};

class ChangeMapPropertiesCommand : public Command
{
    std::string _oldTitle, _newTitle;
    uint _oldWidth, _newWidth;
    uint _oldHeight, _newHeight;

public:
    ChangeMapPropertiesCommand(const std::string& newTitle, uint newWidth, uint newHeight);

    virtual void Do(MainWindow* m);
    virtual void Undo(MainWindow* m);
};

class ChangeEntityPropertiesCommand : public Command
{
    uint _layerIndex;
    uint _entityIndex;
    Map::Layer::Entity _newData;
    Map::Entity _newBlueprint;

    Map::Layer::Entity _oldData;
    Map::Entity _oldBlueprint;

public:
    ChangeEntityPropertiesCommand(uint layerIndex, uint entityIndex, Map::Layer::Entity newData, Map::Entity newBlueprint);

    virtual void Do(MainWindow* m);
    virtual void Undo(MainWindow* m);
};

#endif