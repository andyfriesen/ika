
#ifndef MAIN_H
#define MAIN_H

#define VERSION "0.48 experimental"

// low level components/containers/etc..
#include <list>

#include "common/log.h"
#include "common/misc.h"
#include "video/Driver.h"
#include "input.h"
#include "hooklist.h"

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

class CEngine
{
    // This sucks.
    friend class ScriptEngine;

    typedef std::list<Entity*>      EntityList;
    
public:                                                                             // Too many components need access to this class.  Kinda sucks. :/
    
    Map                             map;                                            ///< tile placement and stuff
    CTileSet*                       tiles;                                          ///< Images.  Of Tiles.
    ScriptEngine                    script;                                         ///< c0de
    
    CSpriteController               sprite;                                         ///< CHR files (TODO: templatize the controller class, so that it can be used with other resources as well)
    EntityList                      entities;                                       ///< entities ;P
public:
    
    Input                           input;                                          ///< keyboard/mouse (todo: joystick)
    Video::Driver*                  video;                                          ///< video. ;)

    bool                            _showFramerate;                                 ///< The current framerate is printed in the upper left corner of the screen if set.
    
    bool                            bKillFlag;                                      ///< set to true if a certain something hits the fan
    bool                            bActive;                                        ///< set to false if we're supposed to sleep
    bool                            bMaploaded;                                     ///< true if a map is... loaded. -_-
    
private:
    int                             xwin, ywin;                                     ///< world coordinates of the viewport

public:
    Entity*                         pPlayer;                                        ///< Points to the current player entity
    Entity*                         cameraTarget;                                   ///< Points to the current camera target
    
    // Odds and ends
    HookList                        _hookRetrace;
    HookList                        _hookTimer;
    int                             nFrameskip;                                     ///< the map engine will skip no more than this amount of ticks per retrace
    
    // interface
    void      Sys_Error(const char* errmsg);                                        ///< bitches, and quits
    void      Script_Error();                                                       ///< also bitchy and quitty
    void      CheckMessages();                                                      ///< Play nice with Mr. Gates
    
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
  
    void      TestActivate(const Entity* player);                                   ///< checks to see if the player has talked to an entity, stepped on a zone, etc...

    Entity*   SpawnEntity();                                                        ///< Creates an entity, and returns it
    void      DestroyEntity(Entity* e);                                             ///< Annihilates the entity

    void      RenderEntities(uint layerIndex);                                      ///< Draws entities
    void      RenderLayer(uint layerIndex);                                         ///< renders a single layer
    void      Render();                                                             ///< renders everything
    void      Render(const std::vector<uint>& list);                                ///< Renders the layers specified, in order.
    
    void      LoadMap(const std::string& filename);                                 ///< switches maps
    
    void      DoHook(HookList& hooklist);                                           ///< Calls every function in the list, then flushes any pending adds/removals from said list

    void      Startup();                                                            ///< Inits the engine
    void      Shutdown();                                                           ///< deinits the engine
    void      MainLoop();                                                           ///< runs the engine

    Point     GetCamera();                                                          ///< Returns the position of the camera. (the point returned is the upper left corner)
    void      SetCamera(Point p);                                                   ///< Moves the camera to the position specified.  Any necessary clipping is performed.

    CEngine();
};

#endif
