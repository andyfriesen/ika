#include "ObjectDefs.h"
#include "main.h"
#include "video/Driver.h"
#include "video/Image.h"

namespace Script {
    namespace Video {
        PyTypeObject type;
        
        PyMethodDef methods[] = {
            {   "Blit",         (PyCFunction)Video_Blit,        METH_VARARGS,
                "Blit(image, x, y[, blendmode])\n\n"
                "Draws the image at (x, y) at its original size.\n"
                "blendmode specifies the algorithm used to blend pixels.  It is one of\n"
                "ika.Opaque, ika.Matte, ika.AlphaBlend, ika.AddBlend, or ika.SubtractBlend.\n"
                "The default is ika.Alphablend."
            },
            
            {   "ClipBlit",         (PyCFunction)Video_ClipBlit,   METH_VARARGS,
                "ClipBlit(image, x, y, ix, iy, iw, ih[, blendmode])\n\n"
                "Draws a portion of the image defined by the coordinates (ix, iy, iw, ih)\n" "at screen coordinates (x, y).\n"
                "blendmode specifies the algorithm used to blend pixels.  It is one of\n"
                "ika.Opaque, ika.Matte, ika.AlphaBlend, ika.AddBlend, or ika.SubtractBlend.\n"
                "blendMode defaults to ika.AlphaBlend."
            },

            {   "ScaleBlit",    (PyCFunction)Video_ScaleBlit,   METH_VARARGS,
                "ScaleBlit(image, x, y, width, height[, blendmode])\n\n"
                "Draws the image at (x, y), stretching it out to the size given.\n"
                "blendmode specifies the algorithm used to blend pixels.  It is one of\n"
                "ika.Opaque, ika.Matte, ika.AlphaBlend, ika.AddBlend, or ika.SubtractBlend.\n"
                "The default is ika.Alphablend."
            },
            
            {   "DistortBlit",  (PyCFunction)Video_DistortBlit, METH_VARARGS,
                "DistortBlit(image, (upleftX, upleftY), (uprightX, uprightY), (downrightX, downrightY), (downleftX, downleftY)[, blendmode])\n\n"
                "Draws the image onscreen, stretched to the four points specified.\n"
                "blendmode specifies the algorithm used to blend pixels.  It is one of\n"
                "ika.Opaque, ika.Matte, ika.AlphaBlend, ika.AddBlend, or ika.SubtractBlend.\n"
                "The default is ika.Alphablend."
            },

            {   "TileBlit",     (PyCFunction)Video_TileBlit,    METH_VARARGS,
                "TileBlit(image, x, y, width, height[, scalex[, scaley[, blendmode]]])\n\n"
                "Draws the image onscreen, \"tiling\" it as necessary to fit the rectangle specified.\n"
                "scalex and scaley are floating point values used as a scale factor.  The default is 1.\n"
                "blendmode specifies the algorithm used to blend pixels.  It is one of\n"
                "ika.Opaque, ika.Matte, ika.AlphaBlend, ika.AddBlend, or ika.SubtractBlend.\n"
                "The default is ika.Alphablend."
            },

            {   "TintBlit",     (PyCFunction)Video_TintBlit,    METH_VARARGS,
                "TintBlit(image, x, y, tintColour[, blendMode])\n\n"
                "Draws the image onscreen, using tintColour to 'tint' the image.\n"
                "Each pixel is multiplied by tintColour.  The resultant values are then\n"
                "scaled before the pixel is plotted.\n\n"

                "In English, this means that RGBA(255, 255, 255, 255) is a normal blit,\n"
                "while RGBA(0, 0, 0, 255) will leave the alpha channel intact, but reduce\n"
                "all pixels to black. (effectively drawing a silhouette)\n\n"

                "blendMode is handled the same way as all the other blits.\n\n"

                "Lots of effects could be created by using this creatively.  Experiment!"
            },

            {   "TintDistortBlit",  (PyCFunction)Video_TintDistortBlit, METH_VARARGS,
                "TintDistortBlit(image, (upleftX, upleftY, upleftTint), (uprightX, uprightY, uprightTint), (downrightX, downrightY, downrightTint), (downleftX, downleftY, downrightTint)[, blendmode])\n\n"
                "Combines the effects of DistortBlit and TintBlit.  Each corner can be tinted individually,\n"
                "using the same algorithm as TintBlit.  The corners, if not the same, are smoothly interpolated\n"
                "across the image."
            },

            {   "TintTileBlit",     (PyCFunction)Video_TintTileBlit, METH_VARARGS,
                "TintTileBlit(image, x, y, width, height, tintColour, scalex=1, scaley=1, blendmode=Normal)\n\n"
                "\"tile\"-blits the image, just like Video.TileBlit, except it multiplies each pixel by\n"
                "tintColour, resulting in a colour tint."
            },

            {   "DrawPixel",    (PyCFunction)Video_DrawPixel,   METH_VARARGS,
                "DrawPixel(x, y, colour[, blendmode])\n\n"
                "Draws a dot at (x, y) with the colour specified."
            },

            {   "DrawLine",     (PyCFunction)Video_DrawLine,    METH_VARARGS,
                "DrawLine(x1, y1, x2, y2, colour[, blendmode])\n\n"
                "Draws a straight line from (x1, y1) to (x2, y2) in the colour specified."
            },

            {   "DrawRect",     (PyCFunction)Video_DrawRect,    METH_VARARGS,
                "DrawRect(x1, y1, x2, y2, colour[, fill, blendmode])\n\n"
                "Draws a rectangle with (x1, y1) and (x2, y2) as opposite corners.\n"
                "If fill is omitted or zero, an outline is drawn, else it is filled in."
            },

            {   "DrawEllipse",  (PyCFunction)Video_DrawEllipse, METH_VARARGS,
                "DrawEllipse(cx, cy, rx, ry, colour[, filled, blendmode])\n\n"
                "Draws an ellipse, centred at (cx, cy), of radius rx and ry on the X and\n"
                "Y axis, respectively.  If filled is omitted or nonzero, the ellipse is filled in\n"
                "else it is drawn as an outline."
            },
            
            {   "DrawTriangle", (PyCFunction)Video_DrawTriangle, METH_VARARGS,
                "DrawTriangle((x, y, colour), (x, y, colour), (x, y, colour)[, blendmode])\n\n"
                "Draws a triangle onscreen.  Each point is drawn in the colour specified."
            },

            {   "ClipScreen",   (PyCFunction)Video_ClipScreen, METH_VARARGS,
                "ClipScreen(left=0, top=0, right=xres, bottom=yres)\n\n"
                "Clips the video display to the rectangle specfied.  All drawing\n"
                "operations will be confined to this region.\n\n"
                "Calling ClipScreen with no arguments will reset the clipping rect\n"
                "to its default setting. (the whole screen)"
            },

            {   "GetClipRect",  (PyCFunction)Video_GetClipRect, METH_NOARGS,
                "GetClipRect() -> (x, y, x2, y2)\n\n"
                "Returns the current clipping rectangle."
            },

            {   "GrabImage",    (PyCFunction)Video_GrabImage, METH_VARARGS,
                "GrabImage(x1, y1, x2, y2) -> image\n\n"
                "Grabs a rectangle from the screen, copies it to an image, and returns it."
            },

            {   "GrabCanvas",   (PyCFunction)Video_GrabCanvas, METH_VARARGS,
                "GrabCanvas(x1, y1, x2, y2) -> canvas\n\n"
                "Grabs a rectangle from the screen, copies it to a canvas, and returns it."
            },

            {   "ClearScreen",  (PyCFunction)Video_ClearScreen, METH_NOARGS,
                "ClearScreen()\n\n"
                "Clears the screen. (with blackness)"
            },

            {   "ShowPage",     (PyCFunction)Video_ShowPage, METH_NOARGS,
                "ShowPage()\n\n"
                "Flips the back and front video buffers.  This must be called after the screen\n"
                "has been completely drawn, or the scene will never be presented to the player.\n"
                "This method is not guaranteed to preserve the contents of the screen, so it is\n"
                "advised to redraw the entire screen, instead of incrementally drawing."
            },

#if 0
            /// Disabled due to limitations inherent in SDL. (switching video modes causes all textures to be erased)
            {   "SetResolution",   (PyCFunction)Video_SetResolution, METH_VARARGS,
                "SetResolution(xres, yres)\n\n"
                "Changes the current video mode to (xres, yres).  If the video mode cannot be set,\n"
                "ika will raise a RuntimeError exception."
            },
#endif

            {   0   }
        };

#define GET(x) PyObject* get ## x(VideoObject* self)
        GET(XRes) { return PyInt_FromLong(self->video->GetResolution().x);  }
        GET(YRes) { return PyInt_FromLong(self->video->GetResolution().y);  }
        GET(Colours) { return ::Script::Colours::New(self->video);  }
#undef GET

