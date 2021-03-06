
#ifndef MAIN_H
#define MAIN_H

// Uncomment for Debug version 
#define DEBUG

// for the debug version
#include "debug.h"

// low level components/containers/etc..
#include <list>
#include <vector>
#include <set>

#include "common/log.h"
#include "common/utility.h"
#include "common/types.h"
#include "video/Driver.h"
#include "hooklist.h"
#include "path.h"

// engine components
#include "common/configfile.h"
#include "common/map.h"
#include "script.h"
#include "tileset.h"
#include "sound.h"
#include "sprite.h"
#include "entity.h"
#include "font.h"


/**
 *  Main module thingie.
 *
 *  This is the part that takes all the pieces, and whips them around as necessary.
 */

struct Engine {

    // This is not good.
    friend struct ScriptEngine;

    typedef std::list<Entity*>      EntityList;
    
public:                                                                             // Too many components need access to this class.  
    
    Map                             map;                                            ///< tile placement and stuff
    Tileset*                        tiles;                                          ///< Images.  Of Tiles.
    ScriptEngine                    script;
    
    SpriteController                sprite;                                         ///< sprite files
    EntityList                      entities;                                       ///< entities	

    Video::Driver*                  video;

	Entity*                         player;                                         ///< Points to the current player entity
	

    bool                            _showFramerate;                                 ///< The current framerate is printed in the upper left corner of the screen if true.
   
    
private:
    int                             xwin, ywin;                                     ///< world coordinates of the viewport
    int                             _oldTime;                                       ///< used for framerate regulation

public:
    Entity*                         cameraTarget;                                   ///< Points to the current camera target

	bool                            _isMapLoaded;                                   ///< true if a map is loaded (gah)

    std::vector<uint>               renderList;                                     ///< List of layer indeces to draw by default.
    
    // Odds and ends
    HookList                        _hookRetrace;
    HookList                        _hookTimer;

    bool                            _recurseStop;                                   ///< check variable used to ensure that Render is not called from within a hookretrace
    int                             _frameSkip;                                     ///< the map engine will skip no more than this amount of ticks per retrace
    
    // interface
    void      Sys_Error(const char* errmsg);                                        ///< complains, and quits
    void      Script_Error();                                                       ///< also complains and quits
    void      Script_Error(std::string msg);
    void      CheckMessages();                                                      ///< Play nice with Mr. Gates (or Torvalds, or Jobs, or...)
    
    void      GameTick();                                                           ///< 1/100th of a second's worth of AI
    void      CheckKeyBindings();                                                   ///< checks to see if any bound keys are pressed
    
    // Entity handling
    bool      DetectMapCollision(int x1, int y1, int w, int h, uint layerIndex);    ///< returns true if there is a map obstruction within the passed rect, on the specified layer
    
    /// If an entity is within the rect, return it, else return 0.  If wantobstructable 
    /// is true, then entities whose obstructEntities attribute is unset will be ignored.
    Entity*   DetectEntityCollision(const Entity* ent, 
                                    int x1, int y1, int w, int h, 
                                    uint layerIndex, bool wantobstructable = false);  
    void      ProcessEntities();                                                    ///< one tick of AI for each entity
    Map::Layer::Zone* TestZoneCollision(const Entity* ent);                         ///< returns the first zone touching the entity, or 0 if the entity touches no zones.
    void      TestActivate(const Entity* player);                                   ///< checks to see if the player has talked to an entity, stepped on a zone, etc...

    Entity*   SpawnEntity();                                                        ///< Creates an entity, and returns it
    void      DestroyEntity(Entity* e);                                             ///< Annihilates the entity

    void      RenderEntity(const Entity* ent);                                      ///< Renders an entity
    void      DrawEntity(const Entity* ent);                                        ///< Default way to render an entity (current frame, at x,y taking xwin/ywin into account etc etc)
    void      DrawEntity(const Entity* ent, int x, int y, uint frameIndex);
    void      RenderEntities(uint layerIndex);                                      ///< Draws entities
    void      RenderLayer(uint layerIndex);                                         ///< renders a single layer
    void      Render();                                                             ///< renders everything
    void      Render(const std::vector<uint>& list);                                ///< Renders the layers specified, in order.
    
    void      LoadMap(const std::string& filename);                                 ///< switches maps
    
    void      DoHook(HookList& hooklist);                                           ///< Calls every function in the list, then flushes any pending adds/removals from said list

    void      Startup(std::string& pathname);                                       ///< Inits the engine
    void      Shutdown();                                                           ///< deinits the engine
    void      MainLoop();                                                           ///< runs the engine

    Point     GetCamera();                                                          ///< Returns the position of the camera. (the point returned is the upper left corner)
    void      SetCamera(Point p);                                                   ///< Moves the camera to the position specified.  Any necessary clipping is performed.

    void      SyncTime();                                                           ///< Resets the internal timer used to regulate framerates.

    Engine();
};

#endif
