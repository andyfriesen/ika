#include "entity.h"
#include "misc.h"
#include "sprite.h"
#include "log.h"
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

static int get_int(string_k s,int& offset)
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
            return atoi(s.mid(start,offset-start).c_str());
    }
}

//#include "log.h"	// temp debugging code

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

void CEntity::SetAnimScript(const string_k& newscript)
{
    curanimscript=newscript;
    animscriptofs=0;
    animscriptct=0;
    UpdateAnimation();									// and immediately update the frame
}

void CEntity::SetMoveScript(const string_k& newscript)
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

void CEntity::Move(Direction d)
{
    int newx=x,newy=y;

    switch (d)
    {
    case face_up:           newy--; break;
    case face_down:         newy++; break;
    case face_left:         newx--; break;
    case face_right:        newx++; break;
       
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

    if (direction!=d || !bMoving)
    {
        bMoving=true;
        direction=d;
        SetAnimScript(pSprite->Script((int)direction));
    }
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
    input.Update();

    if (input.up)
    {
	if (input.left)		return face_upleft;
	if (input.right)	return face_upright;
	return face_up;
    }
    if (input.down)
    {
	if (input.left)		return face_downleft;
	if (input.right)	return face_downright;
	return face_down;
    }
    if (input.left) return face_left;					// by this point, the diagonal possibilities are already taken care of
    if (input.right)return face_right;
    
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
        case mc_script:     newdir=GetMoveScriptCommand();  break;
        default:
            log("CEntity::Update: Internal error -- bogus movecode");
            return;     // O_O;
        }

    if (newdir==face_nothing)
    {
        Stop();
        return;
    }

    Move(newdir);
}