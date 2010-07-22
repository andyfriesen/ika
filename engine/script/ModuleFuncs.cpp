
#include "ObjectDefs.h"
#include "main.h"
#include "timer.h"
#include "SDL/SDL.h"

#define METHOD(x)  PyObject* x(PyObject* /*self*/, PyObject* args)
#define METHOD1(x) PyObject* x(PyObject* /*self*/)
#define METHOD2(x) PyObject* x(PyObject* self, PyObject* args)

namespace Script {
    Engine*    engine;

    PyObject*   entityDict;
    PyObject*   playerent;
    PyObject*   cameraTarget;

    PyObject*   sysModule;                         // the system scripts (system.py)
    PyObject*   mapModule;                         // scripts for the currently loaded map

    std::stringstream pyOutput;

    METHOD(ika_log) {
        static bool warnFlag = false;
        char* message;

        if (!PyArg_ParseTuple(args, "s:log", &message))
            return 0;

        if (!warnFlag) {
            warnFlag = true;
            Log::Write("* ika.Log will not be present in ika 0.61");
            Log::Write("* Use the print statement instead");
        }

        Log::Write("%s", message);

        Py_INCREF(Py_None);
        return Py_None;                                    // returning void :)
    }

    METHOD(ika_exit) {
        char* message = NULL;

        if (!PyArg_ParseTuple(args, "|s:exit", &message))
            return 0;

        engine->Sys_Error(message);

        // never gets called... I hope.
        Py_INCREF(Py_None);
        return Py_None;
    }

    METHOD1(ika_getcaption) {
        char* s;
        SDL_WM_GetCaption(&s, 0);

        return PyString_FromString(s);
    }

    METHOD(ika_setcaption) {
        char* s = NULL;

        if (!PyArg_ParseTuple(args, "|s:SetCaption", &s))
            return 0;

        SDL_WM_SetCaption(s, 0);

        Py_INCREF(Py_None);
        return Py_None;
    }

    METHOD1(ika_getframerate) {
        return PyInt_FromLong(engine->video->GetFrameRate());
    }

    METHOD(ika_delay) {
        int ticks;

        if (!PyArg_ParseTuple(args, "i:delay", &ticks))
            return 0;

        int endtime = ticks + GetTime();

        // Always check messages at least once.
        do {
            engine->CheckMessages();
        }
        while (endtime > GetTime());

        Py_INCREF(Py_None);
        return Py_None;
    }

    METHOD(ika_wait) {
        int ticks;    
        if (!PyArg_ParseTuple(args, "i:wait", &ticks))
            return 0;

        ::Entity* pSaveplayer = engine->player;
        engine->player = 0;                             // stop the player entity

        int t = GetTime();
        int endtime = ticks + t;

        while (endtime > GetTime()) {
            engine->CheckMessages();

            while (t < GetTime()) {
                t++;
                engine->GameTick();
            }

            engine->Render();
            engine->video->ShowPage();
        }

        engine->player = pSaveplayer;                   // restore the player
        Py_INCREF(Py_None);
        return Py_None;
    }

    METHOD1(ika_gettime) {
        return PyInt_FromLong((long)GetTime());
    }

    METHOD(ika_random) {
        int min, max;

        if (!PyArg_ParseTuple(args, "ii:Random", &min, &max))
            return 0;

        return PyInt_FromLong(Random(min, max));
    }

    // video

    METHOD(ika_getrgb) {
        RGBA colour;

        if (!PyArg_ParseTuple(args, "i:GetRGB", &colour.i))
            return 0;

        return Py_BuildValue("iiii", colour.r, colour.g, colour.b, colour.a);
    }

    METHOD(ika_rgb) {
        u8 r, g, b, a = 255;

        if (!PyArg_ParseTuple(args, "BBB|B:RGB", &r, &g, &b, &a))
            return 0;

        return PyInt_FromLong(RGBA(r, g, b, a).i);
    }

    METHOD(ika_processentities) {
        if (!PyArg_ParseTuple(args, ""))
            return 0;

        engine->ProcessEntities();

        Py_INCREF(Py_None);
        return Py_None;
    }

    METHOD(ika_setcameraTarget) {
        Script::Entity::EntityObject* ent;

        if (!PyArg_ParseTuple(args, "O:SetcameraTarget", &ent))
            return 0;

        if ((PyObject*)ent == Py_None) {
            engine->cameraTarget = 0;
            Py_XDECREF(cameraTarget);
            cameraTarget = 0;
        } else {
            if (ent->ob_type != &Script::Entity::type) {
                PyErr_SetString(PyExc_TypeError, "SetcameraTarget not called with entity/None object");
                return 0;
            }

            engine->cameraTarget = ent->ent;  // oops

            Py_INCREF(ent);
            Py_XDECREF(cameraTarget);
            cameraTarget=(PyObject*)ent;
        }

        Py_INCREF(Py_None);
        return Py_None;
    }

