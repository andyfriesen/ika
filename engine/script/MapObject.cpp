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
            {   NULL,       NULL    }   // end of list
        };

        void Init()
        {
            memset(&type, 0, sizeof type);

            type.ob_refcnt=1;
            type.ob_type=&PyType_Type;
            type.tp_name="Map";
            type.tp_basicsize=sizeof type;
            type.tp_dealloc=(destructor)Destroy;
            type.tp_getattr=(getattrfunc)GetAttr;
            type.tp_setattr=(setattrfunc)SetAttr;
            type.tp_doc="Represents the currently executing map.";
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

        PyObject* GetAttr(PyObject* self,char* name)
        {
            if (!strcmp(name,"xwin"))
                return PyInt_FromLong(engine->GetCamera().x);
            if (!strcmp(name,"ywin"))
                return PyInt_FromLong(engine->GetCamera().y);
            if (!strcmp(name,"numtiles"))
                return PyInt_FromLong(engine->tiles->NumTiles());
            if (!strcmp(name,"tilewidth"))
                return PyInt_FromLong(engine->tiles->Width());
            if (!strcmp(name,"tileheight"))
                return PyInt_FromLong(engine->tiles->Height());
            if (!strcmp(name,"width"))
                return PyInt_FromLong(engine->map.Width());
            if (!strcmp(name,"height"))
                return PyInt_FromLong(engine->map.Height());
            if (!strcmp(name,"rstring"))
                return PyString_FromString(engine->map.GetRString().c_str());
            if (!strcmp(name,"vspname"))
                return PyString_FromString(engine->map.GetVSPName().c_str());
            if (!strcmp(name,"defaultmusic"))
                return PyString_FromString(engine->map.GetMusic().c_str());
            if (!strcmp(name,"entities"))
            {
                Py_INCREF(entitydict);
                return entitydict;
            }
            return Py_FindMethod(methods, self, name);
        }

        int SetAttr(PyObject* self,char* name,PyObject* value)
        {
            if (!strcmp(name,"xwin"))
            {
                int x = PyInt_AsLong(value);
                engine->SetCamera(Point(x, engine->GetCamera().y));
            }
            if (!strcmp(name,"ywin"))
            {
                int y = PyInt_AsLong(value);
                engine->SetCamera(Point(engine->GetCamera().x, y));
            }

            if (!strcmp(name,"numtiles"))
            {
                PyErr_SetString(PyExc_SyntaxError,"map.numtiles is read only");
                return -1;
            }
            if (!strcmp(name,"tilewidth"))
            {
                PyErr_SetString(PyExc_SyntaxError,"map.tilewidth is read only");
                return -1;
            }
            if (!strcmp(name,"tileheight"))
            {
                PyErr_SetString(PyExc_SyntaxError,"map.tileheight is read only");
                return -1;
            }
            if (!strcmp(name,"width"))
            {
                PyErr_SetString(PyExc_SyntaxError,"map.width is read only");
                return -1;
            }
            if (!strcmp(name,"height"))
            {
                PyErr_SetString(PyExc_SyntaxError,"map.height is read only");
                return -1;
            }
            if (!strcmp(name,"rstring"))
                engine->map.SetRString(PyString_AsString(value));
            if (!strcmp(name,"vspname"))
                engine->map.SetVSPName(PyString_AsString(value));
            if (!strcmp(name,"defaultmusic"))
                engine->map.SetMusic(PyString_AsString(value));
            if (!strcmp(name,"entities"))
            {
                PyErr_SetString(PyExc_SyntaxError,"map.entities is read only");
                return -1;
            }

            return 0;
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