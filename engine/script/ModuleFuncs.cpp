
#include "ObjectDefs.h"
#include "main.h"
#include "timer.h"
#include "SDL/SDL.h"

#define METHOD(x)  PyObject* x(PyObject* self, PyObject* args)
#define METHOD1(x) PyObject* x(PyObject*)

namespace Script
{
    CEngine*    engine;

    PyObject*   entitydict;
    PyObject*   playerent;
    PyObject*   cameratarget;
    PyObject*   errorhandler;

    PyObject*   sysmodule;                         // the system scripts (system.py)
    PyObject*   mapmodule;                         // scripts for the currently loaded map

    METHOD(std_log)
    {
        char* message;

        if (!PyArg_ParseTuple(args, "s:log", &message))
            return NULL;

        Log::Write(message);

        Py_INCREF(Py_None);
        return Py_None;                                    // returning void :)
    }

    METHOD(std_exit)
    {
        char* message="";

        if (!PyArg_ParseTuple(args, "|s:exit", &message))
            return NULL;

        engine->Sys_Error(message);

        // never gets called... I hope.
        Py_INCREF(Py_None);
        return Py_None;
    }

    METHOD1(std_getcaption)
    {
        char* s;
        SDL_WM_GetCaption(&s, 0);

        return PyString_FromString(s);
    }

    METHOD(std_setcaption)
    {
        char* s = "";

        if (!PyArg_ParseTuple(args, "|s:SetCaption", &s))
            return 0;

        SDL_WM_SetCaption(s, 0);

        Py_INCREF(Py_None);
        return Py_None;
    }

    METHOD1(std_getframerate)
    {
        return PyInt_FromLong(engine->video->GetFrameRate());
    }

    METHOD(std_delay)
    {
        int ticks;

        if (!PyArg_ParseTuple(args, "i:delay", &ticks))
            return NULL;

        int endtime = ticks + GetTime();

        // Always check messages at least once.
        do
        {
            engine->CheckMessages();
        }
        while (endtime > GetTime());

        Py_INCREF(Py_None);
        return Py_None;
    }

    METHOD(std_wait)
    {
        int ticks;    
        if (!PyArg_ParseTuple(args, "i:wait", &ticks))
            return NULL;

        CEntity* pSaveplayer=engine->pPlayer;
        engine->pPlayer=0;                             // stop the player entity

        int t = GetTime();
        int endtime = ticks + t;

        while (endtime > GetTime())
        {
            engine->CheckMessages();

            while (t < GetTime())
            {
                t++;
                engine->GameTick();
            }

            engine->Render();
            engine->video->ShowPage();
        }

        engine->pPlayer = pSaveplayer;                   // restore the player
        Py_INCREF(Py_None);
        return Py_None;
    }

    METHOD1(std_gettime)
    {
        return PyInt_FromLong((long)GetTime());
    }

    METHOD(std_random)
    {
        int min, max;

        if (!PyArg_ParseTuple(args, "ii:Random", &min, &max))
            return NULL;

        return PyInt_FromLong(Random(min, max));
    }

    // video

    METHOD(std_showpage)
    {
        if (!PyArg_ParseTuple(args, ""))
            return NULL;

        engine->video->ShowPage();

        Py_INCREF(Py_None);
        return Py_None;
    }

    METHOD(std_palettemorph)
    {
        int r=255, g=255, b=255;

        if (!PyArg_ParseTuple(args, "|iii:PaletteMorph", &r, &g, &b))
            return NULL;

        //    gfxPaletteMorph(r, g, b);

        Py_INCREF(Py_None);
        return Py_None;
    }

    METHOD(std_getrgb)
    {
        int colour;

        if (!PyArg_ParseTuple(args, "i:GetRGB", &colour))
            return NULL;

        int b=colour&255;
        int g=(colour>>8)&255;
        int r=(colour>>16)&255;
        int a=(colour>>24)&255;

        return Py_BuildValue("iiii", r, g, b, a);
    }

    METHOD(std_rgb)
    {
        int r, g, b, a=255;

        if (!PyArg_ParseTuple(args, "iii|i:RGB", &r, &g, &b, &a))
            return NULL;

        return PyInt_FromLong(RGBA(r, g, b, a).i);
    }

    METHOD(std_processentities)
    {
        if (!PyArg_ParseTuple(args, ""))
            return NULL;

        engine->ProcessEntities();

        Py_INCREF(Py_None);
        return Py_None;
    }

