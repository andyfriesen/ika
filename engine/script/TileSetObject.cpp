
#include "Python.h"

#include "ObjectDefs.h"

#include "main.h"

#include <stdexcept>
#include <cassert>

namespace Script {
    namespace Tileset {
        PyTypeObject type;

        PyMethodDef methods[] = {
            {   "Save",         (PyCFunction)Tileset_Save,      METH_VARARGS,
                "Tileset.Save(filename)\n\n"
                "Save the tileset as the given name."
            },

            {   "Load",         (PyCFunction)Tileset_Load,      METH_VARARGS,
                "Tileset.Load(filename)\n\n"
                "Replace the current map's tileset with one loaded from a file."
            },

            { 0 }
        };

#define GET(x) PyObject* get ## x(TilesetObject* /*self*/)
#define SET(x) PyObject* set ## x(TilesetObject* /*self*/, PyObject* value)

        GET(TileCount)  { return PyInt_FromLong(engine->tiles->NumTiles()); }
        GET(TileWidth)  { return PyInt_FromLong(engine->tiles->Width());    }
        GET(TileHeight) { return PyInt_FromLong(engine->tiles->Height());   }

#undef GET
#undef SET

        PyGetSetDef properties[] =
        {
            {   (char*)"tilecount",        (getter)getTileCount,               0,   (char*)"Gets the number of tiles in the tileset."                         },
            {   (char*)"width",            (getter)getTileWidth,               0,   (char*)"Gets the width of the tiles in the tileset."                      },
            {   (char*)"height",           (getter)getTileHeight,              0,   (char*)"Gets the height of the tiles in the tileset."                     },

            { 0 }
        };

        void Init() {
            memset(&type, 0, sizeof type);

            type.ob_refcnt = 1;
            type.ob_type = &PyType_Type;
            type.tp_name = "Tileset";
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

        void Destroy(TilesetObject* self) {
            // ???
            PyObject_Del(self);
        }

#define METHOD(x) PyObject* x(TilesetObject* /*self*/, PyObject* args)

        METHOD(Tileset_Save) {
            char* fileName = 0;

            if (!PyArg_ParseTuple(args, "s:Save", &fileName)) {
                return 0;
            }

            engine->tiles->Save(fileName);

            Py_INCREF(Py_None);
            return Py_None;
        }

        METHOD(Tileset_Load) {
            char* fileName = 0;

            if (!PyArg_ParseTuple(args, "s:Load", &fileName)) {
                return 0;
            }

            if (!engine->_isMapLoaded) {
                PyErr_SetString(PyExc_IOError, "Can't load a tileset before a map!!");
                return 0;
            }

            ::Tileset* newTiles = 0;
            try {
                newTiles = new ::Tileset(fileName, engine->video);
            } catch (std::runtime_error err) {
                PyErr_SetString(PyExc_IOError, va("Unable to load tileset \"%s\"", fileName));
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