        PyGetSetDef properties[] = {
            {   "xres",     (getter)getXRes,    0,  "Gets the horizontal resolution of the current display mode, in pixels."    },
            {   "yres",     (getter)getYRes,    0,  "Gets the vertical resolution of the current display mode, in pixels."      },
            {   "colours",  (getter)getColours, 0,  "Gets a mapping containing the currently-defined colours by name."      },
            {   0   }
        };

        void Init() {
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

        PyObject* New(::Video::Driver* v) {
            VideoObject* video = PyObject_New(VideoObject, &type);
            video->video = v;

            return (PyObject*)video;
        }

        void Destroy(VideoObject* self) {
            PyObject_Del(self);
        }

#define METHOD(x) PyObject* x(VideoObject* self, PyObject* args)
#define METHOD1(x) PyObject* x(VideoObject* self)

        METHOD(Video_Blit) {
            Script::Image::ImageObject* image;
            int x, y;
            int trans = ::Video::Normal;

            if (!PyArg_ParseTuple(args, "O!ii|i:Video.Blit", &Script::Image::type, &image, &x, &y, &trans)) {
                return 0;
            }

            self->video->SetBlendMode((::Video::BlendMode)trans);
            self->video->BlitImage(image->img, x, y);
            
            Py_INCREF(Py_None);
            return Py_None;
        }

        METHOD(Video_ClipBlit) {
            Script::Image::ImageObject* image;
            int x, y, ix, iy, iw, ih;
            int trans = ::Video::Normal;

            if (!PyArg_ParseTuple(args, "O!iiiiii|i:Video.ClipBlit", &Script::Image::type, &image, &x, &y, &ix, &iy, &iw, &ih, &trans)) {
                return 0;
            }

            self->video->SetBlendMode((::Video::BlendMode)trans);
            self->video->ClipBlitImage(image->img, x, y, ix, iy, iw, ih);
            
            Py_INCREF(Py_None);
            return Py_None;
        }

        METHOD(Video_ScaleBlit) {
            Script::Image::ImageObject* image;
            int x, y;
            int w, h;
            int trans = 1;

            if (!PyArg_ParseTuple(args, "O!iiii|i:Video.ScaleBlit", &Script::Image::type, &image, &x, &y, &w, &h, &trans)) {
                return 0;
            }

            self->video->SetBlendMode((::Video::BlendMode)trans);
            self->video->ScaleBlitImage(image->img, x, y, w, h);
            
            Py_INCREF(Py_None);
            return Py_None;
        }

        METHOD(Video_DistortBlit) {
            Script::Image::ImageObject* image;
            int x[4], y[4];
            int trans = 1;

            if (!PyArg_ParseTuple(args, "O!(ii)(ii)(ii)(ii)|i:Video.DistortBlit", &Script::Image::type, &image, x, y, x + 1, y + 1, x + 2, y + 2, x + 3, y + 3, &trans)) {
                return 0;
            }

            self->video->SetBlendMode((::Video::BlendMode)trans);
            self->video->DistortBlitImage(image->img, x, y);
            
            Py_INCREF(Py_None);
            return Py_None;
        }

        METHOD(Video_TileBlit) {
            Script::Image::ImageObject* image;
            int x, y;
            int w, h;
            float scalex = 1, scaley = 1;
            int trans = 1;

            if (!PyArg_ParseTuple(args, "O!iiii|ffi:Video.TileBlit", &Script::Image::type, &image, &x, &y, &w, &h, &scalex, &scaley, &trans)) {
                return 0;
            }

            self->video->SetBlendMode((::Video::BlendMode)trans);
            self->video->TileBlitImage(image->img, x, y, w, h, scalex, scaley);
            
            Py_INCREF(Py_None);
            return Py_None;
        }

        METHOD(Video_TintBlit) {
            Script::Image::ImageObject* image;
            int x, y;
            u32 tint;
            uint blendMode = 1;

            if (!PyArg_ParseTuple(args, "O!iii|i:Video.TintBlit", &Script::Image::type, &image, &x, &y, &tint, &blendMode)) {
                return 0;
            }

            self->video->SetBlendMode((::Video::BlendMode)blendMode);
            self->video->TintBlitImage(image->img, x, y, tint);

            Py_INCREF(Py_None);
            return Py_None;
        }

        METHOD(Video_TintDistortBlit) {
            Script::Image::ImageObject* image;
            int x[4];
            int y[4];
            u32 tint[4];
            uint blendMode = 1;

            if (!PyArg_ParseTuple(args, "O!(iii)(iii)(iii)(iii)|i:Video.TintDistortBlit", 
                &Script::Image::type, &image, 
                x, y, tint, 
                x + 1, y + 1, tint + 1,
                x + 2, y + 2, tint + 2,
                x + 3, y + 3, tint + 3,
                &blendMode)
            ) {
                return 0;
            }

            self->video->SetBlendMode((::Video::BlendMode)blendMode);
            self->video->TintDistortBlitImage(image->img, x, y, tint);

            Py_INCREF(Py_None);
            return Py_None;
        }

        METHOD(Video_TintTileBlit) {
            Script::Image::ImageObject* image;
            int x, y;
            int w, h;
            u32 colour;
            float scalex = 1, scaley = 1;
            int trans = 1;

            if (!PyArg_ParseTuple(args, "O!iiiii|ffi:Video.TintTileBlit", 
                &Script::Image::type, &image, 
                &x, &y, &w, &h, &colour, &scalex, &scaley, &trans)
            ) {
                return 0;
            }

            self->video->SetBlendMode((::Video::BlendMode)trans);
            self->video->TintTileBlitImage(image->img, x, y, w, h, scalex, scaley, colour);
            
            Py_INCREF(Py_None);
            return Py_None;
        }

        METHOD(Video_DrawPixel) {
            int x, y;
            u32 colour;
            uint blendMode = 1;

            if (!PyArg_ParseTuple(args, "iii|i:Video.DrawPixel", &x, &y, &colour, &blendMode)) {
                return 0;
            }

            self->video->SetBlendMode((::Video::BlendMode)blendMode);
            self->video->DrawPixel(x, y, colour);

            Py_INCREF(Py_None);
            return Py_None;
        }

        METHOD(Video_DrawLine) {
            int x1, y1, x2, y2;
            u32 colour;
            uint blendMode = 1;

            if (!PyArg_ParseTuple(args, "iiiii|i:Video.DrawLine", &x1, &y1, &x2, &y2, &colour, &blendMode)) {
                return 0;
            }

            self->video->SetBlendMode((::Video::BlendMode)blendMode);
            self->video->DrawLine(x1, y1, x2, y2, colour);

            Py_INCREF(Py_None);
            return Py_None;
        }

        METHOD(Video_DrawRect) {
            int x1, y1, x2, y2;
            u32 colour;
            int filled = 0;
            uint blendMode = 1;

            if (!PyArg_ParseTuple(args, "iiiii|ii:Video.DrawRect", &x1, &y1, &x2, &y2, &colour, &filled, &blendMode)) {
                return 0;
            }

            self->video->SetBlendMode((::Video::BlendMode)blendMode);
            self->video->DrawRect(x1, y1, x2, y2, colour, filled != 0);

            Py_INCREF(Py_None);
            return Py_None;
        }

        METHOD(Video_DrawEllipse) {
            int cx, cy;
            int rx, ry;
            u32 colour;
            int filled = 0;
            uint blendMode = 1;

            if (!PyArg_ParseTuple(args, "iiiii|ii:Video.DrawEllipse", &cx, &cy, &rx, &ry, &colour, &filled, &blendMode)) {
                return 0;
            }

            self->video->SetBlendMode((::Video::BlendMode)blendMode);
            self->video->DrawEllipse(cx, cy, rx, ry, colour, filled != 0);

            Py_INCREF(Py_None);
            return Py_None;
        }

        METHOD(Video_DrawTriangle) {
            int x[3];
            int y[3];
            u32 col[3];
            uint blendMode = 1;

            if (!PyArg_ParseTuple(args, "(iii)(iii)(iii)|i:Video.DrawTriangle", x, y, col, x + 1, y + 1, col + 1, x + 2, y + 2, col + 2, &blendMode)) {
                return 0;
            }

            self->video->SetBlendMode((::Video::BlendMode)blendMode);
            self->video->DrawTriangle(x, y, col);

            Py_INCREF(Py_None);
            return Py_None;
        }

        METHOD(Video_ClipScreen) {
            const char* keywords[] = {
                "left", "top", "right", "bottom"
            };
            keywords;  // To remove use warning.

            Point p = self->video->GetResolution();

            int left = 0;
            int top = 0;
            int right = p.x;
            int bottom = p.y;

            if (!PyArg_ParseTuple(args, "|iiii:Video.ClipScreen", &left, &top, &right, &bottom)) {
                return 0;
            }

            self->video->ClipScreen(left, top, right, bottom);

            Py_INCREF(Py_None);
            return Py_None;
        }

        METHOD1(Video_GetClipRect) {
            Rect* points = self->video->GetClipRect();

            PyObject* cliprect = PyTuple_Pack(4, PyInt_FromLong(points->left), PyInt_FromLong(points->top), PyInt_FromLong(points->right), PyInt_FromLong(points->bottom));

            Py_INCREF(cliprect);
            return cliprect;
        }

        METHOD(Video_GrabImage) {
            int x1, y1, x2, y2;

            if (!PyArg_ParseTuple(args, "iiii:Video.GrabImage", &x1, &y1, &x2, &y2)) {
                return 0;
            }

            ::Video::Image* i = self->video->GrabImage(x1, y1, x2, y2);
            if (!i) {
                PyErr_SetString(PyExc_RuntimeError, "GrabImage failed!");
                return 0;
            }

            return ::Script::Image::New(i);
        }

        METHOD(Video_GrabCanvas) {
            int x1, y1, x2, y2;

            if (!PyArg_ParseTuple(args, "iiii:Video.GrabCanvas", &x1, &y1, &x2, &y2)) {
                return 0;
            }

            ::Canvas* c = self->video->GrabCanvas(x1, y1, x2, y2);
            if (!c) {
                PyErr_SetString(PyExc_RuntimeError, "GrabCanvas failed!");
                return 0;
            }

            return ::Script::Canvas::New(c);
        }

        METHOD1(Video_ClearScreen) {
            self->video->ClearScreen();

            Py_INCREF(Py_None);
            return Py_None;
        }

        METHOD1(Video_ShowPage) {
            engine->CheckMessages();
            self->video->ShowPage();

            Py_INCREF(Py_None);
            return Py_None;
        }

        METHOD(Video_SetResolution) {
            int x;
            int y;

            if (!PyArg_ParseTuple(args, "ii:SetResolution", &x, &y)) {
                return 0;
            }

            bool result = self->video->SwitchResolution(x, y);
            if (!result) {
                PyErr_SetString(PyExc_RuntimeError, va("Unable to set video mode %i x %i", x, y));
                return 0;
            }

            Py_INCREF(Py_None);
            return Py_None;
        }

#undef METHOD
    }
}