    METHOD(std_setcameratarget)
    {
        Script::Entity::EntityObject* ent;

        if (!PyArg_ParseTuple(args, "O:SetCameraTarget", &ent))
            return NULL;

        if ((PyObject*)ent==Py_None)
        {
            engine->pCameratarget=0;
            Py_XDECREF(cameratarget);
            cameratarget=0;
        }
        else
        {
            if (ent->ob_type!=&Script::Entity::type)
            {
                PyErr_SetString(PyExc_TypeError, "SetCameraTarget not called with entity/None object");
                return NULL;
            }

            engine->pCameratarget = ent->ent;  // oops

            Py_INCREF(ent);
            Py_XDECREF(cameratarget);
            cameratarget=(PyObject*)ent;
        }

        Py_INCREF(Py_None);
        return Py_None;
    }

    METHOD1(std_getcameratarget)
    {
        PyObject* result = cameratarget ? cameratarget : Py_None;

        Py_INCREF(result);
        return result;
    }

    METHOD(std_setplayer)                                            // FIXME?  Is there a more intuitive way to do this?
    {
        Script::Entity::EntityObject* ent;

        if (!PyArg_ParseTuple(args, "O:SetPlayerEntity", &ent))
            return NULL;

        if ((PyObject*)ent==Py_None)
        {
            Py_XDECREF(playerent);
            playerent=0;
            engine->pPlayer=0;
        }
        else
        {
            if (ent->ob_type != &Script::Entity::type)
            {
                PyErr_SetString(PyExc_TypeError, "SetPlayerEntity not called with entity object or None.");
                return NULL;
            }

            Py_INCREF(ent);
            Py_XDECREF(playerent);
            playerent=(PyObject*)ent;

            engine->pPlayer=ent->ent;
            ent->ent->movecode=mc_nothing;
        }

        PyObject* result = std_setcameratarget(self, args);
        Py_XDECREF(result);

        Py_INCREF(Py_None);
        return Py_None;
    }

    METHOD1(std_getplayer)
    {
        PyObject* result = playerent ? playerent : Py_None;
        Py_INCREF(result);
        return result;
    }

    METHOD(std_entityat)
    {
        int x, y, width, height;
        if (!PyArg_ParseTuple(args, "iiii|EntityAt", &x, &y, &width, &height))
            return 0;

        int x2=x+width;
        int y2=y+height;

        int count=0;
        PyObject* pKey=0;
        PyObject* pValue=0;
        while (PyDict_Next(entitydict, &count, &pKey, &pValue))
        {
            if (pValue->ob_type != &Script::Entity::type)
            {
                Log::Write("Someone's being a goober and putting non-entities in ika.Map.entities! >:(");
                continue;
            }

            CEntity& ent=*((Script::Entity::EntityObject*)pValue)->ent;
            if (x>ent.x+ent.pSprite->nHotw)    continue;
            if (y>ent.y+ent.pSprite->nHoth)    continue;
            if (x2<ent.x)    continue;
            if (y2<ent.y)    continue;

            Py_INCREF(pValue);
            return pValue;
        }

        Py_INCREF(Py_None);
        return Py_None;
    }

    METHOD(std_hookretrace)
    {
        PyObject*    pFunc;

        if (!PyArg_ParseTuple(args, "O:HookRetrace", &pFunc))
            return NULL;

        if (!PyCallable_Check(pFunc))
        {
            PyErr_SetString(PyExc_TypeError, "HookRetrace requires a function as a parameter");
            return NULL;
        }

        Py_INCREF(pFunc);
        engine->pHookretrace.Add(pFunc);

        Py_INCREF(Py_None);
        return Py_None;
    }

    METHOD(std_unhookretrace)
    {
        PyObject* pFunc=NULL;

        if (!PyArg_ParseTuple(args, "|O:UnhookRetrace", &pFunc))
            return NULL;

        if (!pFunc)
        {
            std::list<void*>::iterator i;

            for (i=engine->pHookretrace.begin(); i!=engine->pHookretrace.end(); i++)
                Py_XDECREF((PyObject*)*i);                                        // dereference

            engine->pHookretrace.Clear();
        }
        else
        {
            std::list<void*>::iterator i;

            for (i=engine->pHookretrace.begin(); i!=engine->pHookretrace.end(); i++)
            {
                if (*i==pFunc)
                {
                    Py_DECREF(pFunc);
                    engine->pHookretrace.Remove(*i);
                    break;
                }
            }
        }        

        Py_INCREF(Py_None);
        return Py_None;
    }

