
#include "ObjectDefs.h"
#include "common/Canvas.h"

namespace Script
{
    namespace Canvas
    {
        template <class T, T U, class V>
        PyObject* Get(T* self, PyObject* value);

        template <class T, T U, int>
        PyObject* Get(T* self, PyObject* value)
        {
            return PyInt_FromLong(self->U);
        }

        PyTypeObject type;

        PyMethodDef methods[] =
        {
            {   "GetPixel", (PyCFunction)Canvas_GetPixel,   1   },
            {   "SetPixel", (PyCFunction)Canvas_SetPixel,   1   },
            {   "Clear",    (PyCFunction)Canvas_Clear,      1   },
            {   "Resize",   (PyCFunction)Canvas_Resize,     1   },
            {   "Rotate",   (PyCFunction)Canvas_Rotate,     METH_NOARGS   },
            {   "Flip",     (PyCFunction)Canvas_Flip,       METH_NOARGS   },
            {   "Mirror",   (PyCFunction)Canvas_Mirror,     METH_NOARGS   },
            {   0,  0   }
        };

        PyObject* getWidth(CanvasObject* self)  { return PyInt_FromLong(self->canvas->Width());  }
        PyObject* getHeight(CanvasObject* self) { return PyInt_FromLong(self->canvas->Height()); }

        PyGetSetDef properties[] =
        {
            {   "width",    (getter)getWidth, 0, "Gets the width of the canvas" },
            {   "height",   (getter)getHeight,0, "Gets the height of the canvas" },
        };

        void Init()
        {
            memset(&type, 0, sizeof type);

            type.ob_refcnt = 1;
            type.ob_type = &PyType_Type;
            type.tp_name = "Canvas";
            type.tp_basicsize = sizeof type;
            type.tp_dealloc = (destructor)Destroy;
            type.tp_getset  = &properties[0];
            type.tp_methods = &methods[0];
            type.tp_doc = "A software representation of an image that can be manipulated easily.";
            //type.tp_new = New;
            PyType_Ready(&type);
        }
        
        PyObject* New(PyObject* self, PyObject* args)
        {
            PyObject* o;
            int y = -1;

            if (!PyArg_ParseTuple(args, "O|i:Canvas", &o, &y))
                return 0;

            if (o->ob_type == &PyInt_Type)
            {
                int x = PyInt_AsLong(o);
                if (x < 1 || y < 1)
                {
                    PyErr_SetString(PyExc_RuntimeError, va("Can't create %ix%i canvas.", x, y));
                    return 0;
                }

                CanvasObject* c = PyObject_New(CanvasObject, &type);
                c->canvas = new ::Canvas(x, y);
                c->ref = false;

                return (PyObject*)c;
            }
            else if (o->ob_type == &PyString_Type)
            {
                const char* fname = PyString_AsString(o);
                CanvasObject* c = PyObject_New(CanvasObject, &type);
                try
                {
                    c->canvas = new ::Canvas(fname);
                    c->ref = false;
                    return (PyObject*)c;
                }
                catch (std::runtime_error)
                {
                    Py_DECREF(c);
                    PyErr_SetString(PyExc_IOError, va("Couldn't open image file '%s'", fname));
                    return 0;
                }
            }
            else
            {
                PyErr_SetString(PyExc_ValueError, va("Canvas: Syntax is Canvas('filename') or Canvas(width, height)"));
                return 0;
            }
        }

        void Destroy(CanvasObject* self)
        {
            if (!self->ref)
                delete self->canvas;

            PyObject_Del(self);
        }

#define METHOD(x)  PyObject* x(CanvasObject* self, PyObject* args)
#define METHOD1(x) PyObject* x(CanvasObject* self)

        METHOD(Canvas_GetPixel)
        {
            int x, y;

            if (!PyArg_ParseTuple(args, "ii:GetPixel", &x, &y))
                return 0;

            return PyInt_FromLong(self->canvas->GetPixel(x, y));
        }

        METHOD(Canvas_SetPixel)
        {
            int x, y;
            u32 colour;

            if (!PyArg_ParseTuple(args, "iii:SetPixel", &x, &y, &colour))
                return 0;

            self->canvas->SetPixel(x, y, colour);

            Py_INCREF(Py_None);
            return Py_None;
        }

        METHOD(Canvas_Clear)
        {
            u32 colour;
            if (!PyArg_ParseTuple(args, "i:SetPixel", &colour))
                return 0;

            self->canvas->Clear(colour);

            Py_INCREF(Py_None);
            return Py_None;
        }

        METHOD(Canvas_Resize)
        {
            int x, y;

            if (!PyArg_ParseTuple(args, "ii:Resize", &x, &y))
                return 0;

            self->canvas->Resize(x, y);

            Py_INCREF(Py_None);
            return Py_None;
        }

        METHOD1(Canvas_Rotate)
        {
            self->canvas->Rotate();

            Py_INCREF(Py_None);
            return Py_None;
        }

        METHOD1(Canvas_Flip)
        {
            self->canvas->Flip();

            Py_INCREF(Py_None);
            return Py_None;
        }

        METHOD1(Canvas_Mirror)
        {
            self->canvas->Mirror();

            Py_INCREF(Py_None);
            return Py_None;
        }



    }
}