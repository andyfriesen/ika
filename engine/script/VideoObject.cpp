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
                "Deprecated. Use ika.Image.Blit instead.\n"
            },
            
            {   "ClipBlit",         (PyCFunction)Video_ClipBlit,   METH_VARARGS,
                "ClipBlit(image, x, y, ix, iy, iw, ih[, blendmode])\n\n"
                "Deprecated. Use ika.Image.ClipBlit instead.\n"
            },

            {   "ScaleBlit",    (PyCFunction)Video_ScaleBlit,   METH_VARARGS,
                "ScaleBlit(image, x, y, width, height[, blendmode])\n\n"
                "Deprecated. Use ika.Image.ScaleBlit instead.\n"
            },

            {   "RotateBlit",    (PyCFunction)Video_RotateBlit,   METH_VARARGS,
                "RotateBlit(image, x, y, angle, [scalex[, scaley [, blendmode]]])\n\n"
                "Deprecated. Use ika.Image.RotateBlit instead.\n"
            },
            
            {   "DistortBlit",  (PyCFunction)Video_DistortBlit, METH_VARARGS,
                "DistortBlit(image, (upleftX, upleftY), (uprightX, uprightY), (downrightX, downrightY), (downleftX, downleftY)[, blendmode])\n\n"
				"Deprecated. Use ika.Image.DistortBlit instead.\n"
            },

            {   "TileBlit",     (PyCFunction)Video_TileBlit,    METH_VARARGS,
                "TileBlit(image, x, y, width, height[, scalex[, scaley[, blendmode]]])\n\n"
                "Deprecated. Use ika.Image.TileBlit instead.\n"
            },

            {   "TintBlit",     (PyCFunction)Video_TintBlit,    METH_VARARGS,
                "TintBlit(image, x, y, tintColour[, blendMode])\n\n"
                "Deprecated. Use ika.Image.TintBlit instead.\n"
			},

            {   "TintDistortBlit",  (PyCFunction)Video_TintDistortBlit, METH_VARARGS,
                "TintDistortBlit(image, (upleftX, upleftY, upleftTint), (uprightX, uprightY, uprightTint), (downrightX, downrightY, downrightTint), (downleftX, downleftY, downrightTint)[, blendmode])\n\n"
                "Deprecated. Use ika.Image.TintDistortBlit instead.\n"
            },

            {   "TintTileBlit",     (PyCFunction)Video_TintTileBlit, METH_VARARGS,
                "TintTileBlit(image, x, y, width, height, tintColour, scalex=1, scaley=1, blendmode=Normal)\n\n"
                "Deprecated. Use ika.Image.TintTileBlit instead.\n"
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
            
            {   "DrawArc",  (PyCFunction)Video_DrawArc, METH_VARARGS,
                "DrawArc(cx, cy, rx, ry, irx, iry, start, end, colour[, filled, blendmode])\n\n"
                "Draws an arc, centred at (cx, cy), of radius rx and ry and inner radius irx and iry on the X and\n"
                "Y axis, respectively, from angle start to angle end, in degrees.  If filled is omitted or nonzero, the arc is filled in\n"
                "else it is drawn as an outline."
            },
            
            {   "DrawTriangle", (PyCFunction)Video_DrawTriangle, METH_VARARGS,
                "DrawTriangle((x, y, colour), (x, y, colour), (x, y, colour)[, blendmode])\n\n"
                "Draws a filled triangle onscreen.  Each point is drawn in the colour specified, "
                "and a gradient is applied between the points."
            },
            
            {   "DrawQuad", (PyCFunction)Video_DrawQuad, METH_VARARGS,
                "DrawQuad((x, y, colour), (x, y, colour), (x, y, colour), (x, y, colour)[, blendmode])\n\n"
                "Draws a quad onscreen.  Each point is drawn in the colour specified.\n"
                "(A quad is two triangles from (p1,p2,p3) and (p2,p3,p4).  Therefore, make sure p2 and p3 are"
                "opposite corners of the quad.)"
            },

            {   "DrawLineList", (PyCFunction)Video_DrawLineList, METH_VARARGS,
                "DrawLineList(pointlist[, drawmode, blendmode]])\n\n"
                "Draws a bunch of points onscreen.  Each argument of pointlist should be a tuple in the format (x,y,colour)."
                "Each point is drawn in the colour specified, and a gradient is applied between the points.\n"
                "There is no extra python overhead on this function, so it is much better suited for drawing a lot of lines than"
                "calling DrawLine a bunch of times.\n"
                "Method of drawing depends on the value of drawmode as follows:\n"
                "0 - draws lines between each pair of points, so (p1,p2) (p3,p4) etc. (default)\n"
                "1 - draws lines between each point and the last point, so (p1,p2) (p2,p3) (p3,p4) etc.\n"
                "2 - draws lines between the first point and the other points, so (p1,p2) (p1,p3) (p1,p4) etc.\n"
                "3 - draws lines between each point and every other point."
            },
                        
            {   "DrawTriangleList", (PyCFunction)Video_DrawTriangleList, METH_VARARGS,
                "DrawTriangleList(pointlist[, drawmode, blendmode]])\n\n"
                "Draws a bunch of filled triangles onscreen.  Each argument of pointlist should be a tuple in the format (x,y,colour)."
                "Each point is drawn in the colour specified, and a gradient is applied between the points.\n"
                "There is no extra python overhead on this function, so it is much better suited for drawing a lot of triangles than"
                "calling DrawTriangle a bunch of times.\n"
                "Method of drawing depends on the value of drawmode as follows:\n"
                "0 - draws triangles between each set of 3 points, so (p1,p2,p3) (p4,p5,p6) etc. (default)\n"
                "1 - draws triangles between each point and the last 2 points, so (p1,p2,p3) (p2,p3,p4) (p3,p4,p5) etc.\n"
                "2 - draws triangles between the first point, the previous point, and the current point, so (p1,p2,p3) (p1,p3,p4) (p1,p4,p5) etc."
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
            {   "colours",  (getter)getColours, 0,  "Alias for colors."      },
			{	"colors",	(getter)getColours, 0,	"Gets a mapping containing the currently-defined colours by name."		},
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
#define BLIT_DEPRECATION_WARNING(name) \
				static bool warnFlag = false; \
				if(!warnFlag) \
				{ \
					Log::Write("* ika.Video."#name" is deprecated. Use ika.Image."#name" instead."); \
					warnFlag = true; \
				}

        METHOD(Video_Blit) {
			BLIT_DEPRECATION_WARNING(Blit);

            Script::Image::ImageObject* image;
            int x, y;
            int blendMode = ::Video::Normal;

            if (!PyArg_ParseTuple(args, "O!ii|i:Video.Blit", &Script::Image::type, &image, &x, &y, &blendMode)) {
                return 0;
            }

            self->video->SetBlendMode((::Video::BlendMode)blendMode);
            self->video->BlitImage(image->img, x, y);
            
            Py_INCREF(Py_None);
            return Py_None;
        }

        METHOD(Video_ClipBlit) {
			BLIT_DEPRECATION_WARNING(ClipBlit);
            Script::Image::ImageObject* image;
            int x, y, ix, iy, iw, ih;
            int blendMode = ::Video::Normal;

            if (!PyArg_ParseTuple(args, "O!iiiiii|i:Video.ClipBlit", &Script::Image::type, &image, &x, &y, &ix, &iy, &iw, &ih, &blendMode)) {
                return 0;
            }

            self->video->SetBlendMode((::Video::BlendMode)blendMode);
            self->video->ClipBlitImage(image->img, x, y, ix, iy, iw, ih);
            
            Py_INCREF(Py_None);
            return Py_None;
        }

        METHOD(Video_ScaleBlit) {
			BLIT_DEPRECATION_WARNING(ScaleBlit);
            Script::Image::ImageObject* image;
            int x, y;
            int w, h;
            int blendMode = ::Video::Normal;

            if (!PyArg_ParseTuple(args, "O!iiii|i:Video.ScaleBlit", &Script::Image::type, &image, &x, &y, &w, &h, &blendMode)) {
                return 0;
            }

            self->video->SetBlendMode((::Video::BlendMode)blendMode);
            self->video->ScaleBlitImage(image->img, x, y, w, h);
            
            Py_INCREF(Py_None);
            return Py_None;
        }

        METHOD(Video_RotateBlit) {
			BLIT_DEPRECATION_WARNING(RotateBlit);
            Script::Image::ImageObject* image;
            int x, y;
			float angle;
            float scalex = 1;
			float scaley = 1;
            int blendMode = ::Video::Normal;

            if (!PyArg_ParseTuple(args, "O!iif|ffi:Video.RotateBlit", &Script::Image::type, &image, &x, &y, &angle, &scalex, &scaley, &blendMode)) {
                return 0;
            }

            self->video->SetBlendMode((::Video::BlendMode)blendMode);
            self->video->RotateBlitImage(image->img, x, y, angle, scalex, scaley);
            
            Py_INCREF(Py_None);
            return Py_None;
		}

        METHOD(Video_DistortBlit) {
			BLIT_DEPRECATION_WARNING(DistortBlit);
            Script::Image::ImageObject* image;
            int x[4], y[4];
            int blendMode = ::Video::Normal;

            if (!PyArg_ParseTuple(args, "O!(ii)(ii)(ii)(ii)|i:Video.DistortBlit", &Script::Image::type, &image, x, y, x + 1, y + 1, x + 2, y + 2, x + 3, y + 3, &blendMode)) {
                return 0;
            }

            self->video->SetBlendMode((::Video::BlendMode)blendMode);
            self->video->DistortBlitImage(image->img, x, y);
            
            Py_INCREF(Py_None);
            return Py_None;
        }

        METHOD(Video_TileBlit) {
			BLIT_DEPRECATION_WARNING(TileBlit);
            Script::Image::ImageObject* image;
            int x, y;
            int w, h;
            float scalex = 1, scaley = 1;
            int blendMode = ::Video::Normal;

            if (!PyArg_ParseTuple(args, "O!iiii|ffi:Video.TileBlit", &Script::Image::type, &image, &x, &y, &w, &h, &scalex, &scaley, &blendMode)) {
                return 0;
            }

            self->video->SetBlendMode((::Video::BlendMode)blendMode);
            self->video->TileBlitImage(image->img, x, y, w, h, scalex, scaley);
            
            Py_INCREF(Py_None);
            return Py_None;
        }

        METHOD(Video_TintBlit) {
			BLIT_DEPRECATION_WARNING(TintBlit);
            Script::Image::ImageObject* image;
            int x, y;
            u32 tint;
            int blendMode = ::Video::Normal;

            if (!PyArg_ParseTuple(args, "O!iii|i:Video.TintBlit", &Script::Image::type, &image, &x, &y, &tint, &blendMode)) {
                return 0;
            }

            self->video->SetBlendMode((::Video::BlendMode)blendMode);
            self->video->TintBlitImage(image->img, x, y, tint);

            Py_INCREF(Py_None);
            return Py_None;
        }

        METHOD(Video_TintDistortBlit) {
			BLIT_DEPRECATION_WARNING(TintDistortBlit);
            Script::Image::ImageObject* image;
            int x[4];
            int y[4];
            u32 tint[4];
            int blendMode = ::Video::Normal;

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
			BLIT_DEPRECATION_WARNING(TintTileBlit);
            Script::Image::ImageObject* image;
            int x, y;
            int w, h;
            u32 colour;
            float scalex = 1, scaley = 1;
            int blendMode = ::Video::Normal;

            if (!PyArg_ParseTuple(args, "O!iiiii|ffi:Video.TintTileBlit", 
                &Script::Image::type, &image, 
                &x, &y, &w, &h, &colour, &scalex, &scaley, &blendMode)
            ) {
                return 0;
            }

            self->video->SetBlendMode((::Video::BlendMode)blendMode);
            self->video->TintTileBlitImage(image->img, x, y, w, h, scalex, scaley, colour);
            
            Py_INCREF(Py_None);
            return Py_None;
        }

        METHOD(Video_DrawPixel) {
            int x, y;
            u32 colour;
            int blendMode = ::Video::Normal;

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
            int blendMode = ::Video::Normal;

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
            int blendMode = ::Video::Normal;

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
            int blendMode = ::Video::Normal;

            if (!PyArg_ParseTuple(args, "iiiii|ii:Video.DrawEllipse", &cx, &cy, &rx, &ry, &colour, &filled, &blendMode)) {
                return 0;
            }

            self->video->SetBlendMode((::Video::BlendMode)blendMode);
            self->video->DrawEllipse(cx, cy, rx, ry, colour, filled != 0);

            Py_INCREF(Py_None);
            return Py_None;
        }

        METHOD(Video_DrawArc) {
            int cx, cy;
            int rx, ry;
            int irx, iry;
            int start, end;
            u32 colour;
            int filled = 0;
            int blendMode = ::Video::Normal;

            if (!PyArg_ParseTuple(args, "iiiiiiiii|ii:Video.DrawArc", &cx, &cy, &rx, &ry, &irx, &iry, &start, &end, &colour, &filled, &blendMode)) {
                return 0;
            }

            self->video->SetBlendMode((::Video::BlendMode)blendMode);
            self->video->DrawArc(cx, cy, rx, ry, irx, iry, start, end, colour, filled != 0);

            Py_INCREF(Py_None);
            return Py_None;
        }
        
        METHOD(Video_DrawTriangle) {
            int x[3];
            int y[3];
            u32 col[3];
            int blendMode = ::Video::Normal;

            if (!PyArg_ParseTuple(args, "(iii)(iii)(iii)|i:Video.DrawTriangle", x, y, col, x + 1, y + 1, col + 1, x + 2, y + 2, col + 2, &blendMode)) {
                return 0;
            }

            self->video->SetBlendMode((::Video::BlendMode)blendMode);
            self->video->DrawTriangle(x, y, col);

            Py_INCREF(Py_None);
            return Py_None;
        }

        METHOD(Video_DrawQuad) {
            int x[4];
            int y[4];
            u32 col[4];
            int blendMode = ::Video::Normal;

            if (!PyArg_ParseTuple(args, "(iii)(iii)(iii)(iii)|i:Video.DrawQuad", x, y, col, x + 1, y + 1, col + 1, x + 2, y + 2, col + 2, x + 3, y + 3, col + 3, &blendMode)) {
                return 0;
            }

            self->video->SetBlendMode((::Video::BlendMode)blendMode);
            self->video->DrawQuad(x, y, col);

            Py_INCREF(Py_None);
            return Py_None;
        }
        
        METHOD(Video_DrawLineList) {
            
            int drawMode = 0;
            int blendMode = ::Video::Normal;

            PyObject* pointList;
            if (!PyArg_ParseTuple(args, "O!|ii:Video.DrawLineList", &PyList_Type, &pointList, &drawMode, &blendMode))
                return 0;
                
            int len = PyObject_Length(pointList);
            
            std::vector<int> px;
            std::vector<int> py;
            std::vector<u32> pc;
            
            int x = 0;
            int y = 0;
            u32 col = 0;
            
            px.reserve(len);
            py.reserve(len);
            pc.reserve(len);
            
            for (int i = 0; i < len; i++) {
                
                PyObject* slice = PyList_GetItem(pointList, i);
                
                if (!PyArg_ParseTuple(slice, "iii:Video.DrawLineList", &x, &y, &col))
                    return 0;
                    
                px.push_back(x);
                py.push_back(y);
                pc.push_back(col);
            }

            self->video->SetBlendMode((::Video::BlendMode)blendMode);
            self->video->DrawLineList(px, py, pc, drawMode);

            Py_INCREF(Py_None);
            return Py_None;
        }
        
        METHOD(Video_DrawTriangleList) {
            
            int drawMode = 0;
            int blendMode = ::Video::Normal;

            PyObject* pointList;
            if (!PyArg_ParseTuple(args, "O!|ii:Video.DrawTriangleList", &PyList_Type, &pointList, &drawMode, &blendMode))
                return 0;
                
            int len = PyObject_Length(pointList);
            
            std::vector<int> px;
            std::vector<int> py;
            std::vector<u32> pc;
            
            int x = 0;
            int y = 0;
            u32 col = 0;
            
            px.reserve(len);
            py.reserve(len);
            pc.reserve(len);
            
            for (int i = 0; i < len; i++) {
                
                PyObject* slice = PyList_GetItem(pointList, i);
                
                if (!PyArg_ParseTuple(slice, "iii:Video.DrawTriangleList", &x, &y, &col))
                    return 0;
                    
                px.push_back(x);
                py.push_back(y);
                pc.push_back(col);
            }

            self->video->SetBlendMode((::Video::BlendMode)blendMode);
            self->video->DrawTriangleList(px, py, pc, drawMode);

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
