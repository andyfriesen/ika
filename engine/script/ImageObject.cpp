
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
            {   (char*)"Blit",         (PyCFunction)Image_Blit,         METH_VARARGS,
                (char*)"Image.Blit(x, y[, blendmode])\n\n"
                "Draws the image at (x, y).\n"
				"blendmode specifies the algorithm used to blend pixels.  It is one of\n"
				"the available blend modes defined in ika's constants section.\n"
                "blendmode defaults to ika.AlphaBlend."
            },

            {   (char*)"ClipBlit",        (PyCFunction)Image_ClipBlit,   METH_VARARGS,
                (char*)"ClipBlit(x, y, ix, iy, iw, ih[, blendmode])\n\n"
                "Draws a portion of the image defined by the coordinates (ix, iy, iw, ih)\n"
				"at screen coordinates (x, y).\n"
                "blendmode specifies the algorithm used to blend pixels.  It is one of\n"
				"the available blend modes defined in ika's constants section.\n"
                "blendmode defaults to ika.AlphaBlend."
            },

            {   (char*)"ScaleBlit",    (PyCFunction)Image_ScaleBlit,    METH_VARARGS,
                (char*)"Image.ScaleBlit(x, y, width, height[, blendmode])\n\n"
                "Blits the image, but stretches it out to the dimensions\n"
                "specified in (width, height)."
				"blendmode specifies the algorithm used to blend pixels.  It is one of\n"
				"the available blend modes defined in ika's constants section.\n"
                "blendmode defaults to ika.AlphaBlend."
            },

			{   (char*)"RotateBlit",  (PyCFunction)Image_RotateBlit,  METH_VARARGS,
                (char*)"RotateBlit(x, y, angle, [scalex[, scaley [, blendmode]]])\n\n"
                "Draws the image at (x, y), rotating to the angle given.\n"
                "scalex and scaley are floating point values used as a scale factor.  The default is 1.\n"
				"blendmode specifies the algorithm used to blend pixels.  It is one of\n"
				"the available blend modes defined in ika's constants section.\n"
                "blendmode defaults to ika.AlphaBlend."
            },

            {   (char*)"DistortBlit",  (PyCFunction)Image_DistortBlit,  METH_VARARGS,
                (char*)"Image.DistortBlit((x1, y1), (x2, y2), (x3, y3), (x4, y4)[, blendmode])\n\n"
                "Blits the image scaled to the four points specified."
				"blendmode specifies the algorithm used to blend pixels.  It is one of\n"
				"the available blend modes defined in ika's constants section.\n"
                "blendmode defaults to ika.AlphaBlend."
            },

			{   (char*)"TileBlit",  (PyCFunction)Image_TileBlit,  METH_VARARGS,
                (char*)"Image.TileBlit(x, y, width, height[, scalex[, scaley[, blendmode]]])\n\n"
                "Draws the image onscreen, \"tiling\" it as necessary to fit the rectangle specified.\n"
                "scalex and scaley are floating point values used as a scale factor.  The default is 1.\n"
				"blendmode specifies the algorithm used to blend pixels.  It is one of\n"
				"the available blend modes defined in ika's constants section.\n"
                "blendmode defaults to ika.AlphaBlend."
			},

            {   (char*)"TintBlit",  (PyCFunction)Image_TintBlit,  METH_VARARGS,
                (char*)"Image.TintBlit(x, y, tintColour[, blendMode])\n\n"
                "Draws the image onscreen, using tintColour to 'tint' the image.\n"
                "Each pixel is multiplied by tintColour.  The resultant values are then\n"
                "scaled before the pixel is plotted.\n\n"

                "In English, this means that RGBA(255, 255, 255, 255) is a normal blit,\n"
                "while RGBA(0, 0, 0, 255) will leave the alpha channel intact, but reduce\n"
                "all pixels to black. (effectively drawing a silhouette)\n\n"

                "blendMode is handled the same way as all the other blits.\n\n"

                "Lots of effects could be created by using this creatively.  Experiment!"
            },

            {   (char*)"TintDistortBlit",  (PyCFunction)Image_TintDistortBlit, METH_VARARGS,
                (char*)"Image.TintDistortBlit((upleftX, upleftY, upleftTint), (uprightX, uprightY, uprightTint), (downrightX, downrightY, downrightTint), (downleftX, downleftY, downrightTint)[, blendmode])\n\n"
                "Combines the effects of DistortBlit and TintBlit.  Each corner can be tinted individually,\n"
                "using the same algorithm as TintBlit.  The corners, if not the same, are smoothly interpolated\n"
                "across the image."
            },

            {   (char*)"TintTileBlit",     (PyCFunction)Image_TintTileBlit, METH_VARARGS,
                (char*)"Image.TintTileBlit(x, y, width, height, tintColour, scalex=1, scaley=1, blendmode=Normal)\n\n"
                "\"tile\"-blits the image, just like Video.TileBlit, except it multiplies each pixel by\n"
                "tintColour, resulting in a colour tint."
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
            {   (char*)"width",        (getter)getWidth,       0,  (char*)"Gets the width of the image."  },
            {   (char*)"height",       (getter)getHeight,      0,  (char*)"Gets the height of the image." },
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
			static char* keywords[] = { (char*)"src", 0 };
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
            int blendMode = ::Video::Normal;

            if (!PyArg_ParseTuple(args, "ii|i:Image.Blit", &x, &y, &blendMode))
                return 0;

            engine->video->SetBlendMode((::Video::BlendMode)blendMode);
            engine->video->BlitImage(self->img, x, y);
            
            Py_INCREF(Py_None);
            return Py_None;
        }

        PyObject* Image_ClipBlit(ImageObject* self, PyObject* args) {
            int x, y, ix, iy, iw, ih;
            int blendMode = ::Video::Normal;

            if (!PyArg_ParseTuple(args, "iiiiii|i:Image.ClipBlit", &x, &y, &ix, &iy, &iw, &ih, &blendMode)) {
                return 0;
            }

            engine->video->SetBlendMode((::Video::BlendMode)blendMode);
            engine->video->ClipBlitImage(self->img, x, y, ix, iy, iw, ih);
            
            Py_INCREF(Py_None);
            return Py_None;
        }

        PyObject* Image_ScaleBlit(ImageObject* self, PyObject* args) {
            int x, y, w, h;
            int blendMode = ::Video::Normal;

            if (!PyArg_ParseTuple(args, "iiii|i:Image.ScaleBlit", &x, &y, &w, &h, &blendMode))
                return 0;

            engine->video->SetBlendMode((::Video::BlendMode)blendMode);
            engine->video->ScaleBlitImage(self->img, x, y, w, h);

            Py_INCREF(Py_None);
            return Py_None;
        }

        PyObject* Image_RotateBlit(ImageObject* self, PyObject* args) {
            int x, y;
			float angle;
            float scalex = 1;
			float scaley = 1;
            int blendMode = ::Video::Normal;

            if (!PyArg_ParseTuple(args, "iif|ffi:Image.RotateBlit", &x, &y, &angle, &scalex, &scaley, &blendMode)) {
                return 0;
            }

            engine->video->SetBlendMode((::Video::BlendMode)blendMode);
            engine->video->RotateBlitImage(self->img, x, y, angle, scalex, scaley);
            
            Py_INCREF(Py_None);
            return Py_None;
        };

        PyObject* Image_DistortBlit(ImageObject* self, PyObject* args) {
            int x[4], y[4];
            int blendMode = ::Video::Normal;

            if (!PyArg_ParseTuple(args, "(ii)(ii)(ii)(ii)|i:Image.DistortBlit", x, y, x + 1, y + 1, x + 2, y + 2, x + 3, y + 3, &blendMode))
                return 0;

            engine->video->SetBlendMode((::Video::BlendMode)blendMode);
            engine->video->DistortBlitImage(self->img, x, y);

            Py_INCREF(Py_None);
            return Py_None;
        };

        PyObject* Image_TileBlit(ImageObject* self, PyObject* args) {
			int x, y;
            int w, h;
            float scalex = 1, scaley = 1;
            int blendMode = ::Video::Normal;

            if (!PyArg_ParseTuple(args, "iiii|ffi:Image.TileBlit", &x, &y, &w, &h, &scalex, &scaley, &blendMode)) {
                return 0;
            }

            engine->video->SetBlendMode((::Video::BlendMode)blendMode);
            engine->video->TileBlitImage(self->img, x, y, w, h, scalex, scaley);
            
            Py_INCREF(Py_None);
            return Py_None;
        };

        PyObject* Image_TintBlit(ImageObject* self, PyObject* args) {
            int x, y;
            u32 tint;
            int blendMode = ::Video::Normal;

            if (!PyArg_ParseTuple(args, "iii|i:Image.TintBlit", &x, &y, &tint, &blendMode)) {
                return 0;
            }

            engine->video->SetBlendMode((::Video::BlendMode)blendMode);
            engine->video->TintBlitImage(self->img, x, y, tint);

            Py_INCREF(Py_None);
            return Py_None;
        };

        PyObject* Image_TintDistortBlit(ImageObject* self, PyObject* args) {
            int x[4];
            int y[4];
            u32 tint[4];
            int blendMode = ::Video::Normal;

            if (!PyArg_ParseTuple(args, "(iii)(iii)(iii)(iii)|i:Image.TintDistortBlit", 
                x, y, tint, 
                x + 1, y + 1, tint + 1,
                x + 2, y + 2, tint + 2,
                x + 3, y + 3, tint + 3,
                &blendMode)
            ) {
                return 0;
            }

            engine->video->SetBlendMode((::Video::BlendMode)blendMode);
            engine->video->TintDistortBlitImage(self->img, x, y, tint);

            Py_INCREF(Py_None);
            return Py_None;
        }

		PyObject* Image_TintTileBlit(ImageObject* self, PyObject* args) {
            int x, y;
            int w, h;
            u32 colour;
            float scalex = 1, scaley = 1;
            int blendMode = ::Video::Normal;

            if (!PyArg_ParseTuple(args, "iiiii|ffi:Image.TintTileBlit", 
                &x, &y, &w, &h, &colour, &scalex, &scaley, &blendMode)
            ) {
                return 0;
            }

            engine->video->SetBlendMode((::Video::BlendMode)blendMode);
            engine->video->TintTileBlitImage(self->img, x, y, w, h, scalex, scaley, colour);
            
            Py_INCREF(Py_None);
            return Py_None;
        }

        PyObject* Image_Clip(ImageObject* /*self*/, PyObject* /*args*/) {
            Py_INCREF(Py_None);
            return Py_None;
        }
    }
}
