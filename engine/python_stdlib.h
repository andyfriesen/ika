
/*
Python/C++ interface
*/

#ifndef STDLIB_H
#define STDLIB_H

#define METHOD(x) PyObject* CScriptEngine::x (PyObject* self,PyObject* args)

#include "main.h"

static CEngine* pEngine;

// Prototyped structs and such:
struct v_ImageObject;
struct v_EntityObject;


METHOD(std_log)
{
    char* message;
    
    if (!PyArg_ParseTuple(args,"s:log",&message))
        return NULL;
    
    log(message);
    
    Py_INCREF(Py_None);
    return Py_None;									// returning void :)
}

METHOD(std_exit)
{
    char* message="";
    
    if (!PyArg_ParseTuple(args,"|s:exit",&message))
        return NULL;
    
    pEngine->Sys_Error(message);
    
    // never gets called... I hope.
    Py_INCREF(Py_None);
    return Py_None;
}

METHOD(std_delay)
{
    int ticks;
    
    if (!PyArg_ParseTuple(args,"i:delay",&ticks))
        return NULL;
    
    ticks+=pEngine->timer.systime;
    
    // Always check messages at least once.
    do
    if (pEngine->CheckMessages())
        return NULL;
    while (ticks>pEngine->timer.systime);
    
    Py_INCREF(Py_None);
    return Py_None;
}

METHOD(std_wait)
{
    int ticks;
    
    if (!PyArg_ParseTuple(args,"i:wait",&ticks))
        return NULL;
    
    CEntity* pSaveplayer=pEngine->pPlayer;
    pEngine->pPlayer=0;								// stop the player entity
    
    pEngine->timer.t=0;
    
    ticks+=pEngine->timer.systime;
    while (ticks>pEngine->timer.systime)
    {
        if (pEngine->CheckMessages())
            return NULL;
        while (pEngine->timer.t>0)
        {
            pEngine->timer.t--;
            pEngine->GameTick();
        }
        pEngine->Render();
        gfxShowPage();
    }
    
    pEngine->pPlayer=pSaveplayer;								// restore the player
    Py_INCREF(Py_None);
    return Py_None;
}

METHOD(std_gettime)
{
    if (!PyArg_ParseTuple(args,""))
        return NULL;
    
    return PyInt_FromLong((long)pEngine->timer.systime);
}

METHOD(std_random)
{
    int min,max;
    
    if (!PyArg_ParseTuple(args,"ii:Random",&min,&max))
        return NULL;
    
    return PyInt_FromLong(Random(min,max));
}

// video

METHOD(std_showpage)
{
    if (!PyArg_ParseTuple(args,""))
        return NULL;
    
    gfxShowPage();
    
    Py_INCREF(Py_None);
    return Py_None;
}

METHOD(std_getscreenimage)
{
    if (!PyArg_ParseTuple(args,""))
        return NULL;
    
    Py_INCREF(pScreenobject);
    return pScreenobject;
}

METHOD(std_palettemorph)
{
    int r=255,g=255,b=255;
    
    if (!PyArg_ParseTuple(args,"|iii:PaletteMorph",&r,&g,&b))
        return NULL;
    
    gfxPaletteMorph(r,g,b);
    
    Py_INCREF(Py_None);
    return Py_None;
}

METHOD(std_getrgb)
{
    int colour;
    
    if (!PyArg_ParseTuple(args,"i:GetRGB",&colour))
        return NULL;
    
    int b=colour&255;
    int g=(colour>>8)&255;
    int r=(colour>>16)&255;
    int a=(colour>>24)&255;
    
    return Py_BuildValue("iiii",r,g,b,a);
}

METHOD(std_rgb)
{
    int r,g,b,a=255;
    
    if (!PyArg_ParseTuple(args,"iii|i:RGB",&r,&g,&b,&a))
        return false;
    
    return PyInt_FromLong((a<<24)|(r<<16)|(g<<8)|b);
}

METHOD(std_hookbutton)
{
    int nButton;
    PyObject* pFunc;
    
    if (!PyArg_ParseTuple(args,"iO:HookButton",&nButton,&pFunc))
        return NULL;
    
    if (pFunc==Py_None)
        pEngine->pBindings[nButton]=NULL;
    else
    {
        if (!PyCallable_Check(pFunc))
        {
            PyErr_SetString(PyExc_TypeError,"Hookbutton requires None or a function to be hooked");
            return NULL;
        }
        
        Py_INCREF(pFunc);
        Py_XDECREF((PyObject*)pEngine->pBindings[nButton]);
        
        pEngine->pBindings[nButton]=pFunc;
    }
    
    Py_INCREF(Py_None);
    return Py_None;
}

METHOD(std_hookretrace)
{
    PyObject*	pFunc;
    
    if (!PyArg_ParseTuple(args,"O:HookRetrace",&pFunc))
        return NULL;
    
    if (!PyCallable_Check(pFunc))
    {
        PyErr_SetString(PyExc_TypeError,"HookRetrace requires a function as a parameter");
        return NULL;
    }
    
    Py_INCREF(pFunc);
    pEngine->pHookretrace.push_back(pFunc);
    
    Py_INCREF(Py_None);
    return Py_None;
}

