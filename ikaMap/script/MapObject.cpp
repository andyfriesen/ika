/*
Map manipulation junque.
*/
#include "ObjectDefs.h"

#include "../mainwindow.h"
#include "../command.h"
#include "../tileset.h"

#include "common/map.h"

namespace ScriptObject
{
    namespace Map
    {
        ::Map* MapObject::GetMap() const
        {
            return mainWnd->GetMap();
        }

        PyTypeObject type;

        PyMethodDef methods[] =
        {
            {   "GetTile",      (PyCFunction)Map_GetTile,       METH_VARARGS,
                "GetTile(x, y, layer) -> int\n\n"
                "Returns the index of the tile at (x, y) on the layer specified."
            },

            {   "SetTile",      (PyCFunction)Map_SetTile,       METH_VARARGS,
                "SetTile(x, y, layer, tile)\n\n"
                "Sets the tile at (x, y) of the layer specified."
            },

            {   "GetObs",       (PyCFunction)Map_GetObs,        METH_VARARGS,
                "GetObs(x, y, layerIndex) -> int\n\n"
                "Returns 1 if the tile at (x, y) is obstructed, or 0 if not."
            },

            {   "SetObs",       (PyCFunction)Map_SetObs,        METH_VARARGS,
                "SetObs(x, y, layerIndex, obs)\n\n"
                "If obs is nonzero, the tile at (x, y) is obstructed, else it is\n"
                "unobstructed."
            },

            {   "GetMetaData",  (PyCFunction)Map_GetMetaData,   METH_NOARGS,
                "GetMetaData() -> dict\n\n"
                "Creates a dictionary containing the map's metadata, and returns it."
            },

            {   "SetMetaData", (PyCFunction)Map_SetMetaData, METH_VARARGS,
                "SetMetaData(metaData)\n\n"
                "Copies the dict into the map's metadata.  The dict must contain nothing\n"
                "but strings for both keys and values."
            },


            {   "GetLayerName", (PyCFunction)Map_GetLayerName,  METH_VARARGS,
                "GetLayerName(layerIndex) -> string\n\n"
                "Returns the name of the layer with the given index."
            },

            {   "GetLayerSize", (PyCFunction)Map_GetLayerSize,  METH_VARARGS,
                "GetLayerSize(layerIndex) -> (x,y)\n\n"
                "Returns a tuple containing the size of the layer."
            },

            {   "FindLayerByName", (PyCFunction)Map_FindLayerByName,    METH_VARARGS,
                "FindLayerByName(name) -> integer\n\n"
                "Returns the index of the first layer with the given name, or None if no such layer\n"
                "exists."
            },

            {   "GetParallax",  (PyCFunction)Map_GetParallax,   METH_VARARGS,
                "GetParallax(layer) -> (int, int, int, int)\n\n"
                "Returns a 4-tuple containing parallax settings for the layer specified.\n"
                "Its contents are as follows:\n"
                "( parallax X multiplier, \n"
                "  parallax X divisor, \n"
                "  parallax Y multiplier, \n"
                "  parallax Y divisor )"
            },

            {   "SetParallax",  (PyCFunction)Map_SetParallax,   METH_VARARGS,
                "SetParallax(layer, xmul, xdiv, ymul, ydiv)\n\n"
                "Sets the specified layer's parallax settings according to the multipliers\n"
                "and divisors given.  If either of the divisors are zero, a parallax value of 0/1\n"
                "will be used for that axis."
            },

            {   0   }   // end of list
        };

#define GET(x) PyObject* get ## x(MapObject* self)
#define SET(x) PyObject* set ## x(MapObject* self, PyObject* value)

        GET(Title)      { return PyString_FromString(self->GetMap()->title.c_str()); }
        SET(Title)      { self->GetMap()->title = PyString_AsString(value); return 0;   }
        GET(LayerCount) { return PyInt_FromLong(self->GetMap()->NumLayers());   }
        GET(TileCount)  { return PyInt_FromLong(self->mainWnd->GetTileset()->Count()); }
        GET(TileWidth)  { return PyInt_FromLong(self->mainWnd->GetTileset()->Width()); }
        GET(TileHeight) { return PyInt_FromLong(self->mainWnd->GetTileset()->Height()); }

#undef GET
#undef SET

