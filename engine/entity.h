/*
  Entity stuff.

  AI is done here.  Every entity has a pointer to the engine from which it was spawned, so it has
  access to the engine's state.

  Not sure if I like this, but it's better than it was before.
*/

#ifndef ENTITY_H
#define ENTITY_H

#include "types.h"
#include "strk.h"
#include "map.h"

const entspeed_normal=100;

class CEngine;
class CSprite;

class CEntity
{
    CEngine&    engine;                                            // engine instance.  This allows the entity to gather information about its surroundings
public:
    string_k    curanimscript;                                      // a copy of the last frame animation script assigned to this entity
    int         animscriptofs;                                      // current offset in the current anim script
    int         animscriptct;                                       // delay counter
    
    string_k    curmovescript;                                      // a copy of the last movement script assigned to this entity
    int         movescriptofs;                                      // the current position within the script.
    int         movescriptct;                                       // delay counter for move scripts
    
    int         x,y;                                                // world coordinates of the entity
    int         nSpeed;                                             // Speed of the entity (Number of ticks of AI per second.  100 is the default)
    int         nSpeedcount;                                        // Speed counter.
    
    CSprite*    pSprite;                                            // the sprite this entity uses
    
    Direction   direction;                                          // the direction the entity is facing (and moving in, if applicable)
    bool        bMoving;                                            // true if the entity is moving
    int         nCurframe;                                          // the frame that the engine should render
    int         nSpecframe;                                         // 0 if the engine should use normal frame progression, the frame that should be drawn otherwise
    bool        bVisible;                                           // true if the entity should be rendered
    bool        bMapobs;                                            // if true, the entity cannot walk on obstructed map tiles
    bool        bEntobs;                                            // if true, the entity cannot walk on entities whose bIsobs flag is set
    bool        bIsobs;                                             // if true, the entity obstructs entities whose bEntobs flag is set
    
    string_k    sName;                                              // the entity's name
    
    // Behavior flags and variables
    MoveCode    movecode;                                           // Describes how the entity behaves (see the mc_xxxx enums)
    int         nWandersteps;                                       // wander*:    How many pixels an entity will wander for
    int         nWanderdelay;                                       // wander*:    How long the entity delays between moves
    Rect        wanderrect;                                         // wanderrect: the rect that the entity is restricted to.
    int         nWanderzone;                                        // wanderzone: the zone that the entity is restricted to
    CEntity*    pChasetarget;                                       // chasing:    the entity that this entity is chasing
    int         nMinchasedist;                                      // chasing:    how close to the target the entity wants to be
    
    bool        bAdjacentactivate;                                  // if this is true, the entity should activate whenever it's adjacent to the player.
    string_k    sActscript;                                         // event to be called when the entity is activated
    
    CEntity(CEngine* njin);
    CEntity(CEngine* njin,const SMapEntity& e);                     // converting map entities
    
    void        Init();                                             // does any setup type thingies that need to be done
    void        Free();                                             // cleanup
    
    void        UpdateAnimation();                                  // update the entity's frame based on its active animation script
    void        SetAnimScript(const string_k& newscript);           // makes the entity animate according to the specified script (if animscriptidx!=idx)
    void        SetMoveScript(const string_k& newscript);           // makes the entity move according to the specified script

    void        SetFace(Direction d);                               // Makes the entity face the specified direction. (if it's currently moving, then it'll start moving in that direction instead)

    void        Stop();                                             // the entity stops moving, and stands still
    void        MoveDiagonally(Direction d);
    void        Move(Direction d);                                  // Cause the entity to try to move one pixel in a given direction

//----------------------------------------- AI -----------------------------------------------------
    Direction   HandlePlayer();
    Direction   GetMoveScriptCommand();                             // what do I want to do next?
    Direction   Wander();
    Direction   Chase();

    void        Update();
};

#endif