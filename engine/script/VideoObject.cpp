
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
            {   "Blit",         (PyCFunction)Video_Blit,        METH_VARARGS,
                "Video.Blit(image, x, y[, transparent])\n\n"
                "Draws the image at (x,y) at its original size.\n"
                "If transparent is sepcified and zero, alpha blending is disabled,\n"
                "else it is enabled."
            },

            {   "ScaleBlit",    (PyCFunction)Video_ScaleBlit,   METH_VARARGS,
                "Video.ScaleBlit(image, x, y, width, height[, transparent])\n\n"
                "Draws the image at (x,y), stretching it out to the size given.\n"
                "If transparent is sepcified and zero, alpha blending is disabled,\n"
                "else it is enabled."
            },
            
            {   "DistortBlit",  (PyCFunction)Video_DistortBlit, METH_VARARGS,
                "Video.DistortBlit(image, (upleftX, upleftY), (uprightX, uprightY), (downrightX, downrightY), (downleftX, downleftY)[,transparent])\n\n"
                "Draws the image onscreen, stretched to the four points specified.\n"
                "If transparent is sepcified and zero, alpha blending is disabled,\n"
                "else it is enabled."
            },

            {   "TileBlit",     (PyCFunction)Video_TileBlit,    METH_VARARGS,
                "Video.TileBlit(image, x, y, width, height[, scalex[, scaley[, transparent]]])\n\n"
                "Draws the image onscreen, \"tiling\" it as necessary to fit the rectangle specified.\n"
                "scalex and scaley are floating point values used as a scale factor.  The default is 1.\n"
                "If transparent is specified and zero, then alpha blending is disabled, else it is enabled."
            },

            // TODO: more blits.  I want a wrapblit, tintblit, and others
            {   "DrawPixel",    (PyCFunction)Video_DrawPixel,   METH_VARARGS,
                "Video.DrawPixel(x, y, colour)\n\n"
                "Draws a dot at (x,y) with the colour specified."
            },

            {   "DrawLine",     (PyCFunction)Video_DrawLine,    METH_VARARGS,
                "Video.DrawLine(x1, y1, x2, y2, colour)\n\n"
                "Draws a straight line from (x1, y1) to (x2, y2) in the colour specified."
            },

            {   "DrawRect",     (PyCFunction)Video_DrawRect,    METH_VARARGS,
                "Video.DrawRect(x1, y1, x2, y2, colour[, fill])\n\n"
                "Draws a rectangle with (x1, y1) and (x2, y2) as opposite corners.\n"
                "If fill is omitted or zero, an outline is drawn, else it is filled in."
            },

            {   "DrawEllipse",  (PyCFunction)Video_DrawEllipse, METH_VARARGS,
                "Video.DrawEllipse(cx, cy, rx, ry, colour[, filled])\n\n"
                "Draws an ellipse, centred at (cx, cy), of radius rx and ry on the X and\n"
                "Y axis, respectively.  If filled is omitted or nonzero, the ellipse is filled in\n"
                "else it is drawn as an outline."
            },
            
            {   "DrawTriangle", (PyCFunction)Video_DrawTriangle,METH_VARARGS,
                "Video.DrawTriangle((x, y, colour), (x, y, colour), (x, y, colour))\n\n"
                "Draws a triangle onscreen.  Each point is drawn in the colour specified."
            },

            {   "ShowPage",     (PyCFunction)Video_ShowPage, METH_NOARGS,
                "Video.ShowPage()\n\n"
                "Flips the back and front video buffers.  This must be called after the screen\n"
                "has been completely drawn, or the scene will never be presented to the player.\n"
                "This method is not guaranteed to preserve the contents of the screen, so it is\n"
                "advised to dedraw the entire screen, instead of incrementally drawing."
            },

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
#define METHOD1(x) PyObject* x(VideoObject* self)

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

            if (!PyArg_ParseTuple(args, "O(ii)(ii)(ii)(ii)|i:Video.DistortBlit", &image, x, y, x + 1, y + 1, x + 2, y + 2, x + 3, y + 3, &trans))
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

            if (!PyArg_ParseTuple(args, "Oiiii|ffi:Video.TileBlit", &image, &x, &y, &w, &h, &scalex, &scaley, &trans))
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

            if (!PyArg_ParseTuple(args, "(iii)(iii)(iii):Video.DrawTriangle", x, y, col, x + 1, y + 1, col + 1, x + 2, y + 2, col + 2))
                return 0;

            self->video->DrawTriangle(x, y, col);

            Py_INCREF(Py_None);
            return Py_None;
        }

        METHOD1(Video_ShowPage)
        {
            self->video->ShowPage();

            Py_INCREF(Py_None);
            return Py_None;
        }

        /*METHOD(std_palettemorph)
        {
            int r=255, g=255, b=255;

            if (!PyArg_ParseTuple(args, "|iii:PaletteMorph", &r, &g, &b))
                return NULL;

            Py_INCREF(Py_None);
            return Py_None;
        }*/

#undef METHOD
    }
}
