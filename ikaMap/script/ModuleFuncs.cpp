
#include "ObjectDefs.h"
#include "log.h"

#define METHOD(x)  PyObject* x(PyObject* self, PyObject* args)
#define METHOD1(x) PyObject* x(PyObject*)

namespace ScriptObject
{
    METHOD(std_log)
    {
        char* message;

        if (!PyArg_ParseTuple(args, "s:log", &message))
            return 0;

        Log::Write(message);

        Py_INCREF(Py_None);
        return Py_None;                                    // returning void :)
    }

    /*METHOD(std_exit)
    {
        char* message="";

        if (!PyArg_ParseTuple(args, "|s:exit", &message))
            return 0;

        engine->Sys_Error(message);

        // never gets called... I hope.
        Py_INCREF(Py_None);
        return Py_None;
    }*/

    /*METHOD(std_entityat)
    {
        int x, y, width, height;
        if (!PyArg_ParseTuple(args, "iiii|EntityAt", &x, &y, &width, &height))
            return 0;

        int x2=x+width;
        int y2=y+height;

        int count = 0;
        PyObject* pKey = 0;
        PyObject* pValue = 0;
        while (PyDict_Next(entityDict, &count, &pKey, &pValue))
        {
            if (pValue->ob_type != &ScriptObject::Entity::type)
            {
                Log::Write("Someone's being a goober and putting non-entities in ika.Map.entities! >:(");
                continue;
            }

            ::Entity* ent=((ScriptObject::Entity::EntityObject*)pValue)->ent;
            
            if (x>ent->x+ent->sprite->nHotw)    continue;
            if (y>ent->y+ent->sprite->nHoth)    continue;
            if (x2<ent->x)    continue;
            if (y2<ent->y)    continue;

            Py_INCREF(pValue);
            return pValue;
        }

        Py_INCREF(Py_None);
        return Py_None;
    }*/

    PyMethodDef standard_methods[] =
    {
        //  name  | function

        // Misc
        { "Log",            (PyCFunction)std_log,               METH_VARARGS,
            "Log(message)\n\n"
            "Writes a string to ika.log, if logging is enabled."
        },

        /*{ "Exit",           (PyCFunction)std_exit,              METH_VARARGS,
            "Exit([message])\n\n"
            "Exits ika immediately, displaying the message onscreen, if specified."
        },*/

        /*{ "EntityAt",       (PyCFunction)std_entityat,          METH_VARARGS,
            "EntityAt(x, y, width, height) -> Entity\n\n"
            "If there is an entity within the rectangle passed, it is returned.\n"
            "If not, then None is returned."
        },*/

        {    0    }
    };

#undef METHOD
#undef METHOD1
}
