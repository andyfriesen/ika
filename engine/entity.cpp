
#include <math.h>

#include "common/misc.h"
#include "common/log.h"
#include "entity.h"
#include "sprite.h"
#include "main.h"

Entity::Entity(CEngine* njin)
    : engine              (*njin)
    , x                   (0)
    , y                   (0)
    , layerIndex          (0)
    , animscriptofs       (0)
    , animscriptct        (0)
    , speed               (entspeed_normal)
    , speedCount          (0)
    , sprite              (0)
    , direction           (face_down)
    , isMoving            (false)
    , isVisible           (true)
    
    , curFrame            (0)
    , specFrame           (-1)
{}

Entity::Entity(CEngine* njin, const Map::Entity& e, uint _layerIndex)
    : engine(*njin)
    , animscriptofs       (0)
    , animscriptct        (0)
    , x                   (e.x)
    , y                   (e.y)
    , layerIndex          (_layerIndex) // :x
    , name                (e.label)
    , speed               (e.speed)
    , speedCount          (0)
    , sprite              (0)
    , spriteName          (e.spriteName)
    , direction           ((Direction)e.direction)
    , isMoving            (false)
    , isVisible           (true)
    
    , curFrame            (0)
    , specFrame           (-1)
    
    , obstructsEntities   (e.obstructsEntities)
    , obstructedByMap     (e.obstructedByMap)
    , obstructedByEntities(e.obstructedByEntities)

    , adjActivateScript   (e.adjActivateScript)
    , activateScript      (e.activateScript)
{}

static uint get_int(const std::string& s, uint& offset)
// Grabs the next howevermany numerical characters from s, starting at offset.  On exit, offset is equal 
// to the next non-numeric character in the string.
{
    int start;
    start=offset;
    while (1)
    {
        if (s[offset]>='0' && s[offset]<='9')
            offset++;
        else
            return atoi(s.substr(start).c_str());
    }
}

void Entity::UpdateAnimation()
{
    if (animscriptct<1)
    {
        char c;
        if (curanimscript.length())											// is there a script at all?
        {
            do
            {
                c=curanimscript[animscriptofs++];							// get the next char
                if (animscriptofs >= curanimscript.length()) 
                {
                    animscriptofs = 0;	// wrap around					
                    c = curanimscript[animscriptofs++];
                }
            } while (c==' ');												// skip whitespace
            
            if (c>='a' && c<='z')	c^=32;									// force uppercase
            
            switch(c)
            {
            case 'F': 
                curFrame		 =get_int(curanimscript, animscriptofs);		// tee hee.  get_int also updates animscriptofs for us. :D
                break;
            case 'W': 
                animscriptct	+=get_int(curanimscript, animscriptofs);
                break;
            }
        }
    }
    else
        animscriptct--;
}

void Entity::SetAnimScript(const std::string& newscript)
{
    curanimscript=newscript;
    animscriptofs=0;
    animscriptct=0;
    UpdateAnimation();									// and immediately update the frame
}

void Entity::SetMoveScript(const std::string& newScript)
{
    moveScript = newScript;
}

void Entity::SetFace(Direction d)
{
    if (d==direction)
        return;
}

void Entity::Stop()
{
    if (!isMoving)
        return;

    isMoving=false;
    SetAnimScript(sprite->Script((int)direction+8));
}

// Handles all the nasty stuff required to make entities "slide" along a surface if they walk diagonally into it.
Direction Entity::MoveDiagonally(Direction d)
{
    Direction d1, d2;

    switch(d)
    {
    case face_upleft:       d1=face_up;   d2=face_left;     break;
    case face_upright:      d1=face_up;   d2=face_right;    break;
    case face_downleft:     d1=face_down; d2=face_left;     break;
    case face_downright:    d1=face_down; d2=face_right;    break;
    default:        
        return d;
    }

    int newx=x + (d2==face_left ? -1 : 1);
    int newy=y + (d1==face_up   ? -1 : 1);

    Entity* pEnt=0;

    if (obstructedByMap && engine.DetectMapCollision(x, newy, sprite->nHotw, sprite->nHoth, layerIndex))
         d1=face_nothing;
    else if (obstructedByEntities && engine.DetectEntityCollision(this, x, newy, sprite->nHotw, sprite->nHoth, layerIndex, true))
            d1=face_nothing;

    if (obstructedByMap && engine.DetectMapCollision(newx, y, sprite->nHotw, sprite->nHoth, layerIndex))
         d2=face_nothing;
    else if (obstructedByEntities && engine.DetectEntityCollision(this, newx, y, sprite->nHotw, sprite->nHoth, layerIndex, true))
         d2=face_nothing;

    if (d1==face_nothing)
        return d2;
    if (d2==face_nothing)
        return d1;

    if (d1==face_up)
    {
        if (d2==face_left)
            return face_upleft;
        else
            return face_upright;
    }
    else
    {
        if (d2==face_left)
            return face_downleft;
        else
            return face_downright;
    }
}

void Entity::Move(Direction d)
{
    Direction movedir=MoveDiagonally(d);    // bleh.  TODO: make this more elegant.
    Direction olddir=direction;

    direction=d;

    if (direction != olddir || !isMoving)
    {
        isMoving=true;
        SetAnimScript(sprite->Script((int)direction));
    }

    int newx=x, newy=y;

    switch (movedir)
    {
    case face_up:           newy--; break;
    case face_down:         newy++; break;
    case face_left:         newx--; break;  
    case face_right:        newx++; break;
    case face_nothing:      return;
       
    case face_upleft:       newy--; newx--; break;
    case face_upright:      newy--; newx++; break;
    case face_downleft:     newy++; newx--; break;
    case face_downright:    newy++; newx++; break;
    }

    if (obstructedByMap && engine.DetectMapCollision(newx, newy, sprite->nHotw, sprite->nHoth, layerIndex))
    {   Stop(); return; }

    if (obstructedByEntities)
    {
        Entity* pEnt = engine.DetectEntityCollision(this, newx, newy, sprite->nHotw, sprite->nHoth, layerIndex, true);
        if (pEnt && pEnt->obstructsEntities)
        {
            if (this==engine.pPlayer && pEnt->adjActivateScript.length() != 0)                                    // Adjacent activation
                engine.script.CallEvent(pEnt->adjActivateScript.c_str());
        
            Stop(); 
            return;
        }
    }

    x=newx; y=newy;
}

Direction Entity::GetMoveScriptCommand()
{
    //engine.script.CallEvent(moveScript); // not quite.  I want to pass the entity as an argument.
    return face_nothing;
}

Direction Entity::HandlePlayer()
{
    engine.TestActivate(this);
    
    Input& input=engine.input;

    if (input.Up())
    {
        if (input.Left())	return face_upleft;
        if (input.Right())	return face_upright;
        return face_up;
    }
    if (input.Down())
    {
        if (input.Left())	return face_downleft;
        if (input.Right())	return face_downright;
        return face_down;
    }
    if (input.Left())       return face_left;					// by this point, the diagonal possibilities are already taken care of
    if (input.Right())      return face_right;

    return face_nothing;
}

void Entity::Update()
{
    Direction newDir;

    UpdateAnimation();

    if (this == engine.pPlayer)
        newDir = HandlePlayer();
    else if (x == destLocation.x && y == destLocation.y)
        newDir = GetMoveScriptCommand();
    else
    {
        // move towards destLocation, on heading destVector. (oooo math)
        newDir = face_nothing;
    }

    if (newDir == face_nothing)
    {
        Stop();
        return;
    }

    Move(newDir);
}