    METHOD1(ika_getcameraTarget) {
        PyObject* result = cameraTarget ? cameraTarget : Py_None;

        Py_INCREF(result);
        return result;
    }

    // FIXME?  Is there a more intuitive way to do this?
    METHOD2(ika_setplayer) {
        Script::Entity::EntityObject* ent;

        if (!PyArg_ParseTuple(args, "O:SetPlayerEntity", &ent))
            return 0;

        if ((PyObject*)ent == Py_None) {
            Py_XDECREF(playerent);
            playerent = 0;
            engine->player = 0;
        } else {
            if (ent->ob_type != &Script::Entity::type) {
                PyErr_SetString(PyExc_TypeError, "SetPlayerEntity not called with entity object or None.");
                return 0;
            }

            Py_INCREF(ent);
            Py_XDECREF(playerent);
            playerent = (PyObject*)ent;

            engine->player = ent->ent;
        }

        PyObject* result = ika_setcameraTarget(self, args);
        Py_XDECREF(result);

        Py_INCREF(Py_None);
        return Py_None;
    }

    METHOD1(ika_getplayer) {
        PyObject* result = playerent ? playerent : Py_None;
        Py_INCREF(result);
        return result;
    }

    METHOD(ika_entitiesat) {
        int x, y, width, height;
        uint layer;
        if (!PyArg_ParseTuple(args, "iiiiI|EntitiesAt", &x, &y, &width, &height, &layer))
            return 0;

        int x2 = x+width;
        int y2 = y+height;

        std::vector< ::Script::Entity::EntityObject*> ents;

        for (std::map< ::Entity*, ::Script::Entity::EntityObject*>::iterator 
            iter = ::Script::Entity::instances.begin(); 
            iter != ::Script::Entity::instances.end(); 
            iter++) {
            ::Entity* ent = iter->first;

            if (ent->layerIndex != layer)         continue;
            if (x > ent->x+ent->sprite->nHotw)    continue;
            if (y > ent->y+ent->sprite->nHoth)    continue;
            if (x2 < ent->x)    continue;
            if (y2 < ent->y)    continue;

            ents.push_back(iter->second);
        }

        PyObject* list = PyList_New(ents.size());

        for (uint i = 0; i < ents.size(); i++) {
            Py_INCREF(ents[i]);
            PyList_SET_ITEM(list, i, reinterpret_cast<PyObject*>(ents[i]));
        }

        return list;
    }

    METHOD(ika_hookretrace) {
        PyObject*    pFunc;

        if (!PyArg_ParseTuple(args, "O:HookRetrace", &pFunc))
            return 0;

        if (!PyCallable_Check(pFunc)) {
            PyErr_SetString(PyExc_TypeError, "HookRetrace requires a function as a parameter");
            return 0;
        }

        Py_INCREF(pFunc);
        engine->_hookRetrace.add(pFunc);

        Py_INCREF(Py_None);
        return Py_None;
    }

    METHOD(ika_unhookretrace) {
        PyObject* func = 0;

        if (!PyArg_ParseTuple(args, "|O:UnhookRetrace", &func))
            return 0;

        if (!func) {
            engine->_hookRetrace.clear();
        } else {
            foreach (ScriptObject iter, engine->_hookRetrace) {
                if (iter.get() == func) {
                    engine->_hookRetrace.remove(iter.get());
                    break;
                }
            }
        }        

        Py_INCREF(Py_None);
        return Py_None;
    }

    METHOD(ika_hooktimer) {
        PyObject*    pFunc;

        if (!PyArg_ParseTuple(args, "O:HookTimer", &pFunc))
            return 0;

        if (!PyCallable_Check(pFunc)) {
            PyErr_SetString(PyExc_TypeError, "HookTimer requires a function as a parameter");
            return 0;
        }

        Py_INCREF(pFunc);
        engine->_hookTimer.add(pFunc);

        Py_INCREF(Py_None);
        return Py_None;
    }


    // GAY GAY GAY GAY GAY GAY GAY GAY GAY GAY GAY GAY GAY GAY GAY GAY GAY
    METHOD(ika_unhooktimer) {
        PyObject* func = 0;

        if (!PyArg_ParseTuple(args, "|O:UnhookTimer", &func)) {
            return 0;
        }

        if (!func) {
            engine->_hookTimer.clear();

        } else {
            foreach (ScriptObject iter, engine->_hookTimer) {
                if (iter.get() == func) {
                    Py_DECREF(func);
                    engine->_hookTimer.remove(iter.get());
                    break;
                }
            }
        }
        Py_INCREF(Py_None);
        return Py_None;
    }