    METHOD(std_hooktimer)
    {
        PyObject*    pFunc;

        if (!PyArg_ParseTuple(args, "O:HookTimer", &pFunc))
            return NULL;

        if (!PyCallable_Check(pFunc))
        {
            PyErr_SetString(PyExc_TypeError, "HookTimer requires a function as a parameter");
            return NULL;
        }

        Py_INCREF(pFunc);
        //    engine->pHooktimer.push_back(pFunc);
        engine->pHooktimer.Add(pFunc);

        Py_INCREF(Py_None);
        return Py_None;
    }


    // GAY GAY GAY GAY GAY GAY GAY GAY GAY GAY GAY GAY GAY GAY GAY GAY GAY
    METHOD(std_unhooktimer)
    {
        PyObject* pFunc=NULL;

        if (!PyArg_ParseTuple(args, "|O:UnhookTimer", &pFunc))
            return NULL;

        if (!pFunc)
        {
            std::list<void*>::iterator i;

            for (i=engine->pHooktimer.begin(); i!=engine->pHooktimer.end(); i++)
                Py_DECREF((PyObject*)*i);                                    // dereference

            engine->pHooktimer.Clear();
        }
        else
        {
            std::list<void*>::iterator i;

            for (i=engine->pHooktimer.begin(); i!=engine->pHooktimer.end(); i++)
            {
                if (*i==pFunc)
                {
                    Py_DECREF(pFunc);
                    //engine->pHooktimer.remove(*i);
                    engine->pHooktimer.Remove(*i);
                    break;
                }
            }
        }
        Py_INCREF(Py_None);
        return Py_None;
    }

#define FUNCTION(x, y)  { x, y, 1}
#define FUNCTION1(x, y) { x, y, METH_NOARGS }

    PyMethodDef standard_methods[] =
    {
        //  name  | function

        // Misc
        FUNCTION("Log",                 (PyCFunction)std_log),
        FUNCTION("Exit",                (PyCFunction)std_exit),
        FUNCTION1("GetCaption",         (PyCFunction)std_getcaption),
        FUNCTION("SetCaption",          (PyCFunction)std_setcaption),
        FUNCTION1("GetFrameRate",       (PyCFunction)std_getframerate),
        FUNCTION("Delay",               (PyCFunction)std_delay),
        FUNCTION("Wait",                (PyCFunction)std_wait),
        FUNCTION1("GetTime",            (PyCFunction)std_gettime),
        FUNCTION("Random",              (PyCFunction)std_random),

        // Video
        FUNCTION("ShowPage",            (PyCFunction)std_showpage),
        FUNCTION("RGB",                 (PyCFunction)std_rgb),
        FUNCTION("GetRGB",              (PyCFunction)std_getrgb),
        //FUNCTION("PaletteMorph",        (PyCFunction)std_palettemorph),

        // Entity
        FUNCTION("ProcessEntities",     (PyCFunction)std_processentities),
        FUNCTION("SetCameraTarget",     (PyCFunction)std_setcameratarget),
        FUNCTION1("GetCameraTarget",    (PyCFunction)std_getcameratarget),
        FUNCTION("SetPlayer",           (PyCFunction)std_setplayer),
        FUNCTION1("GetPlayer",          (PyCFunction)std_getplayer),
        FUNCTION("EntityAt",            (PyCFunction)std_entityat),

        FUNCTION("HookRetrace",         (PyCFunction)std_hookretrace),
        FUNCTION("UnhookRetrace",       (PyCFunction)std_unhookretrace),
        FUNCTION("HookTimer",           (PyCFunction)std_hooktimer),
        FUNCTION("UnhookTimer",         (PyCFunction)std_unhooktimer),

        // Object constructors
        FUNCTION("Image",               Script::Image::New),
        FUNCTION("Sound",               Script::Sound::New),
        FUNCTION("Font",                Script::Font::New),
        FUNCTION("Entity",              Script::Entity::New),
        FUNCTION("Canvas",              Script::Canvas::New),
        {    0    }
    };

#undef FUNCTION
#undef FUNCTION1
#undef METHOD
#undef METHOD1
}
