
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
            {   "Save",     (PyCFunction)Canvas_Save,       METH_VARARGS  },
            {   "Blit",     (PyCFunction)Canvas_Blit,       METH_VARARGS  },
            {   "ScaleBlit",(PyCFunction)Canvas_ScaleBlit,  METH_VARARGS  },
            {   "GetPixel", (PyCFunction)Canvas_GetPixel,   METH_VARARGS  },
            {   "SetPixel", (PyCFunction)Canvas_SetPixel,   METH_VARARGS  },
            {   "Clear",    (PyCFunction)Canvas_Clear,      METH_VARARGS  },
            {   "Resize",   (PyCFunction)Canvas_Resize,     METH_VARARGS  },
            {   "Rotate",   (PyCFunction)Canvas_Rotate,     METH_NOARGS   },
            {   "Flip",     (PyCFunction)Canvas_Flip,       METH_NOARGS   },
            {   "Mirror",   (PyCFunction)Canvas_Mirror,     METH_NOARGS   },
            {   0   }
        };

        PyObject* getWidth(CanvasObject* self)  { return PyInt_FromLong(self->canvas->Width());  }
        PyObject* getHeight(CanvasObject* self) { return PyInt_FromLong(self->canvas->Height()); }

        PyGetSetDef properties[] =
        {
            {   "width",    (getter)getWidth, 0, "Gets the width of the canvas" },
            {   "height",   (getter)getHeight,0, "Gets the height of the canvas" },
            {   0  },
        };

        void Init()
        {
            memset(&type, 0, sizeof type);

            type.ob_refcnt = 1;
            type.ob_type = &PyType_Type;
            type.tp_name = "Canvas";
            type.tp_basicsize = sizeof type;
            type.tp_dealloc = (destructor)Destroy;
            type.tp_methods = methods;
            type.tp_getset  = properties;
            type.tp_doc = "A software representation of an image that can be manipulated easily.";
    
            PyType_Ready(&type);
        }
        
        PyObject* New(PyObject* self, PyObject* args)
        {
            PyObject* o;
            int x, y;

            if (!PyArg_ParseTuple(args, "O|i:Canvas", &o, &y))
                return 0;

            if (o->ob_type == &PyInt_Type)
            {
                x = PyInt_AsLong(o);
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
                char* fname = PyString_AsString(o);
                CanvasObject* c = PyObject_New(CanvasObject, &type);
                try
                {
                    c->canvas = new ::Canvas(fname);
                    c->ref = false;
                    return (PyObject*)c;
                }
                catch (std::runtime_error)
                {
                    PyErr_SetString(PyExc_IOError, va("Couldn't open image file '%s'", fname));
                    return 0;
                }
            }
            else
                return 0;
        }

        void Destroy(CanvasObject* self)
        {
            if (!self->ref)
                delete self->canvas;

            PyObject_Del(self);
        }

#define METHOD(x)  PyObject* x(CanvasObject* self, PyObject* args)
#define METHOD1(x) PyObject* x(CanvasObject* self)

        METHOD(Canvas_Save)
        {
            char* fname;
            if (!PyArg_ParseTuple(args, "s:Save", &fname))
                return 0;

            self->canvas->Save(fname);

            Py_INCREF(Py_None);
            return Py_None;
        }

        METHOD(Canvas_Blit)
        {
            CanvasObject* dest;
            int x, y;
            int mode = 0;

            if (!PyArg_ParseTuple(args, "O!ii|i:Blit", &type, &dest, &x, &y, &mode))
                return 0;

            switch (mode)
            {
            case 0: CBlitter<Opaque>::Blit(*self->canvas,*dest->canvas, x, y);  break;
            case 1: CBlitter<Matte> ::Blit(*self->canvas,*dest->canvas, x, y);  break;
            case 2: CBlitter<Alpha> ::Blit(*self->canvas,*dest->canvas, x, y);  break;
            case 3:
                PyErr_SetString(PyExc_RuntimeError, va("%i is not a valid blending mode.", mode));
                return 0;
            }

            Py_INCREF(Py_None);
            return Py_None;
        }

        METHOD(Canvas_ScaleBlit)
        {
            CanvasObject* dest;
            int x, y;
            int w, h;
            int mode = 0;

            if (!PyArg_ParseTuple(args, "O!iiii|i:ScaleBlit", &type, &dest, &x, &y, &w, &h, &mode))
                return 0;

            switch (mode)
            {
            case 0: CBlitter<Opaque>::ScaleBlit(*self->canvas,*dest->canvas, x, y, w, h);  break;
            case 1: CBlitter<Matte> ::ScaleBlit(*self->canvas,*dest->canvas, x, y, w, h);  break;
            case 2: CBlitter<Alpha> ::ScaleBlit(*self->canvas,*dest->canvas, x, y, w, h);  break;
            case 3:
                PyErr_SetString(PyExc_RuntimeError, va("%i is not a valid blending mode.", mode));
                return 0;
            }

            Py_INCREF(Py_None);
            return Py_None;
        }

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
