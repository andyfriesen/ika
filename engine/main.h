/*
    Main module thingie.

    This is the part that takes all the pieces, and whips them around as necessary.
  
    Note that throughout this engine, I use the term "world coords" often.  It's just
    pixel coordinates relative to the upper left corner of the current map.  Tile
    coordinates are poopy. :)
*/

#ifndef MAIN_H
#define MAIN_H

#define VERSION "ika 0.13"

// low level components/containers/etc..
#include <list>
#include "log.h"
#include "misc.h"
#include "graph.h"
#include "input.h"
#include "timer.h"

// engine components
#include "script.h"
#include "tileset.h"
#include "sound.h"
#include "map.h"
#include "sprite.h"
#include "entity.h"
#include "font.h"
#include "configfile.h"

class CEngine
{
    // friends suck.  Kill them all. -_-;
    friend LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    friend class CScriptEngine;

    typedef std::list<CEntity*>     EntityList;
    typedef EntityList::iterator    EntityIterator;
    
public:                                                                             // Too many components need access to this class.  Kinda sucks. :/
    HWND hWnd;
    HINSTANCE hInst;
    
    Map                             map;                                            // tile placement and stuff
    CTileSet                        tiles;                                          // Images.  Of Tiles.
    CScriptEngine                   script;                                         // c0de
    
    CSpriteController               sprite;                                         // CHR files (TODO: templatize the controller class, so that it can be used with other resources as well)
    EntityList                      entities;                                       // entities ;P
    
    Timer                           timer;                                          // timer-based callback type stuff
    Input                           input;                                          // keyboard/mouse (todo: joystick)
    
    bool                            bKillFlag;                                      // set to true if a certain something hits the fan
    bool                            bActive;                                        // set to false if we're supposed to sleep
    bool                            bMaploaded;                                     // true if a map is... loaded. -_-
    
    int                             xwin,ywin;                                      // world coordinates of the viewport
    CEntity*                        pPlayer;
    CEntity*                        pCameratarget;
    
    // Odds and ends
    handle                          hRenderdest;                                    // the current renderdest.  It's only needed here so the renderer knows how many tiles to draw. ;P
    void*                           pBindings[nControls];                           // key bindings
    std::list<void*>                pHookretrace;                                   // list of functions to be executed every retrace
    std::list<void*>                pHooktimer;                                     // list of functions to be executed every tick
    int                             nFrameskip;                                     // the map engine will skip no more than this amount of ticks per retrace
    
    // interface
    void      Sys_Error(const char* errmsg);                                             // bitches, and quits
    void      Script_Error();                                                            // also bitchy and quitty
    int       CheckMessages();                                                           // Play nice with Mr. Gates
    
    void      GameTick();                                                           // 1/100th of a second's worth of AI
    void      CheckKeyBindings();                                                   // checks to see if any bound keys are pressed
    
    // Entity handling
    bool      DetectMapCollision(int x1,int y1,int w,int h);
    CEntity*  DetectEntityCollision(const CEntity* ent,int x1,int y1,int w,int h);
    void      ProcessEntities();                                                    // one tick of AI for each entity
  
    void      TestActivate(const CEntity& player);                                  // checks to see if the player has talked to an entity, stepped on a zone, etc...

    CEntity*  SpawnEntity();
    void      DestroyEntity(CEntity* e);

    void      RenderEntities();                                                     // Draws entities
    void      RenderLayer(int lay,bool transparent);                                // renders a single layer
    void      Render(const char* sTemprstring=NULL);                                // renders everything
    
    void      LoadMap(const char* filename);                                        // switches maps
    
    void      HookTimer();                                                          // does junk to keep hooked scripts running at the proper rate
    void      HookRetrace();                                                        // calls each hookretraced script exactly once (if applicable)

    void      Startup(HWND hwnd, HINSTANCE hinst);                                  // Inits the engine
    void      Shutdown();                                                           // deinits the engine
    void      MainLoop();                                                           // runs the engine
};

#endif