#ifndef EXECUTOR_H
#define EXECUTOR_H

// Keep dependancies to an absolute bare minimum!
// Otherwise we slaughter compile times any time this file changes.
#include "common/utility.h"
#include "common/listener.h"
#include "events.h"

class wxWindow; // -_-.  Okay because we don't have to actually include wx stuff.

struct Command;

struct Map;
struct Tileset;
struct SpriteSet;

struct MapView;
struct TileSetView;

/*
 * The Executor is the class that is responsible for actually doing most 
 * everything.  Executor accepts messages, and relays them to the components
 * that are subscribing to the appropriate listeners.
 *
 * I'm not completely happy with this, as I can tell already that it's going
 * to get mighty bloated.  But it's better tossing this around than the actual
 * main window class.
 *
 * Get methods never ever fire events.  Set methods can be assumed to fire
 * the appropriate event.
 */
struct Executor {
    virtual void HandleCommand(::Command* cmd) = 0;
    virtual void AddCommand(::Command* cmd) = 0;

    virtual void Undo() = 0;
    virtual void Redo() = 0;

    virtual bool IsLayerVisible(uint index) = 0;
    virtual void ShowLayer(uint index, bool show) = 0;
    inline  void ShowLayer(uint index) { ShowLayer(index, true);    }
    inline  void HideLayer(uint index) { ShowLayer(index, false);   }

    // Displays a dialog so the user can tweak the layer.
    virtual void EditLayerProperties(uint index) = 0;

    /*
     * It'd probably be best to discard the idea of a current tile altogether
     * and just use brushes for everything.
     */
    virtual uint GetCurrentTile() = 0;
    virtual void SetCurrentTile(uint i) = 0;

    virtual uint GetCurrentLayer() = 0;
    virtual void SetCurrentLayer(uint i) = 0;

    virtual void SetStatusBar(const std::string& text, int field = 1) = 0;

    // Mustn't use these to mutate!!  Send a command!
    virtual Map* GetMap() = 0;
    virtual Tileset* GetTileSet() = 0;
    virtual SpriteSet* GetSpriteSet(const std::string& filename) = 0;

    virtual MapView* GetMapView() = 0;
    virtual TileSetView* GetTileSetView() = 0;
    virtual wxWindow* GetParentWindow() = 0;

    // ---------- ASSUMES THAT THE CALLER WILL CLEAN UP THE OLD TILESET ------------
    // (this is because it is not always desirable to delete the old tileset; we may
    //  want it back later)
    virtual void SwitchTileSet(Tileset* ts) = 0;

    /*
     * I'm not a fan of implementation inheritance, but doing this
     * any other way will result in serious asspains.
     * 
     * Some sort of way to have heirarchical events is in order.  For instance, the MapView
     * could simple subscribe to mapChanged, and be notified when any change at all occurs.
     * Other controls could subscribe to more specific events.
     */

    Listener<const MapEvent&>  tilesSet;
    Listener<const MapEvent&>  obsSet;
    Listener<const MapEvent&>  zonesChanged;

    Listener<const MapEvent&>  layerCreated;            // arg is the index of the new layer
    Listener<const MapEvent&>  layerDestroyed;          // arg is the index of the layer that was destroyed
    Listener<const MapEvent&>  layersReordered;
    Listener<const MapEvent&>  layerPropertiesChanged;  // arg is the layer index
    Listener<const MapEvent&>  layerResized;            // ditto

    Listener<const MapEvent&>  entitiesChanged;
    Listener<const MapEvent&>  mapPropertiesChanged;

    Listener<const TileSetEvent&>  tilesImported;
    Listener<const TileSetEvent&>  tileSetChanged;

    Listener<const MapTileSetEvent&> mapLoaded;
    
    Listener<const MapEvent&>  mapVisibilityChanged;    // A layer has been hidden, or unhidden, or something.
    Listener<uint>             curLayerChanged;         // The current layer has changed.
    Listener<uint>             curTileChanged;
};

#endif