METHOD(std_unhookretrace)
{
    PyObject* pFunc=NULL;
    
    if (!PyArg_ParseTuple(args,"|O:UnhookRetrace",&pFunc))
        return NULL;
    
    if (!pFunc)
    {
        std::list<void*>::iterator i;
        
        for (i=pEngine->pHookretrace.begin(); i!=pEngine->pHookretrace.end(); i++)
        {
            Py_DECREF((PyObject*)*i);										// dereference
            pEngine->pHookretrace.remove(*i);								// destroy
        }
    }
    else
    {	
        std::list<void*>::iterator i;
        
        for (i=pEngine->pHookretrace.begin(); i!=pEngine->pHookretrace.end(); i++)
        {
            if (*i==pFunc)
            {
                Py_DECREF(pFunc);
                pEngine->pHookretrace.remove(*i);
                break;
            }
        }
    }		
    Py_INCREF(Py_None);
    return Py_None;
}

METHOD(std_hooktimer)
{
    PyObject*	pFunc;
    
    if (!PyArg_ParseTuple(args,"O:HookTimer",&pFunc))
        return NULL;
    
    if (!PyCallable_Check(pFunc))
    {
        PyErr_SetString(PyExc_TypeError,"HookTimer requires a function as a parameter");
        return NULL;
    }
    
    Py_INCREF(pFunc);
    pEngine->pHooktimer.push_back(pFunc);
    
    Py_INCREF(Py_None);
    return Py_None;
}


// GAY GAY GAY GAY GAY GAY GAY GAY GAY GAY GAY GAY GAY GAY GAY GAY GAY
METHOD(std_unhooktimer)
{
    PyObject* pFunc=NULL;
    
    if (!PyArg_ParseTuple(args,"|O:UnhookTimer",&pFunc))
        return NULL;
    
    if (!pFunc)
    {
        std::list<void*>::iterator i;
        
        for (i=pEngine->pHooktimer.begin(); i!=pEngine->pHooktimer.end(); i++)
        {
            Py_DECREF((PyObject*)*i);									// dereference
            pEngine->pHooktimer.remove(*i);								// destroy
        }
    }
    else
    {
        std::list<void*>::iterator i;
        
        for (i=pEngine->pHooktimer.begin(); i!=pEngine->pHooktimer.end(); i++)
        {
            if (*i==pFunc)
            {
                Py_DECREF(pFunc);
                pEngine->pHooktimer.remove(*i);
                break;
            }
        }
    }
    Py_INCREF(Py_None);
    return Py_None;
}

// MASTA TABLE YO

PyMethodDef standard_methods[] =
{
    //  name  | function | calling convention
    
    // Misc
    {	"Log",				CScriptEngine::std_log	,			1	},
    {	"Exit",				CScriptEngine::std_exit	,			1	},
    {	"Delay",			CScriptEngine::std_delay,			1	},
    {	"Wait",				CScriptEngine::std_wait	,			1	},
    {	"GetTime",			CScriptEngine::std_gettime,			1	},
    {	"Random",			CScriptEngine::std_random,			1	},
    
    // Video
    {	"LoadImage",		CScriptEngine::std_loadimage,		1	},
    {	"ShowPage",			CScriptEngine::std_showpage,		1	},
    {	"SetRenderDest",	CScriptEngine::std_setrenderdest,	1	},
    {	"GetRenderDest",	CScriptEngine::std_getrenderdest,	1	},
    {	"GetScreenImage",	CScriptEngine::std_getscreenimage,	1	},
    {	"RGB",				CScriptEngine::std_rgb,				1	},
    {	"GetRGB",			CScriptEngine::std_getrgb,			1	},
    {	"PaletteMorph",		CScriptEngine::std_palettemorph,	1	},
    
    // Entity
    {	"ProcessEntities",	CScriptEngine::std_processentities,	1	},
    {	"SetPlayer",		CScriptEngine::std_setplayer,		1	},
    {	"SetCameraTarget",	CScriptEngine::std_setcameratarget,	1	},
    {	"GetCameraTarget",	CScriptEngine::std_getcameratarget,	1	},
    
    {	"HookButton",		CScriptEngine::std_hookbutton,		1	},
    {	"HookRetrace",		CScriptEngine::std_hookretrace,		1	},
    {	"UnhookRetrace",	CScriptEngine::std_unhookretrace,	1	},
    {	"HookTimer",		CScriptEngine::std_hooktimer,		1	},
    {	"UnhookTimer",		CScriptEngine::std_unhooktimer,		1	},
    
    // Object constructors
    {	"Image",			CScriptEngine::Image_New,			1	},
    {	"Music",			CScriptEngine::Music_New,			1	},
    {	"Sound",			CScriptEngine::Sound_New,			1	},
    {	"Font",				CScriptEngine::Font_New,			1	},
    {	"Entity",			CScriptEngine::std_spawnentity,		1	},
    {	NULL,	NULL	}						// sentinel?  (end of list marker, I think)
};

#undef METHOD

#endif