    METHOD(ika_setrenderlist) {
        // Compile the arguments into a vector.
        std::vector<uint> renderList(PyTuple_Size(args));
        
        for (uint i = 0; i < renderList.size(); i++) {
            PyObject* item = PyTuple_GetItem(args, i);
            if (!PyInt_Check(item)) {
                PyErr_SetString(PyExc_SyntaxError, "SetRenderList needs INTEGERS.");  return 0;
            }

            uint layerIndex = (uint)PyInt_AsLong(item);

            if (layerIndex >= engine->map.NumLayers()) {   
                PyErr_SetString(PyExc_RuntimeError, 
                    va("SetRenderList: asked to render layer %i.  The map only has %i layers!", 
                        layerIndex, engine->map.NumLayers())
                    );
                return 0;
            }

            renderList[i] = layerIndex;
        }

        engine->renderList = renderList;
        Py_INCREF(Py_None);
        return Py_None;
    }

    METHOD(ika_render) {
        if (PyTuple_Size(args) == 0) {
            engine->Render();

        } else {
            // Compile the arguments into a vector.
            std::vector<uint> renderList(PyTuple_Size(args));
            
            for (uint i = 0; i < renderList.size(); i++) {
                PyObject* item = PyTuple_GetItem(args, i);
                if (!PyInt_Check(item)) {
                    PyErr_SetString(PyExc_SyntaxError, "Map.Render needs INTEGERS.");  return 0;
                }

                uint layerIndex = (uint)PyInt_AsLong(item);

                if (layerIndex >= engine->map.NumLayers()) {
                    PyErr_SetString(PyExc_RuntimeError, 
                        va("Map.Render: asked to render layer %i.  The map only has %i layers! (valid layer indeces are 0-%i)", 
                            layerIndex, engine->map.NumLayers(), engine->map.NumLayers() - 1)
                        );
                    return 0;
                }

                renderList[i] = layerIndex;
            }

            // Pass them on.
            engine->Render(renderList);
        }

        Py_INCREF(Py_None);
        return Py_None;
    }

    METHOD(ika_setmappath) {
        char* s = NULL;

        if (!PyArg_ParseTuple(args, "|s:SetMapPath", &s))
            return 0;

        engine->_mapPath = s;

        Py_INCREF(Py_None);
        return Py_None;
    }

