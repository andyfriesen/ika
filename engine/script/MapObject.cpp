/*
Map manipulation junque.
*/
#include "ObjectDefs.h"
#include "main.h"

namespace Script
{
    namespace Map
    {
        PyTypeObject type;

        PyMethodDef methods[] =
        {
            {   "Switch",       (PyCFunction)Map_Switch,        1   },
            {   "Render",       (PyCFunction)Map_Render,        1   },
            {   "GetTile",      (PyCFunction)Map_GetTile,       1   },
            {   "SetTile",      (PyCFunction)Map_SetTile,       1   },
            {   "GetObs",       (PyCFunction)Map_GetObs,        1   },
            {   "SetObs",       (PyCFunction)Map_SetObs,        1   },
            {   "GetZone",      (PyCFunction)Map_GetZone,       1   },
            {   "SetZone",      (PyCFunction)Map_SetZone,       1   },
            {   "GetParallax",  (PyCFunction)Map_GetParallax,   1   },
            {   "SetParallax",  (PyCFunction)Map_SetParallax,   1   },
            {   0   }   // end of list
        };

#define GET(x) PyObject* get ## x(PyObject* self)
#define SET(x) PyObject* set ## x(PyObject* self, PyObject* value)

        GET(XWin)       { return PyInt_FromLong(engine->GetCamera().x); }
        SET(XWin)       { engine->SetCamera(Point(PyInt_AsLong(value), engine->GetCamera().y)); return 0; }
        GET(YWin)       { return PyInt_FromLong(engine->GetCamera().y); }
        SET(YWin)       { engine->SetCamera(Point(engine->GetCamera().x, PyInt_AsLong(value)));  return 0;}
        GET(NumTiles)   { return PyInt_FromLong(engine->tiles->NumTiles()); }
        GET(TileWidth)  { return PyInt_FromLong(engine->tiles->Width()); }
        GET(TileHeight) { return PyInt_FromLong(engine->tiles->Height()); }
        GET(Width)      { return PyInt_FromLong(engine->map.Width()); }
        GET(Height)     { return PyInt_FromLong(engine->map.Height()); }
        GET(RString)    { return PyString_FromString(engine->map.GetRString().c_str()); }
        SET(RString)    { engine->map.SetRString(PyString_AsString(value));     return 0;   }
        GET(VSPName)    { return PyString_FromString(engine->map.GetVSPName().c_str()); }
        SET(VSPName)    { engine->map.SetVSPName(PyString_AsString(value));     return 0;   }
        GET(Music)      { return PyString_FromString(engine->map.GetMusic().c_str()); }
        SET(Music)      { engine->map.SetMusic(PyString_AsString(value));       return 0;   }
        GET(Entities)   { Py_INCREF(entitydict); return entitydict; }

#undef GET
#undef SET

        PyGetSetDef properties[] =
        {
            {   "xwin",         (getter)getXWin,            (setter)setXWin,    "Gets or sets the X coordinate of the camera"   },
            {   "ywin",         (getter)getYWin,            (setter)setYWin,    "Gets or sets the Y coordinate of the camera"   },
            {   "numtiles",     (getter)getNumTiles,        0,                  "Gets the number of tiles in the current tileset"   },
            {   "tilewidth",    (getter)getTileWidth,       0,                  "Gets the width of the current tileset"         },
            {   "tileheight",   (getter)getTileHeight,      0,                  "Gets the height of the current tileset"        },
            {   "width",        (getter)getWidth,           0,                  "Gets the width of the current map, in tiles"   },
            {   "height",       (getter)getHeight,          0,                  "Gets the height of the current map, in tiles"  },
            {   "rstring",      (getter)getRString,         (setter)setRString, "Gets or sets the current render string of the map" },
            {   "vspname",      (getter)getVSPName,         (setter)setVSPName, "Gets or sets the name of the current tileset"  },
            {   "defaultmusic", (getter)getMusic,           (setter)setMusic,   "Gets or sets the default music of the current map" },
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
            PyObject* map=PyObject_New(PyObject,&type);

            if (!map)
                return NULL;

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

            if (!PyArg_ParseTuple(args,"s:MapSwitch",&filename))
                return NULL;

            if (!File::Exists(filename))
            {
                PyErr_SetString(PyExc_OSError,va("Unable to load %s",filename));
                return NULL;
            }

            engine->LoadMap(filename);

            Py_INCREF(Py_None);
            return Py_None;
        }

        METHOD(Map_Render)
        {
            char* rstring=NULL;

            if (!PyArg_ParseTuple(args,"|s:RenderMap",&rstring))
                return NULL;

            if (rstring)
                engine->Render(rstring);
            else
                engine->Render();

            Py_INCREF(Py_None);
            return Py_None;
        }

        METHOD(Map_GetTile)
        {
            int x,y,lay;

            if (!PyArg_ParseTuple(args,"iii:Map.GetTile",&x,&y,&lay))
                return NULL;

            return PyInt_FromLong(engine->map.GetTile(x,y,lay));
        }

        METHOD(Map_SetTile)
        {
            int x,y,lay,tile;

            if (!PyArg_ParseTuple(args,"iiii:Map.SetTile",&x,&y,&lay,&tile))
                return NULL;

            engine->map.SetTile(x,y,lay,tile);

            Py_INCREF(Py_None);
            return Py_None;
        }

        METHOD(Map_GetObs)
        {
            int x,y;

            if (!PyArg_ParseTuple(args,"ii:Map.GetObs",&x,&y))
                return NULL;

            return PyInt_FromLong(engine->map.IsObs(x,y));
        }

        METHOD(Map_SetObs)
        {
            int x,y,bSet;

            if (!PyArg_ParseTuple(args,"iii:Map.SetObs",&x,&y,&bSet))
                return NULL;

            engine->map.SetObs(x,y,bSet!=0);

            Py_INCREF(Py_None);
            return Py_None;
        }

        METHOD(Map_GetZone)
        {
            int x,y;

            if (!PyArg_ParseTuple(args,"ii:Map.GetZone",&x,&y))
                return NULL;

            return PyInt_FromLong(engine->map.GetZone(x,y));
        }

        METHOD(Map_SetZone)
        {
            int x,y,z;

            if (!PyArg_ParseTuple(args,"iii:Map.SetZone",&x,&y,&z))
                return NULL;

            engine->map.SetZone(x,y,z);

            Py_INCREF(Py_None);
            return Py_None;
        }

        // urk.. temporary code.  Please be temporary code.
        METHOD(Map_GetParallax)
        {
            int lay;

            if (!PyArg_ParseTuple(args,"i:Map.GetParallax",&lay))
                return NULL;

            SMapLayerInfo layerinfo;
            engine->map.GetLayerInfo(layerinfo,lay);

            return Py_BuildValue("((iiii)i)",
                layerinfo.pmulx,
                layerinfo.pdivx,
                layerinfo.pmuly,
                layerinfo.pdivy,
                layerinfo.nTransmode
                );
        }

        METHOD(Map_SetParallax)
        {
            return NULL;
        }

#undef METHOD
    }
}
