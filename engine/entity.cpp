
#include <math.h>

#include "common/misc.h"
#include "common/log.h"
#include "entity.h"
#include "sprite.h"
#include "input.h"
#include "main.h"

Entity::Entity(Engine* njin)
    : engine              (*njin)
    , x                   (0)
    , y                   (0)
    , layerIndex          (0)
    , useSpecAnim         (false)
    , delayCount          (0)
    , speed               (entspeed_normal)
    , speedCount          (0)
    , sprite              (0)
    , direction           (face_down)
    , isMoving            (false)
    , isVisible           (true)
    , obstructsEntities   (true)
    , obstructedByMap     (true)
    , obstructedByEntities(true)
    
    , curFrame            (0)
    , specFrame           (-1)

    , moveScript(0)
    , activateScript(0)
    , adjActivateScript(0)
{}

Entity::Entity(Engine* njin, const Map::Entity& e, uint _layerIndex)
    : engine(*njin)
    , useSpecAnim(false)
    , delayCount          (0)
    , x                   (e.x)
    , y                   (e.y)
    , layerIndex          (_layerIndex) // :x
    , destLocation        (e.x, e.y)
    , destVector          (0, 0)
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

    // These can't be set here, because the entities are created before the script is loaded
    , moveScript(0)
    , activateScript(0)
    , adjActivateScript(0)
{}

static uint get_int(const std::string& s, uint& offset)
// Grabs the next howevermany numerical characters from s, starting at offset.  On exit, offset is equal 
// to the next non-numeric character in the string.
{
    int start = offset;

    while (s[offset] >= '0' && s[offset] <= '9' && offset < s.length())
        offset++;

    return atoi(s.substr(start, offset).c_str());
}

void Entity::UpdateAnimation()
{
    if (useSpecAnim)
        specAnim.update(1);
    else
        defaultAnim.update(1);
    if (specFrame != -1)
        curFrame = -1;
    else if (useSpecAnim)
        curFrame = specAnim.getCurFrame();
    else
        curFrame = defaultAnim.getCurFrame();
}

void Entity::SetAnimScript(const std::string& newScript)
{
    defaultAnim = AnimScript(newScript);
    UpdateAnimation();                                                                  // and immediately update the frame
}

void Entity::SetFace(Direction d)
{
    direction = d;
    Stop();
}

void Entity::Stop()
{
    destLocation.x = x;
    destLocation.y = y;
    destVector.x = 0;
    destVector.y = 0;

    if (!isMoving)
        return;

    isMoving = false;
    SetAnimScript(sprite->GetIdleScript(direction));
}

// Handles all the nasty stuff required to make entities "slide" along a surface if they walk diagonally into it.
// Returns the direction the entity should move in as a result of obstructions.
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
        isMoving = true;
        SetAnimScript(sprite->GetWalkScript(direction));
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
            if (this == engine.player && pEnt->adjActivateScript)  // Adjacent activation
                engine.script.ExecObject(pEnt->adjActivateScript);
        
            Stop(); 
            return;
        }
    }

    x=newx; y=newy;
}

void Entity::GetMoveScriptCommand()
{
    if (moveScript)
        engine.script.ExecObject(moveScript, this);
    else
        delayCount = 1000000;   // whatever.  The idea here is that the entity does nothing.  (so, we delay for a little under 3 hours before checking again, by default)
}

Direction Entity::HandlePlayer()
{
    engine.TestActivate(this);
    
    Input* const input = the<Input>();

    if (*input->up)
    {
        if (*input->left)    return face_upleft;
        if (*input->right)   return face_upright;
        return face_up;
    }
    if (*input->down)
    {
        if (*input->left)    return face_downleft;
        if (*input->right)   return face_downright;
        return face_down;
    }
    if (*input->left)        return face_left;                                   // by this point, the diagonal possibilities are already taken care of
    if (*input->right)       return face_right;

    return face_nothing;
}

void Entity::MoveTo(int mx, int my)
{
    destLocation.x = mx;
    destLocation.y = my;
    destVector.x = mx - x;
    destVector.y = my - y;
    delayCount = 0;
}

void Entity::Wait(uint time)
{
    Stop();
    delayCount = time;
}

void Entity::Update()
{
    Direction newDir = face_nothing;;

    UpdateAnimation();
    if (this == engine.player)
        newDir = HandlePlayer();

    if (delayCount == 0 && (x == destLocation.x && y == destLocation.y)) // Nothing to do?
        GetMoveScriptCommand();                                          // ask the script what we should do

    if (delayCount > 0)
    {
        delayCount--;
    }
    else if (destVector.x != 0 || destVector.y != 0)
    {
        // move towards destLocation, on heading destVector. (oooo math.  scary)

        int startX = destLocation.x - destVector.x;
        int startY = destLocation.y - destVector.y;

        int dx = x - destLocation.x;
        int dy = y - destLocation.y;

        // Trivial cases: Motion in the four cardinal directions
        if (dx == 0)
        {
            newDir =
                (y > destLocation.y) ? face_up :
                (y < destLocation.y) ? face_down :
                face_nothing;
        }
        else if (dy == 0)
        {
            newDir =
                (x > destLocation.x) ? face_left :
                (x < destLocation.x) ? face_right :
                face_nothing;
        }
        else
        {
            // General case: arbitrary direction (full 360 degrees)

            double m = (double)(destVector.y) / destVector.x;

            // typical y=mx+b BS
            // targetY is where the entity "should" be on the Y axis, given its current X coordinate.
            int targetY = (int)((x - startX) * m) + startY;
            // deltaY is simply how many pixels up or down the entity must move this tick. (quantity, not direction, as we take the absolute value)
            int deltaY = abs(y - targetY);
            
            // If deltaY is exactly one pixel (+/-), then we go diagonally.
            // If deltaY is zero, then we go left/right, then recalculate. (if it stays zero, then we just go left/right)
            // If deltaY is greater than one pixel, we go up/down

            if (deltaY == 0)
            {
                int _x;
                if (x > destLocation.x)
                {
                    newDir = face_left;
                    _x = x - 1;
                }
                else
                {
                    newDir = face_right;
                    _x = x + 1;
                }

                targetY = int((_x - startX) * m) + startY;
                deltaY = abs(y - targetY);
            }

            if (deltaY == 1)
            {
                // ternary p1mpin
                newDir =
                    (y > destLocation.y) ?
                        ((x > destLocation.x) ? face_upleft
                                              : face_upright)
                    :
                        ((x > destLocation.x) ? face_downleft
                                              : face_downright);
            }
            else if (deltaY > 1)
            {
                newDir =
                    (y > destLocation.y) ? face_up
                                         : face_down;
            }
        }
    }

    if (newDir == face_nothing)
    {
        Stop();
        return;
    }

    Move(newDir);
}
