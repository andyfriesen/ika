
#include "Python.h"

#include "ObjectDefs.h"

#include "main.h"

#include <cassert>

namespace Script {
    namespace TileSet {
        PyTypeObject type;

        PyMethodDef methods[] = {
            {   "Save",         (PyCFunction)TileSet_Save,      METH_VARARGS,
                "TileSet.Save(filename)\n\n"
                "Save the tileset as the given name."
            },

            {   "Load",         (PyCFunction)TileSet_Load,      METH_VARARGS,
                "TileSet.Load(filename)\n\n"
                "Replace the current map's tileset with one loaded from a file."
            },

            { 0 }
        };

#define GET(x) PyObject* get ## x(TileSetObject* self)
#define SET(x) PyObject* set ## x(TileSetObject* self, PyObject* value)

        GET(TileCount)  { return PyInt_FromLong(engine->tiles->NumTiles()); }
        GET(TileWidth)  { return PyInt_FromLong(engine->tiles->Width());    }
        GET(TileHeight) { return PyInt_FromLong(engine->tiles->Height());   }

#undef GET
#undef SET

        PyGetSetDef properties[] =
        {
            {   "tilecount",        (getter)getTileCount,               0,   "Gets the number of tiles in the tileset."                         },
            {   "width",            (getter)getTileWidth,               0,   "Gets the width of the tiles in the tileset."                      },
            {   "height",           (getter)getTileHeight,              0,   "Gets the height of the tiles in the tileset."                     },

            { 0 }
        };

        void Init() {
            memset(&type, 0, sizeof type);

            type.ob_refcnt = 1;
            type.ob_type = &PyType_Type;
            type.tp_name = "TileSet";
            type.tp_basicsize = sizeof type;
            type.tp_dealloc = (destructor)Destroy;
            type.tp_methods = methods;
            type.tp_getset  = properties;
            type.tp_doc = "Represents the current tileset";

            PyType_Ready(&type);
        }

        PyObject* New() {
            PyObject* obj = PyObject_New(PyObject, &type);
            assert(obj != 0);
            return obj;
        }

        void Destroy(TileSetObject* self) {
            // ???
            PyObject_Del(self);
        }

#define METHOD(x) PyObject* x(TileSetObject* self, PyObject* args)

        METHOD(TileSet_Save) {
            char* fileName = 0;

            if (!PyArg_ParseTuple(args, "s:Save", &fileName)) {
                return 0;
            }

            engine->tiles->Save(fileName);

            Py_INCREF(Py_None);
            return Py_None;
        }

        METHOD(TileSet_Load) {
            char* fileName = 0;

            if (!PyArg_ParseTuple(args, "s:Load", &fileName)) {
                return 0;
            }

            if (!engine->_isMapLoaded) {
                PyErr_SetString(PyExc_RuntimeError, "Can't load a tileset before a map!!");
                return 0;
            }

            ::TileSet* newTiles = 0;
            try {
                newTiles = new ::TileSet(fileName, engine->video);
            } catch (std::runtime_error err) {
                PyErr_SetString(PyExc_RuntimeError, va("Unable to load tileset \"%s\"", fileName));
                return 0;
            }

            delete engine->tiles;
            engine->tiles = newTiles;

            Py_INCREF(Py_None);
            return Py_None;
        }

#undef METHOD
    }
}
