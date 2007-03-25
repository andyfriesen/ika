#ifndef EDITSTATE_H
#define EDITSTATE_H

#include "common/map.h"

class wxWindow;
class wxMouseEvent;
class wxKeyEvent;
struct Executor;
struct MapView;
struct TilesetView;
struct Tileset;
struct Command;

/**
 * Encapsulates the editor's current behaviour.  There are
 * EditStates for tile editing, copy/paste mode, entity editing,
 * and so forth.
 *
 * Not quite an interface, because it's more convenient if EditState
 * can do some simple things on its own. (mostly accessors to keep
 * the code brief)
 */
struct EditState {
    EditState(Executor* e, std::string name);
    virtual ~EditState(){}

    std::string GetName();

    virtual void OnMouseDown(wxMouseEvent&) = 0;
    virtual void OnMouseUp(wxMouseEvent&) = 0;
    virtual void OnMouseMove(wxMouseEvent&) = 0;

    // Optional things.  Override whichever are useful.
    virtual void OnBeginState() {}
    virtual void OnEndState() {}

    virtual void OnMouseWheel(wxMouseEvent&) {}
    virtual void OnRender() {}
    virtual void OnRenderCurrentLayer() {}
    virtual void OnTilesetViewRender() {}
    virtual void OnSwitchLayers(unsigned int oldLayer,
								unsigned int newLayer) {}
    virtual void OnKeyPress(wxKeyEvent&) {}

protected:
    // Convenience accessors.
    Executor*       GetExecutor() const;
    MapView*        GetMapView() const;
    TilesetView*    GetTilesetView() const;
    Map*            GetMap() const;
    Map::Layer*     GetCurLayer() const;
    uint            GetCurLayerIndex() const;
    Tileset*        GetTileset() const;
    wxWindow*       GetParentWindow() const;
    void            HandleCommand(::Command* cmd);

private:
    Executor*       _executor;
    std::string     _name;
};

#endif
