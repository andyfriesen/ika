
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
            // TODO: more blits.  I want a wrapblit, tintblit, and others
            {   "DrawLine",     (PyCFunction)Video_DrawLine,    1   },
            {   "DrawRect",     (PyCFunction)Video_DrawRect,    1   },
            {   "DrawEllipse",  (PyCFunction)Video_DrawEllipse, 1   },
            {   "DrawPixel",    (PyCFunction)Video_DrawPixel,   1   },
        };

        void Init()
        {
            memset(&type, 0, sizeof type);

            type.ob_refcnt = 1;
            type.ob_type = &PyType_Type;
            type.tp_name = "Video";
            type.tp_basicsize = sizeof type;
            type.tp_getattr = (getattrfunc)GetAttr;
            type.tp_dealloc = (destructor)Destroy;
            type.tp_doc = "Interface for ika's graphics engine.";
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

        PyObject* GetAttr(VideoObject* self, char* name)
        {
            return Py_FindMethod(methods, (PyObject*)self, name);
        }

#define METHOD(x) PyObject* x(VideoObject* self,PyObject* args)

        METHOD(Video_Blit)
        {
            Script::Image::ImageObject* image;
            int x, y;
            int trans = 1;

            if (!PyArg_ParseTuple(args, "Oii|i:Video.Blit", &image, &x, &y, &trans))
                return 0;

            self->video->BlitImage(image->img, x, y, trans != 0);

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

            self->video->ScaleBlitImage(image->img, x, y, w, h, trans != 0);

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

            self->video->DistortBlitImage(image->img, x, y, trans != 0);

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

        METHOD(Video_DrawPixel)
        {
            int x, y;
            u32 colour;
            int filled = 0;

            if (!PyArg_ParseTuple(args, "iii:Video.DrawPixel", &x, &y, &colour))
                return 0;

            self->video->DrawPixel(x, y, colour);

            Py_INCREF(Py_None);
            return Py_None;
        }

#undef METHOD
    }
}