        PyGetSetDef properties[] =
        {
            {   "title",        (getter)getTitle,           (setter)setTitle,   "Gets or sets the map's title."                     },
            {   "layercount",   (getter)getLayerCount,      0,                  "Gets the number of layers on the current map."     },
            {   "tilecount",    (getter)getTileCount,       0,                  "Gets the number of tiles in the current tileset"   },
            {   "tilewidth",    (getter)getTileWidth,       0,                  "Gets the width of the current tileset"             },
            {   "tileheight",   (getter)getTileHeight,      0,                  "Gets the height of the current tileset"            },
            {   0   }
        };

        void Init()
        {
            memset(&type, 0, sizeof type);

            type.ob_refcnt = 1;
            type.ob_type = &PyType_Type;
            type.tp_name = "Map";
            type.tp_basicsize = sizeof type;
            type.tp_dealloc = (destructor)Destroy;
            type.tp_methods = methods;
            type.tp_getset  = properties;
            type.tp_doc = "Represents the current map";

            PyType_Ready(&type);
        }

        PyObject* New(MainWindow* mainWnd)
        {
            MapObject* map = PyObject_New(MapObject, &type);

            if (!map)
                return 0;

            map->mainWnd = mainWnd;

            return (PyObject*)map;
        }

        void Destroy(PyObject* self)
        {
            PyObject_Del(self);
        }

#define METHOD(x) PyObject* x(MapObject* self, PyObject* args)

        METHOD(Map_GetTile)
        {
            int x, y;
            uint lay;

            if (!PyArg_ParseTuple(args, "iii:Map.GetTile", &x, &y, &lay))
                return 0;

            if (lay >= self->GetMap()->NumLayers())
            {
                // Error checking crap can wait.
                return PyInt_FromLong(0);
                //PyErr_SetString(PyExc_RuntimeError, va("Cannot GetTile from layer %i.  The map only has %i layers.", lay, self->GetMap()->NumLayers()));
                return 0;
            }

            return PyInt_FromLong(self->GetMap()->GetLayer(lay)->tiles(x, y));
        }

        METHOD(Map_SetTile)
        {
            int x, y;
            uint lay;
            uint tile;

            if (!PyArg_ParseTuple(args, "iiii:Map.SetTile", &x, &y, &lay, &tile))
                return 0;

            if (lay >= self->GetMap()->NumLayers())
            {
                Py_INCREF(Py_None); return Py_None;
                //PyErr_SetString(PyExc_RuntimeError, va("Cannot SetTile to layer %i.  The map only has %i layers.", lay, self->GetMap()->NumLayers()));
                //return 0;
            }

            self->mainWnd->HandleCommand(new SetTileCommand(x, y, lay, tile));

            Py_INCREF(Py_None);
            return Py_None;
        }

        METHOD(Map_GetObs)
        {
            int x, y;
            uint lay;

            if (!PyArg_ParseTuple(args, "iii:Map.GetObs", &x, &y, &lay))
                return 0;

            if (lay >= self->GetMap()->NumLayers())
            {
                Py_INCREF(Py_None); return Py_None;
                //PyErr_SetString(PyExc_RuntimeError, va("Cannot GetObs from layer %i.  The map only has %i layers.", lay, self->GetMap()->NumLayers()));
                //return 0;
            }

            return PyInt_FromLong(self->GetMap()->GetLayer(lay)->obstructions(x, y));
        }

        METHOD(Map_SetObs)
        {
            int x, y;
            uint lay;
            int set;

            if (!PyArg_ParseTuple(args, "iiii:Map.SetObs", &x, &y, &lay, &set))
                return 0;

            if (lay >= self->GetMap()->NumLayers())
            {
                Py_INCREF(Py_None); return Py_None;
                //PyErr_SetString(PyExc_RuntimeError, va("Cannot SetObs to layer %i.  The map only has %i layers.", lay, self->GetMap()->NumLayers()));
                //return 0;
            }

            self->mainWnd->HandleCommand(new SetObstructionCommand(x, y, lay, (u8)set));

            Py_INCREF(Py_None);
            return Py_None;
        }

