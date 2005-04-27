
#include "ObjectDefs.h"
#include "video/Driver.h"
#include "video/Image.h"
#include "common/log.h"
#include "main.h"

#include <stdexcept>

namespace Script {
    namespace Image {
        PyTypeObject type;

        PyMethodDef methods[] = {
            {   "Blit",         (PyCFunction)Image_Blit,         METH_VARARGS,
                "Image.Blit(x, y[, transparent])\n\n"
                "Draws the image at (x, y).\n"
                "If transparent is specified and 0 the image is opaque.\n"
                "Otherwise, alpha blending is enabled."

            },

            {   "ScaleBlit",    (PyCFunction)Image_ScaleBlit,    METH_VARARGS,
                "Image.ScaleBlit(x, y, width, height[, transparent])\n\n"
                "Blits the image, but stretches it out to the dimensions\n"
                "specified in (width, height)."
            },

            {   "DistortBlit",  (PyCFunction)Image_DistortBlit,  METH_VARARGS,
                "Image.DistortBlit((x1, y1), (x2, y2), (x3, y3), (x4, y4)[, transparency])\n\n"
                "Blits the image scaled to the four points specified."
            },

            /*{   "Clip",         (PyCFunction)Image_Clip,         METH_VARARGS,
                "Image.Clip(x, y, x2, y2)\n\n"
                "Sets the dimensions of the image's clipping rectangle.\n"
                "When images are drawn they will only be drawn in this area."
            }, */ // NYI
            
            {   NULL,    NULL }
        };

#define GET(x) PyObject* get ## x(ImageObject* self)
#define SET(x) PyObject* set ## x(ImageObject* self, PyObject* value)
        GET(Width) { return PyInt_FromLong(self->img->Width()); }
        GET(Height) { return PyInt_FromLong(self->img->Height()); }

#undef GET
#undef SET

        PyGetSetDef properties[] = {
            {   "width",    (getter)getWidth,   0,  "Gets the width of the image."  },
            {   "height",   (getter)getHeight,  0,  "Gets the height of the image." },
            {   0   }
        };

        void Init() {
            memset(&type, 0, sizeof type);

            type.ob_refcnt = 1;
            type.ob_type = &PyType_Type;
            type.tp_name = "Image";
            type.tp_basicsize = sizeof type;
            type.tp_dealloc = (destructor)Destroy;
            type.tp_methods = methods;
            type.tp_getset = properties;
            type.tp_doc = "A hardware-dependant image.";
            type.tp_new = New;
            PyType_Ready(&type);
        }

        void Destroy(ImageObject* self) {
            //delete self->img;
            engine->video->FreeImage(self->img);
            PyObject_Del(self);
        }

        PyObject* New(::Video::Image* image) {
            ImageObject* img = PyObject_New(ImageObject, &type);
            img->img = image;
            return (PyObject*)img;
        }

        PyObject* New(PyTypeObject* type, PyObject* args, PyObject* kw) {
            static char* keywords[] = { "src", 0 };
            PyObject* obj;

            if (!PyArg_ParseTupleAndKeywords(args, kw, "O:__new__", keywords, &obj))
                return NULL;

            if (obj->ob_type == &PyString_Type) {
                try {
                    const char* filename = PyString_AsString(obj);

                    ::Canvas img(filename);

                    ImageObject* image = PyObject_New(ImageObject, type);
                    if (!image) {
                        PyErr_SetString(PyExc_MemoryError, "newimage: This should never happen. :o");
                        return 0;
                    }

                    image->img = engine->video->CreateImage(img);

                    return (PyObject*)image;
                }
                catch (std::runtime_error e) {
                    PyErr_SetString(PyExc_RuntimeError, e.what());
                    return 0;
                }
            }
            else if (obj->ob_type == &Script::Canvas::type) {
                ImageObject* image = PyObject_New(ImageObject, type);
                if (!image)
                    return 0;

                image->img = engine->video->CreateImage(*((Script::Canvas::CanvasObject*)obj)->canvas);

                return (PyObject*)image;
            }
            else {
                PyErr_SetString(PyExc_TypeError, "Image constructor accepts a string (filename) or a Canvas object.");
                return 0;
            }
        }

        PyObject* Image_Blit(ImageObject* self, PyObject* args) {
            int x, y;
            int trans = ::Video::Normal;

            if (!PyArg_ParseTuple(args, "ii|i:Image.Blit", &x, &y, &trans))
                return 0;

            engine->video->SetBlendMode((::Video::BlendMode)trans);
            engine->video->BlitImage(self->img, x, y);
            
            Py_INCREF(Py_None);
            return Py_None;
        }

        PyObject* Image_ScaleBlit(ImageObject* self, PyObject* args) {
            int x, y, w, h;
            int trans = ::Video::Normal;

            if (!PyArg_ParseTuple(args, "iiii|i:Image.ScaleBlit", &x, &y, &w, &h, &trans))
                return 0;

            engine->video->SetBlendMode((::Video::BlendMode)trans);
            engine->video->ScaleBlitImage(self->img, x, y, w, h);

            Py_INCREF(Py_None);
            return Py_None;
        }

        PyObject* Image_DistortBlit(ImageObject* self, PyObject* args) {
            int x[4], y[4];
            int trans = ::Video::Normal;

            if (!PyArg_ParseTuple(args, "(ii)(ii)(ii)(ii)|i:Video.DistortBlit", x, y, x + 1, y + 1, x + 2, y + 2, x + 3, y + 3, &trans))
                return 0;

            engine->video->SetBlendMode((::Video::BlendMode)trans);
            engine->video->DistortBlitImage(self->img, x, y);

            Py_INCREF(Py_None);
            return Py_None;
        };

        PyObject* Image_Clip(ImageObject* self, PyObject* args) {
           
            Py_INCREF(Py_None);
            return Py_None;
        }

    }
}
