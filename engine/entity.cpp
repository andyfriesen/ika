#include "common/misc.h"
#include "common/log.h"
#include "entity.h"
#include "sprite.h"
#include "main.h"

CEntity::CEntity(CEngine* njin) :
    engine              (*njin),
    animscriptofs       (0),
    animscriptct        (0),
    movescriptofs       (0),
    movescriptct        (0),
    x                   (0),
    y                   (0),
    nSpeed              (entspeed_normal),
    nSpeedcount         (0),
    pSprite             (0),
    direction           (face_down),
    bMoving             (false),
    movecode            (mc_nothing),
    bVisible            (true),
    
    nCurframe           (0),
    nSpecframe          (0),
    
    nWandersteps        (0),
    nWanderdelay        (0),
    bAdjacentactivate   (false),
    bIsobs              (true),
    bMapobs             (true),
    bEntobs             (true)
{}

CEntity::CEntity(CEngine* njin,const SMapEntity& e) :
    engine(*njin),
    animscriptofs       (0),
    animscriptct        (0),
    movescriptofs       (0),
    movescriptct        (0),
    x                   (e.x),
    y                   (e.y),
    nSpeed              (e.nSpeed),
    nSpeedcount         (0),
    pSprite             (0),
    direction           ((Direction)e.direction),
    bMoving             (false),
    movecode            (e.state),
    bVisible            (true),
    
    nCurframe           (0),
    nSpecframe          (0),
    
    nWandersteps        (e.nWandersteps),
    nWanderdelay        (e.nWanderdelay),
    wanderrect          (e.wanderrect),
    bAdjacentactivate   (false),
    bIsobs              (e.bIsobs),
    bMapobs             (e.bMapobs),
    bEntobs             (e.bEntobs),

    sName               (e.sName),
    sActscript          (e.sActscript)
{}

static int get_int(const string& s,int& offset)
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

void CEntity::UpdateAnimation()
{
    if (animscriptct<1)
    {
        char c;
        if (curanimscript.length())											// is there a script at all?
        {
            do
            {
                c=curanimscript[animscriptofs++];							// get the next char
                if (animscriptofs>=curanimscript.length()) 
                {
                    animscriptofs=0;	// wrap around					
                    c=curanimscript[animscriptofs++];
                }
            } while (c==' ');												// skip whitespace
            
            if (c>='a' && c<='z')	c^=32;									// force uppercase
            
            switch(c)
            {
            case 'F': 
                nCurframe		 =get_int(curanimscript,animscriptofs);		// tee hee.  get_int also updates animscriptofs for us. :D
                break;
            case 'W': 
                animscriptct	+=get_int(curanimscript,animscriptofs);
                break;
            }
        }
    }
    else
        animscriptct--;
}

void CEntity::SetAnimScript(const string& newscript)
{
    curanimscript=newscript;
    animscriptofs=0;
    animscriptct=0;
    UpdateAnimation();									// and immediately update the frame
}

void CEntity::SetMoveScript(const string& newscript)
{
    curmovescript=newscript;
    movescriptofs=0;
    movescriptct=0;
}

void CEntity::SetFace(Direction d)
{
    if (d==direction)
        return;
}

void CEntity::Stop()
{
    if (!bMoving)
        return;

    bMoving=false;
    SetAnimScript(pSprite->Script((int)direction+8));
}

