/*
 * I was actually testing ikaMap awhile ago when it became apparent just how much tedium is involved.
 * Setting border tiles, making sure everything is on the same layer, it's all a pain.
 *
 * So!
 *
 * We make ikaMap scriptable, and let people automate the task.
 * Here's how it works:
 *
 * A script is loaded up. (ooh, complex, that)
 * The script is free to implement certain functions:
 * 
 * OnMouseDown(x, y)  - called when the mouse is clicked in the map area.  x/y are the world pixel coordinates that the 
 *                      mouse click occurred at.
 * OnMouseUp(x, y)    - Same as above, but called when the mouse button is released.
 * OnMouseMove(x, y)  - Same, except when the mouse is moved. (whether any buttons are down or not)
 * OnMouseWheel(x, y, wheelDelta) - Same as above, but wheelDelta is added.  (it's the amount and direction of the 
 *                                  mousewheel movement
 * OnRender()         - Called after drawing the entire map.
 * OnRenderCurrentLayer() - Called after drawing the current layer.
 *
 * OnBeginState()     - Called when the script becomes active. (the current 'tool')
 * OnEndState()       - Called when the script's time is up. (the current tool was changed to something else)
 * OnSwitchLayers(oldLayer, newLayer) - Called if the current layer is switched while this script is active.
 *                                      oldLayer and newLayer are the indeces of the old layer and new, respectively.
 * OnKeyPress(keyCode) - Called when a key is pressed.  keyCode is an obscure number that signifies which key was pressed.
 *
 * OnActivated()      - If a script is not meant to be used as a painting tool, it should implement this.
 *                      Scripts that have an OnActivated method will be available in the Tools submenu of the editor.
 *
 * Next up, the interface.
 * For compatibility (and fun) both ikaMap and ika use a module called ika.  The ika.Map interface from ika is 
 * translated to ikaMap as faithfully as possible.  A separate reference will be available, as there are things
 * ikaMap can do that ika can't.
 *
 * ika.Map class members:
 *      title - the title of the map.  read/write
 *      layercount - Number of layers
 *      tilecount - Number of tiles (most definitely needs to be moved to a ika.TileSet object)
 *      tilewidth/tileheight - Size of the tileset. (ditto)
 *
 *      GetTile(x, y, layer)       - Returns the tile at x,y on the layer requested.
 *      SetTile(x, y, layer, tile) - Sets a tile at x,y on the layer.
 *      GetObs(x, y, layer)
 *      SetObs(x, y, layer, set)
 *      GetMetaData()
 *      SetMetaData(**kwargs)
 *      GetLayerName(layer)
 *      GetLayerSize(layer)
 *      FindLayerByName(name)
 *      GetParallax(layer)
 *      SetParallax(layer, mulx, divx, muly, divy)
 *
 *     Also stuff that ika doesn't have (yet, if ever)
 *      AddLayer(width, height)
 *      DestroyLayer(layer)
 *      ResizeLayer(layer, width, height)
 *      SwapLayers(layer1, layer2)  - Switches the two layers in the rendering order.  This is what you use to move layers behind or in front of other layers.
 *      CloneLayer(layer)           - Appends a copy of the layer to the map.
 *      GetLayerProperties(layer)   - returns a 5-tuple of (label, x, y, wrapx, wrapy)
 *      SetLayerProperties(layer, x, y, wrapx, wrapy)
 *     TODO: entity interface
 *     TODO: zone interface
 *     TODO: waypoint interface
 *
 * TODO: tileset interface
 *
 * ika.Editor class members:
 *      xwin/ywin
 *      curlayer
 *      curtile
 *      MapToTile(x, y, layer = -1) - Translates the x/y coordinates from world pixel coordinates to tile coordinates, on the layer specified.
 *                                    If layer is omitted, or an invalid value, the current layer is used.  A 2-tuple is returned (x,y)
 *      TileToMap(x, y, layer = -1) - Undoes MapToTile.  Returns an (x,y) pair.
 *      MouseState()                - Returns a 5-tuple containing (x, y, leftButton, middleButton, rightButton).  The latter three values are
 *                                    booleans, set to true if the mouse button is down.  x/y are map world coordinates, or None if the mouse is
 *                                    currently outside the map editing area. (this is broken.  all the mouse buttons are reported as being up)
 */

#ifndef SCRIPT_H
#define SCRIPT_H

#include <string>
#include "types.h"
#include "Python.h" // I *HATE* having to put this here.  I can't just proto PyObject because of gay C.

class Script
{
private:
    PyObject* module;

    PyObject* onMouseDown;
    PyObject* onMouseUp;
    PyObject* onMouseMove;
    PyObject* onMouseWheel;

    PyObject* onRender;
    PyObject* onRenderCurrentLayer;
    PyObject* onBeginState;
    PyObject* onEndState;
    PyObject* onSwitchLayers;
    PyObject* onActivated;
    //PyObject* onKeyPress;

    // Metadata!!
    std::string _name; // filename, I guess
    std::string _desc; // __doc__

public:
    Script(const std::string& fileName);
    ~Script();

    void OnMouseDown(int x, int y);
    void OnMouseUp(int x, int y);
    void OnMouseMove(int x, int y);
    void OnMouseWheel(int x, int y, int wheelDelta);

    void OnRender();
    void OnRenderCurrentLayer();
    void OnBeginState();
    void OnEndState();
    void OnSwitchLayers(uint oldLayer, uint newLayer);
    void OnActivated();
    //void OnKeyPress(uint keycode);

    std::string GetName() const;
    std::string GetDesc() const;

    bool IsTool() const;
    bool IsActivatable() const;

    void Reload();
    void Deallocate();

private:
    void ReportError();
};

#endif