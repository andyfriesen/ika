/*
	Entity stuff.

	AI has to be processed in the main module.  So!  This will have lots of handy-dandy methods for getting
	info about the entity, so that the amount of processing done by the engine is kept to a minimum.
*/

#include "types.h"
#include "strk.h"
#include "map.h"

const entspeed_normal=100;

class CEntity
{
public:
	string_k	curanimscript;										// a copy of the last frame animation script assigned to this entity
	int			animscriptidx;										// the index of this strand from the CHR
	int			animscriptofs;										// current offset in the current anim script
	int			animscriptct;										// delay counter
	
	string_k	curmovescript;										// a copy of the last movement script assigned to this entity
	int			movescriptofs;										// the current position within the script.
	int			movescriptct;										// delay counter for move scripts

	Direction	thedirectionImgoinginnow;							// WYSIWYG

	int		x,y;													// world coordinates of the entity
	int		nSpeed;													// Speed of the entity (Number of ticks of AI per second.  100 is the default)
	int		nSpeedcount;											// Temp variable to keep track of whether or not an AI tick should be processed.

	int		hSprite;												// Index to which sprite is to be used
	
	Direction	facing;												// the direction the entity is facing
	bool    bMoving;												// true if the entity is moving
	int		nFramect;												// frame counter
	int		nCurframe;												// the frame that the engine should render
	int		nSpecframe;												// 0 if the engine should use normal frame progression, the frame that should be drawn otherwise
	bool	bVisible;												// true if the entity should be rendered
	bool	bMapobs;												// if true, the entity cannot walk on obstructed map tiles
	bool	bEntobs;												// if true, the entity cannot walk on entities whose bIsobs flag is set
	bool	bIsobs;													// if true, the entity obstructs entities whose bEntobs flag is set

	string_k	sName;												// the entity's name

	// Behavior flags and variables
	MoveCode	movecode;											// Describes how the entity behaves (see the mc_xxxx enums)
	int		nWandersteps;											// wander*:			How many pixels an entity will wander for
	int		nWanderdelay;											// wander*:			How long the entity delays between moves
	Rect	wanderrect;												// wanderrect:		the rect that the entity is restricted to.
	int		nWanderzone;											// wanderzone:		the zone that the entity is restricted to
	int		nEntchasetarget;										// chasing:			the entity that this entity is chasing
	int		nMinchasedist;											// chasing:			how close to the target the entity wants to be

	bool	bAdjacentactivate;										// if this is true, the entity should activate whenever it's adjacent to the player.
	string_k	sActscript;											// event to be called when the entity is activated

	CEntity();
	CEntity(const Sv2entity& e);									// converting old-style entity struct
	CEntity(const SMapEntity& e);									// converting map entities

	void	Init();													// does any setup type thingies that need to be done
	void	Free();													// cleanup

	void		UpdateAnimation();									// update the entity's frame based on its active animation script
	Direction	GetMoveScriptCommand();								// what do I want to do next?
	void	SetAnimScript(const string_k& newscript,int idx);		// makes the entity animate according to the specified script (if animscriptidx!=idx)
	void	SetMoveScript(const string_k& newscript);				// makes the entity move according to the specified script
};
