/*
Map manipulation junque.
*/
#include "ObjectDefs.h"
#include "main.h"
#include "common/fileio.h"

namespace Script
{
    namespace Map
    {
        PyTypeObject type;

        PyMethodDef methods[] =
        {
            {   "Switch",       (PyCFunction)Map_Switch,        METH_VARARGS,
                "Switch(filename)\n\n"
                "Switches the current map to the map file specified.\n"
                "The new map's AutoExec event is executed, if it exists."
            },

            {   "GetMetaData",  (PyCFunction)Map_GetMetaData,   METH_NOARGS,
                "GetMetaData() -> dict\n\n"
                "Creates a dictionary containing the map's metadata, and returns it."
            },

            {   "Render",       (PyCFunction)Map_Render,        METH_VARARGS,
                "Render([layerList])\n\n"
                "Draws the map and entities.  layerList is a sequence of integers;\n"
                "ika will only draw these layers (in the order given).  If layerList\n"
                "is omitted, all layers will be drawn, in their predefined order (set\n"
                "in the editor)\n\n"
                "If layerList is omitted, the screen is cleared to black before rendering\n"
                "begins."
            },

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

            /*{   "GetZone",      (PyCFunction)Map_GetZone,       METH_VARARGS,
                "GetZone(x, y) -> int\n\n"
                "Returns the id number of the zone at (x, y)"
            },
            {   "SetZone",      (PyCFunction)Map_SetZone,       METH_VARARGS,
                "SetZone(x, y, zone)\n\n"
                "Sets the zone id number at (x, y)."
            },*/

            {   "GetLayerName", (PyCFunction)Map_GetLayerName,  METH_VARARGS,
                "GetLayerName(layerIndex) -> string\n\n"
                "Returns the name of the layer with the given index."
            },

            {   "SetLayerName", (PyCFunction)Map_SetLayerName,  METH_VARARGS,
                "SetLayerName(layerIndex, newName)\n\n"
                "Sets the layer's name to newName.  Subsequent GetLayerName and FindLayerByName will\n"
                "use this as the layer's name, as if it were set in the editor."
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

            {   "GetLayerProperties",   (PyCFunction)Map_GetLayerProperties,    METH_VARARGS,
                "GetLayerProperties(layerIndex) -> (label, width, height, wrapx, wrapy)\n\n"
                "Returns a 5-tuple containing information about the map layer specified."
            },

            {   "GetLayerPosition",     (PyCFunction)Map_GetLayerPosition,  METH_VARARGS,
                "GetLayerPosition(layerIndex) -> (x, y)\n\n"
                "Returns a tuple containing the layer's position on the map. (in pixels)"
            },

            {   "SetLayerPosition",     (PyCFunction)Map_SetLayerPosition,  METH_VARARGS,
                "SetLayerPosition(layerIndex, x, y)\n\n"
                "Sets the layer's position to the (x,y) coordinates specified."
            },

            {   "GetWaypoints", (PyCFunction)Map_GetWaypoints,  METH_NOARGS,
                "GetWaypoints() -> list\n\n"
                "Returns a list of three-tuples in the format of (name, x, y), one for\n"
                "each waypoint defined within the editor."
            },

            {   "GetAllEntities",   (PyCFunction)Map_GetAllEntities,    METH_NOARGS,
                "GetAllEntities() -> list\n\n"
                "Creates a list of every single existing entity, and returns it."
            },

            {   0   }   // end of list
        };

#define GET(x) PyObject* get ## x(PyObject* self)
#define SET(x) PyObject* set ## x(PyObject* self, PyObject* value)

        GET(Title)      { return PyString_FromString(engine->map.title.c_str()); }
        SET(Title)      { engine->map.title = PyString_AsString(value); return 0;   }
        GET(XWin)       { return PyInt_FromLong(engine->GetCamera().x); }
        SET(XWin)       { engine->SetCamera(Point(PyInt_AsLong(value), engine->GetCamera().y)); return 0; }
        GET(YWin)       { return PyInt_FromLong(engine->GetCamera().y); }
        SET(YWin)       { engine->SetCamera(Point(engine->GetCamera().x, PyInt_AsLong(value)));  return 0;}
        GET(LayerCount) { return PyInt_FromLong(engine->map.NumLayers());   }
        GET(NumTiles)   { return PyInt_FromLong(engine->tiles->NumTiles()); }
        GET(TileWidth)  { return PyInt_FromLong(engine->tiles->Width()); }
        GET(TileHeight) { return PyInt_FromLong(engine->tiles->Height()); }
        GET(Width)      { return PyInt_FromLong(engine->map.width); }
        GET(Height)     { return PyInt_FromLong(engine->map.height); }
        //GET(RString)    { return PyString_FromString(engine->map.GetRString().c_str()); }
        //SET(RString)    { engine->map.SetRString(PyString_AsString(value));     return 0;   }
        GET(TileSetName)    { return PyString_FromString(engine->map.tileSetName.c_str()); }
        SET(TileSetName)    { engine->map.tileSetName = PyString_AsString(value);     return 0;   }
        //GET(Music)      { return PyString_FromString(engine->map.GetMusic().c_str()); }
        //SET(Music)      { engine->map.SetMusic(PyString_AsString(value));       return 0;   }
        GET(Entities)   { Py_INCREF(entityDict); return entityDict; }

#undef GET
#undef SET

        PyGetSetDef properties[] =
        {
            {   "title",        (getter)getTitle,           (setter)setTitle,   "Gets or sets the map's title."                     },
            {   "xwin",         (getter)getXWin,            (setter)setXWin,    "Gets or sets the X coordinate of the camera"       },
            {   "ywin",         (getter)getYWin,            (setter)setYWin,    "Gets or sets the Y coordinate of the camera"       },
            {   "layercount",   (getter)getLayerCount,      0,                  "Gets the number of layers on the current map."     },
            {   "numtiles",     (getter)getNumTiles,        0,                  "Gets the number of tiles in the current tileset"   },
            {   "tilewidth",    (getter)getTileWidth,       0,                  "Gets the width of the current tileset"             },
            {   "tileheight",   (getter)getTileHeight,      0,                  "Gets the height of the current tileset"            },
            {   "width",        (getter)getWidth,           0,                  "Gets the width of the current map, in pixels"      },
            {   "height",       (getter)getHeight,          0,                  "Gets the height of the current map, in pixels"     },
            {   "tilesetname",  (getter)getTileSetName,         (setter)setTileSetName, "Gets or sets the name of the current tileset"  },
            {   "entities",     (getter)getEntities,        0,                  "Gets a dictionary of entities currently tied to the map"   },
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

        PyObject* New()
        {
            PyObject* map = PyObject_New(PyObject, &type);

            if (!map)
                return 0;

            return (PyObject*)map;
        }

        void Destroy(PyObject* self)
        {
            PyObject_Del(self);
        }

#define METHOD(x) PyObject* x(PyObject* self, PyObject* args)

        METHOD(Map_Switch)
        {
            char* filename;

            if (!PyArg_ParseTuple(args, "s:MapSwitch", &filename))
                return 0;

            if (!File::Exists(filename))
            {
                PyErr_SetString(PyExc_OSError, va("Unable to load %s", filename));
                return 0;
            }

            engine->LoadMap(filename);

            Py_INCREF(Py_None);
            return Py_None;
        }
        
        METHOD(Map_GetMetaData)
        {
            PyObject* dict = PyDict_New();

            for (std::map<std::string, std::string>::iterator iter = engine->map.metaData.begin(); iter != engine->map.metaData.end(); iter++)
                PyDict_SetItemString(dict, const_cast<char*>(iter->first.c_str()), PyString_FromString(iter->second.c_str()));

            return dict;
        }

        METHOD(Map_Render)
        {

            if (PyTuple_Size(args) == 0)
                engine->Render();
            else
            {
                // Compile the arguments into a vector.
                std::vector<uint> renderList(PyTuple_Size(args));
                
                for (uint i = 0; i < renderList.size(); i++)
                {
                    PyObject* item = PyTuple_GetItem(args, i);
                    if (!PyInt_Check(item))
                    {
                        PyErr_SetString(PyExc_SyntaxError, "Map.Render needs INTEGERS.");  return 0;
                    }

                    uint layerIndex = (uint)PyInt_AsLong(item);

                    if (layerIndex >= engine->map.NumLayers())
                    {   
                        PyErr_SetString(PyExc_RuntimeError, 
                            va("Map.Render: asked to render layer %i.  The map only has %i layers!", 
                                layerIndex, engine->map.NumLayers())
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

        METHOD(Map_GetTile)
        {
            int x, y;
            uint lay;

            if (!PyArg_ParseTuple(args, "iii:Map.GetTile", &x, &y, &lay))
                return 0;

            if (lay >= engine->map.NumLayers())
            {
                PyErr_SetString(PyExc_RuntimeError, va("Cannot GetTile from layer %i.  The map only has %i layers.", lay, engine->map.NumLayers()));
                return 0;
            }

            return PyInt_FromLong(engine->map.GetLayer(lay)->tiles(x, y));
        }

        METHOD(Map_SetTile)
        {
            int x, y, tile;
            uint lay;

            if (!PyArg_ParseTuple(args, "iiii:Map.SetTile", &x, &y, &lay, &tile))
                return 0;

            if (lay >= engine->map.NumLayers())
            {
                PyErr_SetString(PyExc_RuntimeError, va("Cannot SetTile to layer %i.  The map only has %i layers.", lay, engine->map.NumLayers()));
                return 0;
            }

            engine->map.GetLayer(lay)->tiles(x, y) = tile;

            Py_INCREF(Py_None);
            return Py_None;
        }

        METHOD(Map_GetObs)
        {
            int x, y;
            uint lay;

            if (!PyArg_ParseTuple(args, "iii:Map.GetObs", &x, &y, &lay))
                return 0;

            if (lay >= engine->map.NumLayers())
            {
                PyErr_SetString(PyExc_RuntimeError, va("Cannot GetObs from layer %i.  The map only has %i layers.", lay, engine->map.NumLayers()));
                return 0;
            }

            return PyInt_FromLong(engine->map.GetLayer(lay)->obstructions(x, y));
        }

        METHOD(Map_SetObs)
        {
            int x, y, set;
            uint lay;

            if (!PyArg_ParseTuple(args, "iiii:Map.SetObs", &x, &y, &lay, &set))
                return 0;

            if (lay >= engine->map.NumLayers())
            {
                PyErr_SetString(PyExc_RuntimeError, va("Cannot SetObs to layer %i.  The map only has %i layers.", lay, engine->map.NumLayers()));
                return 0;
            }

            engine->map.GetLayer(lay)->obstructions(x, y) = set != 0;

            Py_INCREF(Py_None);
            return Py_None;
        }

        /*METHOD(Map_GetZone)
        {
            int x, y;

            if (!PyArg_ParseTuple(args, "ii:Map.GetZone", &x, &y))
                return 0;

            return PyInt_FromLong(engine->map.GetZone(x, y));
        }

        METHOD(Map_SetZone)
        {
            int x, y, z;

            if (!PyArg_ParseTuple(args, "iii:Map.SetZone", &x, &y, &z))
                return 0;

            engine->map.SetZone(x, y, z);

            Py_INCREF(Py_None);
            return Py_None;
        }*/

        METHOD(Map_GetLayerName)
        {
            uint index;

            if (!PyArg_ParseTuple(args, "i:Map.GetLayerName", &index))
                return 0;

            if (index >= engine->map.NumLayers())
            {
                PyErr_SetString(PyExc_RuntimeError, va("The map has no layer number %i", index));
                return 0;
            }

            return PyString_FromString(engine->map.GetLayer(index)->label.c_str());
        }

        METHOD(Map_SetLayerName)
        {
            uint index;
            char* newName;

            if (!PyArg_ParseTuple(args, "is:Map.SetLayerName", &index, &newName))
                return 0;

            if (index >= engine->map.NumLayers())
            {
                PyErr_SetString(PyExc_RuntimeError, va("The map has no layer number %i", index));
                return 0;
            }

            engine->map.GetLayer(index)->label = newName;

            Py_INCREF(Py_None);
            return Py_None;
        }

        METHOD(Map_FindLayerByName)
        {
            char* name;

            if (!PyArg_ParseTuple(args, "s:Map.FindLayerByName", &name))
                return 0;

            for (uint i = 0; i < engine->map.NumLayers(); i++)
            {
                if (engine->map.GetLayer(i)->label == name)
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

            if (lay >= engine->map.NumLayers())
            {
                PyErr_SetString(PyExc_RuntimeError, va("Cannot GetParallax from layer %i.  The map only has %i layers.", lay, engine->map.NumLayers()));
                return 0;
            }

            ::Map::Layer* layer = engine->map.GetLayer(lay);

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

            if (lay >= engine->map.NumLayers())
            {
                PyErr_SetString(PyExc_RuntimeError, va("Cannot SetParallax to layer %i.  The map only has %i layers.", lay, engine->map.NumLayers()));
                return 0;
            }

            if (pdivx == 0)
                pmulx = 0, pdivx = 1;
            if (pdivy == 0)
                pmuly = 0, pdivy = 1;

            ::Map::Layer* layer = engine->map.GetLayer(lay);
            layer->parallax.mulx = pmulx;
            layer->parallax.divx = pdivx;
            layer->parallax.muly = pmulx;
            layer->parallax.divy = pdivy;

            Py_INCREF(Py_None);
            return Py_None;
        }

        METHOD(Map_GetLayerProperties)
        {
            uint lay;

            if (!PyArg_ParseTuple(args, "i:Map.GetLayerProperties", &lay))
                return 0;

            if (lay >= engine->map.NumLayers())
            {
                PyErr_SetString(PyExc_RuntimeError, va("Cannot get properties of layer %i.  The map only has %i layers.", lay, engine->map.NumLayers()));
                return 0;
            }

            const ::Map::Layer* layer = engine->map.GetLayer(lay);

            return Py_BuildValue("(siiii)", layer->label.c_str(), layer->tiles.Width(), layer->tiles.Height(), layer->wrapx ? 1 : 0, layer->wrapy ? 1 : 0);
        }

        METHOD(Map_GetLayerPosition)
        {
            uint lay;

            if (!PyArg_ParseTuple(args, "i:Map.GetLayerPosition", &lay))
                return 0;

            if (lay >= engine->map.NumLayers())
            {
                PyErr_SetString(PyExc_RuntimeError, va("Cannot get position of layer %i.  The map only has %i layers.", lay, engine->map.NumLayers()));
                return 0;
            }

            const ::Map::Layer* layer = engine->map.GetLayer(lay);

            return Py_BuildValue("(ii)", layer->x, layer->y);
        }

        METHOD(Map_SetLayerPosition)
        {
            uint lay;
            int x, y;

            if (!PyArg_ParseTuple(args, "iii:Map.GetLayerPosition", &lay, &x, &y))
                return 0;

            if (lay >= engine->map.NumLayers())
            {
                PyErr_SetString(PyExc_RuntimeError, va("Cannot set position of layer %i.  The map only has %i layers.", lay, engine->map.NumLayers()));
                return 0;
            }

            ::Map::Layer* layer = engine->map.GetLayer(lay);

            layer->x = x;
            layer->y = y;

            Py_INCREF(Py_None);
            return Py_None;
        }

        METHOD(Map_GetWaypoints)
        {
            if (!PyArg_ParseTuple(args, ":Map.GetWaypoints"))
                return 0;

            uint s = engine->map.wayPoints.size();
            PyObject* list = PyTuple_New(s);
            uint i = 0;
            
            for (
                std::map<std::string, ::Map::WayPoint>::iterator iter = engine->map.wayPoints.begin(); 
                iter != engine->map.wayPoints.end(); 
                iter++)
            {
                ::Map::WayPoint& wp = iter->second;

                PyObject* o = PyTuple_New(3);
                PyTuple_SET_ITEM(o, 0, PyString_FromString(wp.label.c_str()));
                PyTuple_SET_ITEM(o, 1, PyInt_FromLong(wp.x));
                PyTuple_SET_ITEM(o, 2, PyInt_FromLong(wp.y));

                PyTuple_SET_ITEM(list, i, o);
                i++;
            }

            return list;
        }

        METHOD(Map_GetAllEntities)
        {
            PyObject* list = PyList_New(Script::Entity::instances.size());

            int i = 0;
            for (std::map< ::Entity*, Script::Entity::EntityObject*>::iterator iter = Script::Entity::instances.begin();
                iter != Script::Entity::instances.end();
                iter++)
            {
                Py_INCREF(iter->second);
                PyList_SetItem(list, i, reinterpret_cast<PyObject*>(iter->second));
                i++;
            }

            return list;

            /*
                def GetEntitiesOnLayer(layerIndex):
                    return [ent for ent in ika.Map.GetAllEntities() if ent.layer == layerIndex]
            */
        }

#undef METHOD
    }
}
