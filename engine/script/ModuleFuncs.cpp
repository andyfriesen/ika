
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

    PyMethodDef standard_methods[] =
    {
        //  name  | function

        // Misc
        { "Log",            (PyCFunction)std_log,               METH_VARARGS,
            "Log(message)\n\n"
            "Writes a string to ika.log, if logging is enabled."
        },

        { "Exit",           (PyCFunction)std_exit,              METH_VARARGS,
            "Exit([message])\n\n"
            "Exits ika immediately, displaying the message onscreen, if specified."
        },

        { "GetCaption",     (PyCFunction)std_getcaption,        METH_NOARGS,
            "GetCaption() -> string\n\n"
            "Returns the caption on the ika window title bar."
        },

        { "SetCaption",     (PyCFunction)std_setcaption,        METH_VARARGS,
            "SetCaption(newcaption)\n\n"
            "Sets the caption on the ika window title bar."
        },

        { "GetFrameRate",   (PyCFunction)std_getframerate,      METH_NOARGS,
            "GetFrameRate() -> int\n\n"
            "Returns the current engine framerate, in frames per second."
        },

        { "Delay",          (PyCFunction)std_delay,             METH_VARARGS,
            "Delay(time)\n\n"
            "Freezes the engine for a number of 'ticks'. (one tick is 1/100th of a second)"
        },

        { "Wait",           (PyCFunction)std_wait,              METH_VARARGS,
            "Wait(time)\n\n"
            "Runs the engine for a number of ticks, disallowing player input.\n"
            "Unlike Delay, Wait causes entities to be processed, the tileset to be animated, and the map drawn."
        },

        { "GetTime",        (PyCFunction)std_gettime,           METH_NOARGS,
            "GetTime() -> int\n\n"
            "Returns the number of ticks since the engine was started."
        },

        { "Random",         (PyCFunction)std_random,            METH_VARARGS,
            "Random(min, max) -> int\n\n"
            "Returns a random integer less than or equal to min, and less than max.\n"
            "ie.  min <= value < max"
        },

        // Video

        { "RGB",            (PyCFunction)std_rgb,               METH_VARARGS,
            "RGB(r, g, b[, a]) -> int\n\n"
            "Creates a 32bpp colour value from the four colour levels passed.  If alpha is\n"
            "omitted, it is assumed to be 255. (opaque)"
        },

        { "GetRGB",         (PyCFunction)std_getrgb,            METH_VARARGS,
            "GetRGB(colour) -> tuple(int, int, int, int)\n\n"
            "Returns a 4-tuple containing the red, blue, green, and alpha values of the colour\n"
            "passed, respectively."
        },

        //{ "PaletteMorph", (PyCFunction)std_palettemorph,      METH_VARARGS },

        // Entity
        { "ProcessEntities", (PyCFunction)std_processentities,  METH_VARARGS,
            "ProcessEntities()\n\n"
            "Performs 1/100th of a second of entity AI.  Calling this 100 times a second\n"
            "will cause entities to move around as if the engine was in control."
        },

        { "SetCameraTarget", (PyCFunction)std_setcameratarget,  METH_VARARGS,
            "SetCameraTarget(entity)\n\n"
            "Sets the camera target to the entity specified.  If None is passed instead, \n"
            "the camera remains stationary, and can be altered with the Map.xwin and Map.ywin\n"
            "properties."
        },

        { "GetCameraTarget", (PyCFunction)std_getcameratarget,  METH_NOARGS,
            "GetCameraTarget() -> Entity\n\n"
            "Returns the entity that the camera is following, or None if it is free."
        },

        { "SetPlayer",      (PyCFunction)std_setplayer,         METH_VARARGS,
            "SetPlayer(entity)\n\n"
            "Sets the player entity to the entity passed.  The player entity is the entity\n"
            "that moves according to user input.  Passing None instead unsets any player entity\n"
            "that may have been previously set."
        },

        { "GetPlayer",      (PyCFunction)std_getplayer,         METH_NOARGS,
            "GetPlayer(entity) -> Entity\n\n"
            "Returns the current player entity, or None if there isn't one."
        },
        { "EntityAt",       (PyCFunction)std_entityat,          METH_VARARGS,
            "EntityAt(x, y, width, height) -> Entity\n\n"
            "If there is an entity within the rectangle passed, it is returned.\n"
            "If not, then None is returned."
        },

        { "HookRetrace",    (PyCFunction)std_hookretrace,       METH_VARARGS,
            "HookRetrace(function)\n\n"
            "Adds the function to the retrace queue. (it will be called whenever the map is drawn, \n"
            "whether by Map.Render or by other means)"
        },

        { "UnhookRetrace",  (PyCFunction)std_unhookretrace,     METH_VARARGS,
            "UnhookRetrace([function])\n\n"
            "Removes the function from the retrace queue if it is present.  If not, the call does\n"
            "nothing.  If the argument is omitted, then the list is cleared in its entirety."
        },

        { "HookTimer",      (PyCFunction)std_hooktimer,         METH_VARARGS,
            "HookTimer(function)\n\n"
            "Adds the function to the timer queue. (the function will be called 100 times per second.\n"
            "This feature should be used sparingly, as it will cause serious problems if the queue\n"
            "cannot be executed in less than 1/100th of a second."
        },

        { "UnhookTimer",    (PyCFunction)std_unhooktimer,       METH_VARARGS,
            "UnhookTimer([function])\n\n"
            "Removes the function from the timer queue if it is present.  If not, the call does\n"
            "nothing.  If the argument is omitted, then the list is cleared in its entirety."
        },

        {    0    }
    };

#undef METHOD
#undef METHOD1
}