        METHOD(Map_GetMetaData)
        {
            PyObject* dict = PyDict_New();

            for (std::map<std::string, std::string>::iterator iter = self->GetMap()->metaData.begin(); iter != self->GetMap()->metaData.end(); iter++)
                PyDict_SetItemString(dict, const_cast<char*>(iter->first.c_str()), PyString_FromString(iter->second.c_str()));

            return dict;
        }

        METHOD(Map_SetMetaData)
        {
            PyObject* dict = 0;

            if (!PyArg_ParseTuple(args, "O!:Map.SetMetaData", &PyDict_Type, &dict))
                return 0;

            std::map<std::string, std::string>& metaData = self->GetMap()->metaData;
            metaData.clear();

            int count = 0;
            PyObject* key = 0;
            PyObject* value = 0;
            while (PyDict_Next(dict, &count, &key, &value))
            {
                const char* k = PyString_AsString(key);
                const char* v = PyString_AsString(value);
                // TODO: Report an error if either k or v is null

                if (k && v)
                    metaData[k] = v;
            }

            Py_INCREF(Py_None);
            return Py_None;
        }

        METHOD(Map_GetLayerName)
        {
            uint index;

            if (!PyArg_ParseTuple(args, "i:Map.GetLayerName", &index))
                return 0;

            if (index >= self->GetMap()->NumLayers())
            {
                PyErr_SetString(PyExc_RuntimeError, va("The map has no layer number %i", index));
                return 0;
            }

            return PyString_FromString(self->GetMap()->GetLayer(index)->label.c_str());
        }

        METHOD(Map_GetLayerSize)
        {
            uint index;

            if (!PyArg_ParseTuple(args, "i:Map.GetLayerSize", &index))
                return 0;

            if (index >= self->GetMap()->NumLayers())
            {
                PyErr_SetString(PyExc_RuntimeError, va("The map has no layer number %i", index));
                return 0;
            }

            ::Map::Layer* layer = self->GetMap()->GetLayer(index);
            return Py_BuildValue("(ii)", layer->Width(), layer->Height());
        }

        METHOD(Map_FindLayerByName)
        {
            char* name;

            if (!PyArg_ParseTuple(args, "s:Map.FindLayerByName", &name))
                return 0;

            for (uint i = 0; i < self->GetMap()->NumLayers(); i++)
            {
                if (self->GetMap()->GetLayer(i)->label == name)
                    return PyInt_FromLong(i);
            }

            Py_INCREF(Py_None);
            return Py_None;
        }

        METHOD(Map_GetParallax)
        {
            uint lay;

            if (!PyArg_ParseTuple(args, "i:Map.GetParallax", &lay))
                return 0;

            if (lay >= self->GetMap()->NumLayers())
            {
                PyErr_SetString(PyExc_RuntimeError, va("Cannot GetParallax from layer %i.  The map only has %i layers.", lay, self->GetMap()->NumLayers()));
                return 0;
            }

            ::Map::Layer* layer = self->GetMap()->GetLayer(lay);

            return Py_BuildValue("(iiii)",
                layer->parallax.mulx,
                layer->parallax.divx,
                layer->parallax.muly,
                layer->parallax.divy
                );
        }

        METHOD(Map_SetParallax)
        {
            int pmulx, pdivx, pmuly, pdivy;
            uint lay;

            if (!PyArg_ParseTuple(args, "iiiii:Map.SetParallax", &lay, &pmulx, &pdivx, &pmuly, &pdivy))
                return 0;

            if (lay >= self->GetMap()->NumLayers())
            {
                PyErr_SetString(PyExc_RuntimeError, va("Cannot SetParallax to layer %i.  The map only has %i layers.", lay, self->GetMap()->NumLayers()));
                return 0;
            }

            if (pdivx == 0)
                pmulx = 0, pdivx = 1;
            if (pdivy == 0)
                pmuly = 0, pdivy = 1;

            ::Map::Layer* layer = self->GetMap()->GetLayer(lay);
            layer->parallax.mulx = pmulx;
            layer->parallax.divx = pdivx;
            layer->parallax.muly = pmulx;
            layer->parallax.divy = pdivy;

            Py_INCREF(Py_None);
            return Py_None;
        }

#undef METHOD
    }
}