    PyMethodDef standard_methods[] = {
        //  name  | function

        // Misc
        { "Log",            (PyCFunction)ika_log,               METH_VARARGS,
            "Log(message)\n\n"
            "Writes a string to ika.log, if logging is enabled."
        },

        { "Exit",           (PyCFunction)ika_exit,              METH_VARARGS,
            "Exit([message])\n\n"
            "Exits ika immediately, displaying the message onscreen, if specified."
        },

        { "GetCaption",     (PyCFunction)ika_getcaption,        METH_NOARGS,
            "GetCaption() -> string\n\n"
            "Returns the caption on the ika window title bar."
        },

        { "SetCaption",     (PyCFunction)ika_setcaption,        METH_VARARGS,
            "SetCaption(newcaption)\n\n"
            "Sets the caption on the ika window title bar."
        },

        { "GetFrameRate",   (PyCFunction)ika_getframerate,      METH_NOARGS,
            "GetFrameRate() -> int\n\n"
            "Returns the current engine framerate, in frames per second."
        },

        { "Delay",          (PyCFunction)ika_delay,             METH_VARARGS,
            "Delay(time)\n\n"
            "Freezes the engine for a number of 'ticks'. (one tick is 1/100th of a second)"
        },

        { "Wait",           (PyCFunction)ika_wait,              METH_VARARGS,
            "Wait(time)\n\n"
            "Runs the engine for a number of ticks, disallowing player input.\n"
            "Unlike Delay, Wait causes entities to be processed, the tileset to be animated, and the map drawn."
        },

        { "GetTime",        (PyCFunction)ika_gettime,           METH_NOARGS,
            "GetTime() -> int\n\n"
            "Returns the number of ticks since the engine was started."
        },

        { "Random",         (PyCFunction)ika_random,            METH_VARARGS,
            "Random(min, max) -> int\n\n"
            "Returns a random integer less than or equal to min, and less than max.\n"
            "ie.  min <= value < max"
        },

        // Video

        { "RGB",            (PyCFunction)ika_rgb,               METH_VARARGS,
            "RGB(r, g, b[, a]) -> int\n\n"
            "Creates a 32bpp colour value from the four colour levels passed.  If alpha is\n"
            "omitted, it is assumed to be 255. (opaque)"
        },

        { "GetRGB",         (PyCFunction)ika_getrgb,            METH_VARARGS,
            "GetRGB(colour) -> tuple(int, int, int, int)\n\n"
            "Returns a 4-tuple containing the red, blue, green, and alpha values of the colour\n"
            "passed, respectively."
        },

        //{ "PaletteMorph", (PyCFunction)ika_palettemorph,      METH_VARARGS },

        // Entity
        { "ProcessEntities", (PyCFunction)ika_processentities,  METH_VARARGS,
            "ProcessEntities()\n\n"
            "Performs 1/100th of a second of entity AI.  Calling this 100 times a second\n"
            "will cause entities to move around as if the engine was in control."
        },

        { "SetCameraTarget", (PyCFunction)ika_setcameraTarget,  METH_VARARGS,
            "SetCameraTarget(entity)\n\n"
            "Sets the camera target to the entity specified.  If None is passed instead, \n"
            "the camera remains stationary, and can be altered with the Map.xwin and Map.ywin\n"
            "properties."
        },

        { "GetCameraTarget", (PyCFunction)ika_getcameraTarget,  METH_NOARGS,
            "GetCameraTarget() -> Entity\n\n"
            "Returns the entity that the camera is following, or None if it is free."
        },

        { "SetPlayer",      (PyCFunction)ika_setplayer,         METH_VARARGS,
            "SetPlayer(entity)\n\n"
            "Sets the player entity to the entity passed.  The player entity is the entity\n"
            "that moves according to user input.  Passing None instead unsets any player entity\n"
            "that may have been previously set."
        },

        { "GetPlayer",      (PyCFunction)ika_getplayer,         METH_NOARGS,
            "GetPlayer(entity) -> Entity\n\n"
            "Returns the current player entity, or None if there isn't one."
        },

        { "EntitiesAt",       (PyCFunction)ika_entitiesat,          METH_VARARGS,
            "EntitiesAt(x, y, width, height, layer) -> list\n\n"
            "Returns a list containing all entities within the given rect,\n"
            "on the given layer.  If there are no entities there, the empty\n"
            "list is returned."
        },

        { "HookRetrace",    (PyCFunction)ika_hookretrace,       METH_VARARGS,
            "HookRetrace(function)\n\n"
            "Adds the function to the retrace queue. (it will be called whenever the map is drawn, \n"
            "whether by Map.Render or by other means)"
        },

        { "UnhookRetrace",  (PyCFunction)ika_unhookretrace,     METH_VARARGS,
            "UnhookRetrace([function])\n\n"
            "Removes the function from the retrace queue if it is present.  If not, the call does\n"
            "nothing.  If the argument is omitted, then the list is cleared in its entirety."
        },

        { "HookTimer",      (PyCFunction)ika_hooktimer,         METH_VARARGS,
            "HookTimer(function)\n\n"
            "Adds the function to the timer queue. (the function will be called 100 times per second.\n"
            "This feature should be used sparingly, as it will cause serious problems if the queue\n"
            "cannot be executed in less than 1/100th of a second."
        },

        { "UnhookTimer",    (PyCFunction)ika_unhooktimer,       METH_VARARGS,
            "UnhookTimer([function])\n\n"
            "Removes the function from the timer queue if it is present.  If not, the call does\n"
            "nothing.  If the argument is omitted, then the list is cleared in its entirety."
        },

        { "SetRenderList",  (PyCFunction)ika_setrenderlist,     METH_VARARGS,
            "SetRenderList(index, ...)\n\n"
            "Sets the default rendering order used with Map.Render.  This can be useful for\n"
            "hiding layers, or moving them up and down in the ordering."
        },

        {   "Render",       (PyCFunction)ika_render,        METH_VARARGS,
            "Render([layerList])\n\n"
            "Redraw the scene.  layerList is an optional sequence of integers;\n"
            "ika will only draw these layers, in the order given, if specified.\n"
            "If layerList is omitted, all layers will be drawn, in their predefined\n"
            "order (set in the editor)\n\n"
            "If layerList is omitted, the screen is cleared to black before rendering\n"
            "begins."
        },

        {   "SetMapPath",   (PyCFunction)ika_setmappath,    METH_VARARGS,
            "SetMapPath(newmappath)\n\n"
            "Sets the folder for ika to load both map and tileset files from.\n"
            "Make to sure to end your folder with a trailing /!"
        },

        {    0    }
    };

#undef METHOD
#undef METHOD1
#undef METHOD2
}
