
#include "ObjectDefs.h"
#include "video/Driver.h"
#include "video/Image.h"
#include "common/log.h"
#include "main.h"

namespace Script
{
    namespace Image
    {
        PyTypeObject type;

        PyMethodDef methods[] =
        {
            {   "Blit",         (PyCFunction)Image_Blit,         1    },
            {   "ScaleBlit",    (PyCFunction)Image_ScaleBlit,    1    },
            {   "DistortBlit",  (PyCFunction)Image_DistortBlit,  1    },
            {   "Clip",         (PyCFunction)Image_Clip,         1    },
            {   NULL,    NULL }
        };

#define GET(x) PyObject* get ## x(ImageObject* self)
#define SET(x) PyObject* set ## x(ImageObject* self, PyObject* value)
        GET(Width) { return PyInt_FromLong(self->img->Width()); }
        GET(Height) { return PyInt_FromLong(self->img->Height()); }
#undef GET
#undef SET

        PyGetSetDef properties[] =
        {
            {   "width",    (getter)getWidth,   0,  "Gets the width of the image."  },
            {   "height",   (getter)getHeight,  0,  "Gets the height of the image." },
        };

        void Init()
        {
            memset(&type, 0, sizeof type);

            type.ob_refcnt=1;
            type.ob_type=&PyType_Type;
            type.tp_name="Image";
            type.tp_basicsize=sizeof type;
            type.tp_dealloc=(destructor)Destroy;
            type.tp_methods = methods;
            type.tp_getset = properties;
            type.tp_doc="A hardware-dependant image.";
        }

        void Destroy(ImageObject* self)
        {
            delete ((ImageObject*)self)->img;
            PyObject_Del(self);
        }

        PyObject* New(PyObject* self,PyObject* args)
        {
            char* filename;

            if (!PyArg_ParseTuple(args,"s:newimage",&filename))
                return NULL;

            try
            {
                ::Canvas img(filename);

                ImageObject* image=PyObject_New(ImageObject,&type);
                if (!image)
                    return 0;

                image->img = engine->video->CreateImage(img);

                return (PyObject*)image;
            }
            catch (std::runtime_error e)
            {
                Log::Write(e.what());
                return 0;
            }
        }

        PyObject* Image_Blit(ImageObject* self, PyObject* args)
        {
            int x,y;
            int trans = 1;

            if (!PyArg_ParseTuple(args,"ii|i:Image.Blit",&x,&y, &trans))
                return NULL;

            engine->video->BlitImage(
                self->img,
                x,y, trans != 0);

            Py_INCREF(Py_None);
            return Py_None;
        }

        PyObject* Image_ScaleBlit(ImageObject* self,PyObject* args)
        {
            Py_INCREF(Py_None);
            return Py_None;
        }

        PyObject* Image_DistortBlit(ImageObject* self,PyObject* args)
        {
            Py_INCREF(Py_None);
            return Py_None;
        };

        PyObject* Image_Clip(ImageObject* self,PyObject* args)
        {
            Py_INCREF(Py_None);
            return Py_None;
        }

    }
}