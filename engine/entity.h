#ifndef ENTITY_H
#define ENTITY_H

#include "common/types.h"
#include "common/map.h"
#include "scriptobject.h"

//! Normal entity speed
const int entspeed_normal = 100;

class CEngine;
struct Sprite;

/*!
  Entity stuff.

  AI is done here.  Every entity has a pointer to the engine from which it was spawned, so it has
  access to the engine's state.

  I'm not sure if I like this, but it's better than it was before.
*/

struct Entity
{
    std::string curanimscript;                                      //!< a copy of the last frame animation script assigned to this entity
    uint        animscriptofs;                                      //!< current offset in the current anim script
    uint        animscriptct;                                       //!< delay counter
    
    std::string moveScript;                                         //!< Movement script assigned to this entity.
    
    int         x, y;                                               //!< coordinates of the entity
    uint        layerIndex;                                         //!< layer the entity inhabits
    Point       destVector;                                         //!< Direction the entity is going.
    Point       destLocation;                                       //!< coordinates the entity is walking towards.
    int         speed;                                              //!< Speed of the entity (Number of ticks of AI per second.  100 is the default)
    int         speedCount;                                         //!< Speed counter.
    uint        delayCount;                                         //!< Delay counter. (der)
    
    Sprite*     sprite;                                             //!< the sprite this entity uses
    std::string spriteName;                                         //!< filename of the sprite this entity uses.
    
    Direction   direction;                                          //!< the direction the entity is facing (and moving in, if applicable)
    bool        isMoving;                                           //!< true if the entity is moving
    uint        curFrame;                                           //!< the frame that the engine should render
    uint        specFrame;                                          //!< 0 if the engine should use normal frame progression, the frame that should be drawn otherwise
    bool        isVisible;                                          //!< true if the entity should be rendered
    bool        obstructedByMap;                                    //!< if true, the entity cannot walk on obstructed map tiles
    bool        obstructedByEntities;                               //!< if true, the entity cannot walk on entities whose bIsobs flag is set
    bool        obstructsEntities;                                  //!< if true, the entity obstructs entities whose bEntobs flag is set
    
    std::string name;                                               //!< the entity's name
    
    ScriptObject activateScript;                                    //!< event to be called when the entity is activated
    ScriptObject adjActivateScript;                                 //!< event to be called when the entity touches the player
    
    Entity(CEngine* njin);                                          //!< Default constructor
    Entity(CEngine* njin, const Map::Entity& e, uint _layerIndex);  //!< Converts a map entity
    
    void        Init();                                             //!< does any setup type thingies that need to be done
    void        Free();                                             //!< cleanup
    
    void        UpdateAnimation();                                  //!< update the entity's frame based on its active animation script
    void        SetAnimScript(const std::string& newScript);        //!< makes the entity animate according to the specified script
    void        SetMoveScript(const std::string& newScript);        //!< makes the entity move according to the specified script

    void        SetFace(Direction d);                               //!< Makes the entity face the specified direction. (and stop)

    void        Stop();                                             //!< the entity stops what it's doing, and stands still
    Direction   MoveDiagonally(Direction d);                        //!< Cheezy hack to handle the extra complications involved in moving entities diagonally.
    void        Move(Direction d);                                  //!< Cause the entity to try to move one pixel in a given direction

    //----------------------------------------- AI -----------------------------------------------------
    Direction   HandlePlayer();                                     //!< Gets the next command from the user input
    void        GetMoveScriptCommand();                             //!< Gets the next command from the move script

    void        MoveTo(int x, int y);                               //!< Commands the entity to walk to the given point.
    void        Wait(uint time);                                    //!< Commands the entity to stop what it's doing for the given time period.

    void        Update();                                           //!< Performs one tick of AI

private:
    CEngine&    engine;                                             //!< engine instance.  This allows the entity to gather information about its surroundings
};

#endif
