
#include "../wxinc.h"

#include "ObjectDefs.h"

#include "../mainwindow.h"
#include "../mapview.h"
#include "../tilesetview.h"

#include "common/map.h"

namespace ScriptObject
{
    namespace Editor
    {
        PyTypeObject type;

        PyMethodDef methods[] =
        {
            {   "MapToTile",    (PyCFunction)Editor_MapToTile,  METH_VARARGS,
                "MapToTile(x, y, layer=-1) -> (x,y)\n\n"
                "Translates the point (x,y) from map pixel coordinates to tile\n"
                "coordinates on the layer specified.  If the layer is omitted,\n"
                "or is an invalid value, the current layer is used."
            },

            {   "TileToMap",    (PyCFunction)Editor_TileToMap,  METH_VARARGS,
                "TileToMap(x, y, layer=-1) -> (x,y)\n\n"
                "Translates the point (x,y) from tile coordinates on the layer\n"
                "specified to world pixel coordinates.  If the layer is omitted\n"
                "or is an invalid value, the current layer is used."
            },

            {   "GetMouseState",    (PyCFunction)Editor_GetMouseState,  METH_NOARGS,
                "GetMouseState() -> (x, y, left, middle, right)\n\n"
                "Returns a tuple containing the status of the mouse.\n"
                "x and y are in map pixel coordinates.  Left, middle, and right\n"
                "are boolean values representing the current state of each mouse\n"
                "button."
            },
        };

#define GET(x) PyObject* get ## x(EditorObject* self)
#define SET(x) PyObject* set ## x(EditorObject* self, PyObject* value)

        GET(XWin)           {   return PyInt_FromLong(self->mainWnd->GetMapView()->GetXWin());  }
        //SET(XWin)
        GET(YWin)           {   return PyInt_FromLong(self->mainWnd->GetMapView()->GetYWin());  }
        //SET(YWin)
        GET(CurLayer)   {   return PyInt_FromLong(self->mainWnd->GetCurrentLayer());          }
        SET(CurLayer)   {   self->mainWnd->SetCurrentLayer(PyInt_AsLong(value));  return 0;   }
        GET(CurTile)    {   return PyInt_FromLong(self->mainWnd->GetCurrentTile());       }
        SET(CurTile)
        {
            self->mainWnd->SetCurrentTile(PyInt_AsLong(value));
            return 0;
        }
        //GET(CurBrush)   {   Py_INCREF(self->brush); return self->brush;}
        //SET(CurBrush)   {   }

#undef GET
#undef SET

        PyGetSetDef properties[] =
        {
            {   "xwin",         (getter)getXWin,            0,                          "Gets the current x position of the map viewport."  },
            {   "ywin",         (getter)getYWin,            0,                          "Gets the current y position of the map viewport."  },
            {   "curlayer",     (getter)getCurLayer,        (setter)setCurLayer,        "Gets or sets the active map layer."                },
            {   "curtile",      (getter)getCurTile,         (setter)setCurTile,         "Gets or sets the active tile."                     },
            //{   "curbrush",      (getter)getCurBrush,         0,                        "Gets or sets the current brush."                    },
            {   0   }
        };

        void Init()
        {
            memset(&type, 0, sizeof type);

            type.ob_refcnt = 1;
            type.ob_type = &PyType_Type;
            type.tp_name = "Editor";
            type.tp_basicsize = sizeof type;
            type.tp_dealloc = (destructor)Destroy;
            type.tp_methods = methods;
            type.tp_getset  = properties;
            type.tp_doc = "Methods and properties pertaining to the ikaMap editor itself.";

            PyType_Ready(&type);
        }

        PyObject* New(MainWindow* mainWnd)
        {
            EditorObject* object = PyObject_New(EditorObject, &type);

            if (!object)    return 0;
            //object->brush = ScriptObject::Brush::New(mainWnd);
            object->mainWnd = mainWnd;

            return (PyObject*)object;
        }

        void Destroy(PyObject* self)
        {
            PyObject_Del(self);
        }

#define METHOD(x) PyObject* x(EditorObject* self, PyObject* args)
#define METHOD1(x) PyObject* x(EditorObject* self)

        METHOD(Editor_MapToTile)
        {
            int x, y;
            uint layer = -1;

            if (!PyArg_ParseTuple(args, "ii|i:MapToTile", &x, &y, &layer))
                return 0;

            if (layer >= self->mainWnd->GetMap()->NumLayers())
                layer = self->mainWnd->GetCurrentLayer();

            self->mainWnd->GetMapView()->MapToTile(x, y, layer);

            return Py_BuildValue("(ii)", x, y);
        }

        METHOD(Editor_TileToMap)
        {
            int x, y;
            uint layer = -1;

            if (!PyArg_ParseTuple(args, "ii|i:MapToTile", &x, &y, &layer))
                return 0;

            if (layer >= self->mainWnd->GetMap()->NumLayers())
                layer = self->mainWnd->GetCurrentLayer();

            self->mainWnd->GetMapView()->TileToMap(x, y, layer);

            return Py_BuildValue("(ii)", x, y);
        }

        METHOD1(Editor_GetMouseState)
        {
            wxPoint pos = ::wxGetMousePosition();

            return Py_BuildValue("(iiiii)", pos.x, pos.y, 0, 0, 0);
        }

#undef METHOD
#undef METHOD1
    }
}