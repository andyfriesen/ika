#ifndef EDITSTATE_H
#define EDITSTATE_H

#include "map.h"

class wxMouseEvent;
class MainWindow;
class MapView;
class TileSetView;
class TileSet;
class Command;

/**
 * Encapsulates the editor's current behaviour.  There are
 * EditStates for tile editing, copy/paste mode, entity editing,
 * and so forth.
 *
 * Not quite an interface, because it's more convenient if EditState
 * can do some simple things on its own. (mostly accessors to keep
 * the code brief)
 */
class EditState
{
private:
    MainWindow* _mainWindow;

public:
    EditState(MainWindow* mw);
    virtual ~EditState(){}

    virtual void OnMouseDown(wxMouseEvent&) = 0;
    virtual void OnMouseUp(wxMouseEvent&) = 0;
    virtual void OnMouseMove(wxMouseEvent&) = 0;
    virtual void OnMouseWheel(wxMouseEvent&) {}
    virtual void OnRender() {}

protected:
    // Accessors for ease and whatnot.
    MainWindow*     GetMainWindow() const;
    MapView*        GetMapView() const;
    TileSetView*    GetTileSetView() const;
    Map*            GetMap() const;
    Map::Layer*     GetCurrentLayer() const;
    TileSet*        GetTileSet() const;
    void            HandleCommand(::Command* cmd);
};

#endif