
#include "ObjectDefs.h"
#include "video/Driver.h"
#include "video/Image.h"

namespace Script
{
    namespace Video
    {
        PyTypeObject type;
        
        PyMethodDef methods[] =
        {
            {   "Blit",         (PyCFunction)Video_Blit,        1   },
            {   "ScaleBlit",    (PyCFunction)Video_ScaleBlit,   1   },
            {   "DistortBlit",  (PyCFunction)Video_DistortBlit, 1   },
            {   "TileBlit",     (PyCFunction)Video_TileBlit,    1   },
            // TODO: more blits.  I want a wrapblit, tintblit, and others
            {   "DrawPixel",    (PyCFunction)Video_DrawPixel,   1   },
            {   "DrawLine",     (PyCFunction)Video_DrawLine,    1   },
            {   "DrawRect",     (PyCFunction)Video_DrawRect,    1   },
            {   "DrawEllipse",  (PyCFunction)Video_DrawEllipse, 1   },
            {   "DrawTriangle", (PyCFunction)Video_DrawTriangle,1   },
            {   0   }
        };

#define GET(x) PyObject* get ## x(VideoObject* self)
        GET(XRes) { return PyInt_FromLong(self->video->GetResolution().x);  }
        GET(YRes) { return PyInt_FromLong(self->video->GetResolution().y);  }
#undef GET

        PyGetSetDef properties[] =
        {
            {   "xres",     (getter)getXRes,    0,  "Gets the horizontal resolution of the current display mode, in pixels."    },
            {   "yres",     (getter)getYRes,    0,  "Gets the vertical resolution of the current display mode, in pixels."      },
            {   0   }
        };

        void Init()
        {
            memset(&type, 0, sizeof type);

            type.ob_refcnt = 1;
            type.ob_type = &PyType_Type;
            type.tp_name = "Video";
            type.tp_basicsize = sizeof type;
            type.tp_methods = methods;
            type.tp_getset = properties;
            type.tp_dealloc = (destructor)Destroy;
            type.tp_doc = "Interface for ika's graphics engine.";

            PyType_Ready(&type);
        }

        PyObject* New(::Video::Driver* v)
        {
            VideoObject* video = PyObject_New(VideoObject, &type);
            video->video = v;

            return (PyObject*)video;
        }

        void Destroy(VideoObject* self)
        {
            PyObject_Del(self);
        }

#define METHOD(x) PyObject* x(VideoObject* self,PyObject* args)

        METHOD(Video_Blit)
        {
            Script::Image::ImageObject* image;
            int x, y;
            int trans = 1;

            if (!PyArg_ParseTuple(args, "Oii|i:Video.Blit", &image, &x, &y, &trans))
                return 0;

            self->video->SetBlendMode(trans ? ::Video::Normal : ::Video::None);
            self->video->BlitImage(image->img, x, y);
            
            Py_INCREF(Py_None);
            return Py_None;
        }

        METHOD(Video_ScaleBlit)
        {
            Script::Image::ImageObject* image;
            int x, y;
            int w, h;
            int trans = 1;

            if (!PyArg_ParseTuple(args, "Oiiii|i:Video.ScaleBlit", &image, &x, &y, &w, &h, &trans))
                return 0;

            self->video->SetBlendMode(trans ? ::Video::Normal : ::Video::None);
            self->video->ScaleBlitImage(image->img, x, y, w, h);
            
            Py_INCREF(Py_None);
            return Py_None;
        }

        METHOD(Video_DistortBlit)
        {
            Script::Image::ImageObject* image;
            int x[4], y[4];
            int trans = 1;

            if (!PyArg_ParseTuple(args, "O(iiii)(iiii)|i:Video.DistortBlit", &image, x, x + 1, x + 2, x + 3, y, y + 1, y + 2, y + 3, &trans))
                return 0;

            self->video->SetBlendMode(trans ? ::Video::Normal : ::Video::None);
            self->video->DistortBlitImage(image->img, x, y);
            
            Py_INCREF(Py_None);
            return Py_None;
        }

        METHOD(Video_TileBlit)
        {
            Script::Image::ImageObject* image;
            int x, y;
            int w, h;
            float scalex = 1, scaley = 1;
            int trans = 1;

            if (!PyArg_ParseTuple(args, "Oiiii|ffi:Video.DistortBlit", &image, &x, &y, &w, &h, &scalex, &scaley, &trans))
                return 0;

            self->video->SetBlendMode(trans ? ::Video::Normal : ::Video::None);
            self->video->TileBlitImage(image->img, x, y, w, h, scalex, scaley);
            
            Py_INCREF(Py_None);
            return Py_None;
        }

        METHOD(Video_DrawPixel)
        {
            int x, y;
            u32 colour;

            if (!PyArg_ParseTuple(args, "iii:Video.DrawPixel", &x, &y, &colour))
                return 0;

            self->video->DrawPixel(x, y, colour);

            Py_INCREF(Py_None);
            return Py_None;
        }

        METHOD(Video_DrawLine)
        {
            int x1, y1, x2, y2;
            u32 colour;

            if (!PyArg_ParseTuple(args, "iiiii:Video.DrawLine", &x1, &y1, &x2, &y2, &colour))
                return 0;

            self->video->DrawLine(x1, y1, x2, y2, colour);

            Py_INCREF(Py_None);
            return Py_None;
        }

        METHOD(Video_DrawRect)
        {
            int x1, y1, x2, y2;
            u32 colour;
            int filled = 0;

            if (!PyArg_ParseTuple(args, "iiiii|i:Video.DrawRect", &x1, &y1, &x2, &y2, &colour, &filled))
                return 0;

            self->video->DrawRect(x1, y1, x2, y2, colour, filled != 0);

            Py_INCREF(Py_None);
            return Py_None;
        }

        METHOD(Video_DrawEllipse)
        {
            int cx, cy;
            int rx, ry;
            u32 colour;
            int filled = 0;

            if (!PyArg_ParseTuple(args, "iiiii|i:Video.DrawEllipse", &cx, &cy, &rx, &ry, &colour, &filled))
                return 0;

            self->video->DrawEllipse(cx, cy, rx, ry, colour, filled != 0);

            Py_INCREF(Py_None);
            return Py_None;
        }

        METHOD(Video_DrawTriangle)
        {
            int x[3];
            int y[3];
            u32 col[3];

            if (!PyArg_ParseTuple(args, "(iii)(iii)(iii):Video.DrawTriangle", x, x + 1, x + 2, y, y + 1, y + 2, col, col + 1, col + 2))
                return 0;

            self->video->DrawTriangle(x, y, col);

            Py_INCREF(Py_None);
            return Py_None;
        }

#undef METHOD
    }
}