#include "entity.h"

// TODO: make sure entities never have a negative speed

CEntity::CEntity()
{
    animscriptidx=0;
    animscriptofs=0;
    animscriptct=0;
    movescriptofs=0;
    movescriptct=0;
    thedirectionImgoinginnow=face_nothing;
    x=0;
    y=0;
    nSpeed=entspeed_normal;
    nSpeedcount=0;
    pSprite=0;
    facing=face_down;
    bMoving=false;
    movecode=mc_nothing;
    bVisible=true;
    nFramect=0;
    nCurframe=0;
    nSpecframe=0;
    nWandersteps=0;
    bAdjacentactivate=false;
    bIsobs=true;
    bMapobs=true;
    bEntobs=true;
}

CEntity::CEntity(const Sv2entity& e)
{
    x=e.x;
    y=e.y;
    nCurframe=0;
    //pActscript=e.actscript;
    bAdjacentactivate=e.actm?true:false;
    
    nWandersteps=e.step;
    nWanderdelay=e.delay;
    nSpeed=100;
    nSpeedcount=0;
    facing=face_down;
    bVisible=true;
    
    movecode=(MoveCode)e.movecode;	
    
    sName=string_k(e.desc);
}

CEntity::CEntity(const SMapEntity& e)
{
    animscriptidx=0;
    animscriptofs=0;
    animscriptct=0;
    movescriptofs=0;
    movescriptct=0;
    thedirectionImgoinginnow=face_nothing;
    nSpeedcount=0;
    pSprite=0;
    facing=face_down;
    bMoving=false;
    bVisible=true;
    nFramect=0;
    nCurframe=0;
    nSpecframe=0;
    nWandersteps=0;
    bAdjacentactivate=false;
    bIsobs=e.bIsobs;
    bMapobs=e.bMapobs;
    bEntobs=e.bEntobs;
    
    x=e.x;
    y=e.y;
    sName=e.sName;
    facing=(Direction)e.direction;
    nSpeed=e.nSpeed;
    movecode=mc_nothing;//e.state;
    sActscript=e.sActscript;
    movecode=e.state;
    
    nWandersteps=e.nWandersteps;
    nWanderdelay=e.nWanderdelay;
    
    wanderrect.left=e.nWanderrect[0];
    wanderrect.top=e.nWanderrect[1];
    wanderrect.right=e.nWanderrect[2];
    wanderrect.bottom=e.nWanderrect[3];
}

void CEntity::Init()
{
    x=y=0;
    movecode=mc_nothing;
    nSpeed=entspeed_normal;
    nCurframe=0;
    bVisible=true;
    animscriptct=1;
    movescriptct=1;
    nSpeed=100;
    nSpeedcount=0;
    facing=face_down;
}

void CEntity::Free()
{
    bVisible=false;
}

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
                movescriptofs=0;
                curmovescript="";								// nuke it, the entity's task has been finished
                thedirectionImgoinginnow=face_nothing;
                return thedirectionImgoinginnow;
            }
            
        } while (c==' ');
        
        if (c>='a' && c<='z') c^=32;
        
        switch(c)
        {
        case 'U':
            thedirectionImgoinginnow=face_up;
            movescriptct=get_int(curmovescript,movescriptofs);
            break;
        case 'D':
            thedirectionImgoinginnow=face_down;
            movescriptct=get_int(curmovescript,movescriptofs);
            break;
        case 'L':
            thedirectionImgoinginnow=face_left;
            movescriptct=get_int(curmovescript,movescriptofs);
            break;
        case 'R':
            thedirectionImgoinginnow=face_right;
            movescriptct=get_int(curmovescript,movescriptofs);
            break;
        case 'F':
            thedirectionImgoinginnow=(Direction)get_int(curmovescript,movescriptofs);
            break;
        case 'X':
            {
                int destx=get_int(curmovescript,movescriptofs);
                if (x>destx)
                {
                    thedirectionImgoinginnow=face_left;
                    movescriptct=x-destx;
                }
                else if (x<destx)
                {
                    thedirectionImgoinginnow=face_right;
                    movescriptct=destx-x;
                }
                else
                {
                    thedirectionImgoinginnow=face_nothing;
                    movescriptct=1;
                }
                break;
            }
        case 'Y':
            {
                int desty=get_int(curmovescript,movescriptofs);
                if (y>desty)
                {
                    thedirectionImgoinginnow=face_down;
                    movescriptct=y-desty;
                }
                else if (y<desty)
                {
                    thedirectionImgoinginnow=face_up;
                    movescriptct=desty-y;
                }
                else
                {
                    thedirectionImgoinginnow=face_nothing;
                    movescriptct=1;
                }
                break;
            }
        case 'Z':
            nSpecframe=get_int(curmovescript,movescriptofs);
            break;
            
        case 'W':
            movescriptct=get_int(curmovescript,movescriptofs);
            thedirectionImgoinginnow=face_nothing;
            break;
            
        case 'B':
            movescriptofs=0;										// start over
            movescriptct=0;
            thedirectionImgoinginnow=face_nothing;					// the entity will get a new command next tick, so make it stand still for this one
            break;
        }
    }
    else
        movescriptct--;
    
    return thedirectionImgoinginnow;
}

void CEntity::SetAnimScript(const string_k& newscript,int idx)
{
    //	if (idx==animscriptidx && animscriptidx!=-1)		// by remembering the index, we avoid redundantly setting the strand, without having to do string comparisons. @_@ --tSB
    //		return;
    curanimscript=newscript;
    animscriptidx=idx;
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