// Handles all the nasty stuff required to make entities "slide" along a surface if they walk diagonally into it.
Direction CEntity::MoveDiagonally(Direction d)
{
    Direction d1,d2;

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

    CEntity* pEnt=0;

    if (bMapobs && engine.DetectMapCollision(x,newy,pSprite->nHotw,pSprite->nHoth))
         d1=face_nothing;
    else if (bEntobs && engine.DetectEntityCollision(this,x,newy,pSprite->nHotw,pSprite->nHoth,true))
            d1=face_nothing;

    if (bMapobs && engine.DetectMapCollision(newx,y,pSprite->nHotw,pSprite->nHoth))
         d2=face_nothing;
    else if (bEntobs && engine.DetectEntityCollision(this,newx,y,pSprite->nHotw,pSprite->nHoth,true))
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

void CEntity::Move(Direction d)
{
    Direction movedir=MoveDiagonally(d);    // bleh.  TODO: make this more elegant.
    Direction olddir=direction;

    direction=d;

    if (direction!=olddir || !bMoving)
    {
        bMoving=true;
        SetAnimScript(pSprite->Script((int)direction));
    }

    int newx=x,newy=y;

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

    if (bMapobs && engine.DetectMapCollision(newx,newy,pSprite->nHotw,pSprite->nHoth))
    {   Stop(); return; }

    if (bEntobs)
    {
        CEntity* pEnt=engine.DetectEntityCollision(this,newx,newy,pSprite->nHotw,pSprite->nHoth);
        if (pEnt && pEnt->bIsobs)
        {
            if (this==engine.pPlayer && pEnt->bAdjacentactivate)                                    // Adjacent activation
                engine.script.CallEvent(pEnt->sActscript.c_str());
        
            Stop(); 
            return;
        }
    }

    x=newx; y=newy;
}

Direction CEntity::Wander()
{
    if (movescriptct<1)
    {
        if (!bMoving)
        {            
            movescriptct=nWandersteps;
            return (Direction)Random(0,4);
        }
        else
        {
            movescriptct=nWanderdelay;
            return face_nothing;
        }
    }
    else
        movescriptct--;

    return bMoving?direction:face_nothing;
}

Direction CEntity::Chase()
{
    if (!pChasetarget)
    {
        Log::Write( "Chasing entity with no chasing target!!");
        return face_nothing;
    }

    int nDeltax=pChasetarget->x-x;
    int nDeltay=pChasetarget->y-y;

    // close enough?
    if (sqrt(nDeltax*nDeltax + nDeltay*nDeltay)<nMinchasedist)
        return face_nothing;

    if (nDeltay<0)
    {
        if (nDeltax<0)  return face_upleft;
        if (nDeltax>0)  return face_upright;
        return face_up;
    }
    if (nDeltay>0)
    {
        if (nDeltax<0)  return face_downleft;
        if (nDeltax>0)  return face_downright;
        return face_down;
    }
    if (nDeltax<0)
        return face_left;
    if (nDeltax>0)
        return face_right;

    // if we get this far, then the entity is standing directly on top of its target
    return face_nothing;
}

Direction CEntity::GetMoveScriptCommand()
{
    // TODO: check for diagonals here.
    char c;
    
    if (movescriptct<1)
    {
        do
        {
            c=curmovescript[movescriptofs++];
            
            if (movescriptofs>curmovescript.length())			// if we've reached the end of the move script,
            {
                Stop();
                return face_nothing;
            }
            
        } while (c==' ');
        
        if (c>='a' && c<='z') c^=32;
        
        Direction newdir;

        switch(c)
        {
        case 'U': bMoving=true;  newdir=face_up;                 movescriptct=get_int(curmovescript,movescriptofs);  break;
        case 'D': bMoving=true;  newdir=face_down;               movescriptct=get_int(curmovescript,movescriptofs);  break;
        case 'L': bMoving=true;  newdir=face_left;               movescriptct=get_int(curmovescript,movescriptofs);  break;
        case 'R': bMoving=true;  newdir=face_right;              movescriptct=get_int(curmovescript,movescriptofs);  break;
        case 'F': bMoving=false; newdir=(Direction)get_int(curmovescript,movescriptofs);                             break;
        case 'Z': bMoving=false; nSpecframe=get_int(curmovescript,movescriptofs);                                       break;
        case 'W': bMoving=false; movescriptct=get_int(curmovescript,movescriptofs);                                     break;
    
        case 'X':
            {
                int destx=get_int(curmovescript,movescriptofs);
                if      (x>destx)  {    bMoving=true;               newdir=face_left;                        movescriptct=x-destx;   }
                else if (x<destx)  {    bMoving=true;               newdir=face_right;                       movescriptct=destx-x;   }
                else               {    bMoving=false;                                                          movescriptct=1;         }
                break;
            }
        case 'Y':
            {
                int desty=get_int(curmovescript,movescriptofs);
                if      (y>desty)  {    bMoving=true;               newdir=face_down;                        movescriptct=y-desty;   }
                else if (y<desty)  {    bMoving=true;               newdir=face_up;                          movescriptct=desty-y;   }
                else               {    bMoving=false;                                                          movescriptct=1;         } 
                break;
            }
            
        case 'B':
            movescriptofs=0;										// start over
            movescriptct=0;
            bMoving=false;					// the entity will get a new command next tick, so make it stand still for this one
            break;
        }

        return bMoving?newdir:face_nothing;
    }
    else
        movescriptct--;

    return bMoving?direction:face_nothing;
}

Direction CEntity::HandlePlayer()
{
    engine.TestActivate(*this);
    
    Input& input=engine.input;
    //input.Update();

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

void CEntity::Update()
{
    Direction newdir;

    UpdateAnimation();

    if (this==engine.pPlayer)
        newdir=HandlePlayer();
    else
        switch (movecode)
        {
        case mc_nothing:    newdir=face_nothing;            break;
        case mc_wander:
        case mc_wanderzone:
        case mc_wanderrect: newdir=Wander();                break;
        case mc_chase:      newdir=Chase();                 break;
        case mc_script:     newdir=GetMoveScriptCommand();  break;
        default:
            Log::Write( "CEntity::Update: Internal error -- bogus movecode");
            return;     // O_O;
        }

    if (newdir==face_nothing)
    {
        Stop();
        return;
    }

    Move(newdir);
}