#ifndef EDITSTATE_H
#define EDITSTATE_H

#include "map.h"

class wxWindow;
class wxMouseEvent;
class wxKeyEvent;
struct Executor;
struct MapView;
struct TileSetView;
struct TileSet;
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
struct EditState
{
    EditState(Executor* e);
    virtual ~EditState(){}

    virtual void OnMouseDown(wxMouseEvent&) = 0;
    virtual void OnMouseUp(wxMouseEvent&) = 0;
    virtual void OnMouseMove(wxMouseEvent&) = 0;

    // Optional things.  Override whichever are useful.
    virtual void OnBeginState() {}
    virtual void OnEndState() {}

    virtual void OnMouseWheel(wxMouseEvent&) {}
    virtual void OnRender() {}
    virtual void OnRenderCurrentLayer() {}
    virtual void OnSwitchLayers(uint oldLayer, uint newLayer) {}
    virtual void OnKeyPress(wxKeyEvent&) {}

protected:
    // Convenience accessors.
    Executor*       GetExecutor() const;
    MapView*        GetMapView() const;
    TileSetView*    GetTileSetView() const;
    Map*            GetMap() const;
    Map::Layer*     GetCurLayer() const;
    uint            GetCurLayerIndex() const;
    TileSet*        GetTileSet() const;
    wxWindow*       GetParentWindow() const;
    void            HandleCommand(::Command* cmd);

private:
    Executor*       _executor;
};

#endif