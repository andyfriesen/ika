/*
  Main module thingie.

  This is the part that takes all the pieces, and whips them around as necessary.

  Note that throughout this engine, I use the term "world coords" often.  It's just
  pixel coordinates relative to the upper left corner of the current map.  Tile
  coordinates are poopy. :)
*/

#ifndef MAIN_H
#define MAIN_H

#define VERSION "ika"

// low level components/containers/etc..
#include <list>
#include "log.h"
#include "misc.h"
#include "graph.h"
#include "input.h"
#include "timer.h"

// engine components
#include "script.h"
#include "graphlib.h"
#include "sound.h"
#include "map.h"
#include "char.h"
#include "entity.h"
#include "font.h"
#include "controller.h"
#include "v_config.h"

// Do Not Remove - Begin Insert 11
//#include "comm.h"
// Do Not Remove - End   Insert 11

class CEngine
{
	friend LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	friend class CScriptEngine;

	enum
	{
		b_up=0,
		b_down,
		b_left,
		b_right,
		b_enter,
		b_cancel,
	};

protected:
	HWND hWnd;
	HINSTANCE hInst;

	SUserConfig				cfg;

	Map						map;						// tile placement and stuff
	CTileSet				tiles;						// Images.  Of Tiles.
	CScriptEngine			script;						// c0de

	CCharacterController	characters;					// CHR files
	CController<CEntity>	entities;					// you know what this is, I hope
	CController<CFont>		font;						// because I sure as hell don't!

	Timer					timer;						// timer-based callback type stuff
	Input					input;						// keyboard/mouse (todo: joystick)

	bool					bKillFlag;					// set to true if a certain something hits the fan
	bool					bActive;					// set to false if we're supposed to sleep
	bool					bMaploaded;					// true if a map is... loaded. -_-

	int						xwin,ywin;					// world coordinates of the viewport
	int						player;						// player handle (-1 if not applicable)
	int						hCameratarget;				// the camera points to this entity (equal to -1 if the camera is free)

	// Odds and ends
	handle					hRenderdest;
	void*					pBindings[nControls];		// key bindings
	std::list<void*>		pHookretrace;				// list of functions to be executed every retrace
	std::list<void*>		pHooktimer;					// list of functions to be executed every tick
	
	// Interface (TODO: class wrap this elsewhere? a la Sphere?)
	void Sys_Error(const char* errmsg);					// bitches, and quits
	void Script_Error();								// also bitchy and quitty
	int  CheckMessages();								// Play nice with Mr. Gates
	
	void GameTick();									// 1/100th of a second's worth of AI
	void CheckKeyBindings();							// checks to see if any bound keys are pressed

	// Entity handling
	int  EntityAt(int x,int y,int w,int h);				// index of entity within the specified rect, or -1 if none
	bool DetectMapCollision(CEntity* e,int x1,int y1,int w,int h);
	int  DetectEntityCollision(int x1,int y1,int w,int h,int entidx);
	void ProcessEntities();								// one tick of AI for each entity
	void ProcessEntity(int entidx);						// one tick of AI for one entity
	Direction HandlePlayer();							// one tick of player-controlledness
	Direction HandleWanderingEntity(CEntity& ent);		// entity wanders for one tick
	Direction HandleChasingEntity(CEntity& ent);		// entity spends one tick chasing another entity

	void TestActivate();								// checks to see if the player has talked to an entity, stepped on a zone, etc...

	void RenderEntities();								// Draws entities
	void RenderLayer(int lay,bool transparent);			// renders a single layer
	void Render(const char* sTemprstring=NULL);			// renders everything

	void LoadMap(const char* filename);					// switches maps

	void HookTimer();									// does junk to keep hooked scripts running at the proper rate
	void HookRetrace();									// calls each hookretraced script exactly once (if applicable)
public:
	void Startup(HWND hwnd, HINSTANCE hinst);			// Inits the engine
	void Shutdown();									// deinits the engine
	void MainLoop();									// runs the engine
};

#endif