#include "ObjectDefs.h"
#include "common/Canvas.h"
#include "common/log.h"
#include "font.h"

#include <stdexcept>

namespace Script
{
    namespace Canvas
    {
        template <typename T, T U, typename V>
        PyObject* Get(T* self, PyObject* value);

        template <typename T, T U, int>
        PyObject* Get(T* self, PyObject* value)
        {
            return PyInt_FromLong(self->U);
        }

        PyTypeObject type;

        PyMethodDef methods[] =
        {
            {   "Save",     (PyCFunction)Canvas_Save,       METH_VARARGS,
                "Canvas.Save(fname)\n\n"
                "Writes the image to the filename specified in PNG format.\n"
                "\n"
                "ie.  canvas.Save('myimage.png')"
            },

            {   "AlphaMask",     (PyCFunction)Canvas_AlphaMask,       METH_NOARGS,
                "Canvas.AlphaMask()\n\n"
                "Takes the max of r/g/b for each pixel, and applies the value to that pixel's a.\n"
                "Very handy in conjunction with ika.Video.GrabCanvas and canvas blitting routines."
            },
            
            {   "Blit",     (PyCFunction)Canvas_Blit,       METH_VARARGS,
                "Canvas.Blit(destcanvas, x, y, blendmode)\n\n"
                "Draws the image on destcanvas, at position (x, y)\n"
				"blendmode specifies the algorithm used to blend pixels.  It is one of\n"
				"the available blend modes defined in ika's constants section.\n"
                "blendmode defaults to ika.AlphaBlend."
            },

            {   "ScaleBlit", (PyCFunction)Canvas_ScaleBlit,  METH_VARARGS,
                "Canvas.ScaleBlit(destcanvas, x, y, width, height, blendmode)\n\n"
                "Draws the image on destcanvas, at position (x, y), scaled to (width, height) pixels in size.\n"
				"blendmode specifies the algorithm used to blend pixels.  It is one of\n"
				"the available blend modes defined in ika's constants section.\n"
                "blendmode defaults to ika.AlphaBlend."
            },

            {   "TileBlit", (PyCFunction)Canvas_TileBlit,   METH_VARARGS,
                "Canvas.TileBlit(destcanvas, x, y, width, height, [offsetx, offsety, blendmode])\n\n"
                "Tiles the image within the region specified, on destcanvas.  The tiling is offset\n"
                "(moved) by offsetx and offsety. (both default to zero)\n"
				"blendmode specifies the algorithm used to blend pixels.  It is one of\n"
				"the available blend modes defined in ika's constants section.\n"
                "blendmode defaults to ika.AlphaBlend."
            },

            {   "GetPixel", (PyCFunction)Canvas_GetPixel,   METH_VARARGS,
                "Canvas.GetPixel(x, y)\n\n"
                "Returns the pixel at position (x, y) on the canvas, as a packed 32bpp RGBA colour."
            },

            {   "SetPixel", (PyCFunction)Canvas_SetPixel,   METH_VARARGS,
                "Canvas.SetPixel(x, y, colour)\n\n"
                "Sets the pixel at (x, y) on the canvas to the colour specified.  This function\n"
                "totally disregards alpha.  It *sets* the pixel, in the truest sense of the word."
            },

            {   "DrawLine", (PyCFunction)Canvas_DrawLine,   METH_VARARGS,
                "Canvas.DrawLine(x1, y1, x2, y2, colour, [blendmode])\n\n"
                "Draws a line from (x1, y1) to (x2, y2) of the colour and blendmode specified."
            },
            
            {   "DrawRect", (PyCFunction)Canvas_DrawRect,   METH_VARARGS,
                "Canvas.DrawRect(x1, y1, x2, y2, colour, [blendmode])\n\n"
                "Draws a rect from top left (x1, y1) to bottom right (x2, y2) of the colour and blendmode specified."
            },
            
            {   "DrawText", (PyCFunction)Canvas_DrawText,   METH_VARARGS,
                "Canvas.DrawText(font, x, y, text)\n\n"
                "Draws a string starting at (x,y) with the font.  No word wrapping is done."
            },

            {   "Clear",    (PyCFunction)Canvas_Clear,      METH_VARARGS,
                "Canvas.Clear([colour])\n\n"
                "Sets every pixel on the canvas to the colour given.  If colour is omitted, \n"
                "flat black is used."
            },

            {   "Resize",   (PyCFunction)Canvas_Resize,     METH_VARARGS,
                "Canvas.Resize(width, height)\n\n"
                "Resizes the canvas to the size specified.  No scaling takes place; if the dimensions\n"
                "given are smaller than the existing image, it is cropped.  Blank, transparent space is\n"
                "added when the canvas is enlarged."
            },

            {   "Clip",     (PyCFunction)Canvas_Clip,       METH_VARARGS,
                "Canvas.Clip([x, y, width, height])\n\n"
                "Sets the clipping rectangle of the canvas.  Blitting to the canvas will be confined to\n"
                "the clip rectangle; blitting the canvas on others will only blit the clipped region.\n\n"
                "If no arguments are specified, the clip rectangle is reset to cover the whole canvas."
            },

            {   "Rotate",   (PyCFunction)Canvas_Rotate,     METH_NOARGS,
                "Canvas.Rotate()\n\n"
                "Rotates the contents of the canvas 90 degrees, clockwise."
            },

            {   "Flip",     (PyCFunction)Canvas_Flip,       METH_NOARGS,
                "Canvas.Flip()\n\n"
                "Flips the contents of the canvas on the X axis.\n"
                "(turns it upside down)"
            },

            {   "Mirror",   (PyCFunction)Canvas_Mirror,     METH_NOARGS,
                "Canvas.Mirror()\n\n"
                "Mirrors the contents of the canvas along the Y axis.\n"
                "(left to right)"
            },
            {   0   }
        };

        PyObject* getWidth(CanvasObject* self)  { return PyInt_FromLong(self->canvas->Width());  }
        PyObject* getHeight(CanvasObject* self) { return PyInt_FromLong(self->canvas->Height()); }

        PyGetSetDef properties[] =
        {
            {   (char*)"width",    (getter)getWidth, 0, (char*)"Gets the width of the canvas" },
            {   (char*)"height",   (getter)getHeight, 0, (char*)"Gets the height of the canvas" },
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
            type.tp_new = New;
            type.tp_doc = 
                "A software representation of an image that can be manipulated easily.\n\n"
                "Canvas(filename)\n\n"
                "Loads the image specified by 'filename' into a new canvas.\n\n"
                "Canvas(width, height)\n\n"
                "Creates a new, blank canvas of the specified size.";
    
            PyType_Ready(&type);
        }

        PyObject* New(::Canvas* c)
        {
            CanvasObject* canvas = PyObject_New(CanvasObject, &type);
            canvas->canvas = c;
            canvas->ref = false;
            return (PyObject*)canvas;
        }
        
        PyObject* New(PyTypeObject* type, PyObject* args, PyObject* kw)
        {          
			static char* keywords[] = { (char*)"x", (char*)"y", 0 };
            PyObject* o;
            int x, y;

            if (!PyArg_ParseTupleAndKeywords(args, kw, "O|i:__init__", keywords, &o, &y))
                return 0;

            if (o->ob_type == &PyInt_Type)
            {
                x = PyInt_AsLong(o);
                if (x < 1 || y < 1)
                {
                    PyErr_SetString(PyExc_RuntimeError, va("Can't create %ix%i canvas.", x, y));
                    return 0;
                }

                CanvasObject* c = PyObject_New(CanvasObject, type);
                c->canvas = new ::Canvas(x, y);
                c->ref = false;

                return (PyObject*)c;
            }
            else if (o->ob_type == &PyString_Type)
            {
                char* fname = PyString_AsString(o);
                CanvasObject* c = PyObject_New(CanvasObject, type);
                try {
                    c->canvas = new ::Canvas(fname);
                    c->ref = false;
                    return (PyObject*)c;
                }
                catch (std::runtime_error) {
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

        METHOD1(Canvas_AlphaMask)
        {
            Blitter::AlphaMask(*self->canvas);

            Py_INCREF(Py_None);
            return Py_None;        
        }
        
        METHOD(Canvas_Blit)
        {
            CanvasObject* dest;
            int x, y;
            ::Video::BlendMode blendMode = ::Video::Normal;

            if (!PyArg_ParseTuple(args, "O!ii|i:Blit", &type, &dest, &x, &y, &blendMode))
                return 0;

            Blitter::Blit(*self->canvas, *dest->canvas, x, y, *Blitter::GetBlender(blendMode));

            Py_INCREF(Py_None);
            return Py_None;
        }

        METHOD(Canvas_ScaleBlit)
        {
            CanvasObject* dest;
            int x, y;
            int w, h;
            ::Video::BlendMode blendMode = ::Video::Normal;

            if (!PyArg_ParseTuple(args, "O!iiii|i:ScaleBlit", &type, &dest, &x, &y, &w, &h, &blendMode))
                return 0;

            Blitter::ScaleBlit(*self->canvas, *dest->canvas, x, y, w, h, *Blitter::GetBlender(blendMode));

            Py_INCREF(Py_None);
            return Py_None;
        }

        METHOD(Canvas_TileBlit)
        {
            CanvasObject* dest;
            int x, y;
            int w, h;
            int ofsx = 0;
            int ofsy = 0;
            ::Video::BlendMode blendMode = ::Video::Normal;

            if (!PyArg_ParseTuple(args, "O!iiii|iii:TileBlit", &type, &dest, &x, &y, &w, &h, &ofsx, &ofsy, &blendMode))
                return 0;

            Blitter::TileBlit(*self->canvas, *dest->canvas, x, y, w, h, ofsx, ofsy, *Blitter::GetBlender(blendMode));

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

        METHOD(Canvas_DrawLine)
        {
            int x1, y1, x2, y2;
            u32 colour;
            ::Video::BlendMode blendMode = ::Video::Normal;

            if (!PyArg_ParseTuple(args, "iiiii|i:DrawLine", &x1, &y1, &x2, &y2, &colour, &blendMode))
                return 0;

            Blitter::DrawLine(*self->canvas, x1, y1, x2, y2, colour, *Blitter::GetBlender(blendMode));

            Py_INCREF(Py_None);
            return Py_None;
        }

        METHOD(Canvas_DrawRect)
        {
            int x1, y1, x2, y2;
            u32 colour;
            bool filled;
            ::Video::BlendMode blendMode = ::Video::Normal;

            if (!PyArg_ParseTuple(args, "iiiii|ii:DrawRect", &x1, &y1, &x2, &y2, &colour, &filled, &blendMode))
                return 0;

            Blitter::DrawRect(*self->canvas, x1, y1, x2, y2, colour, filled, *Blitter::GetBlender(blendMode));

            Py_INCREF(Py_None);
            return Py_None;
        }
                
        METHOD(Canvas_DrawText)
        {
            Script::Font::FontObject* font;
            int x, y;
            char* text;
            ::Video::BlendMode blendMode = ::Video::Normal;

            if (!PyArg_ParseTuple(args, "O!iis|i:DrawText", &Script::Font::type, &font, &x, &y, &text, &blendMode))
                return 0;

            font->font->PrintString(x, y, text, *self->canvas, blendMode);

            Py_INCREF(Py_None);
            return Py_None;
        }

        METHOD(Canvas_Clear)
        {
            u32 colour = 0;
            if (!PyArg_ParseTuple(args, "|i:SetPixel", &colour))
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

        METHOD(Canvas_Clip)
        {
            int x = 0;
            int y = 0;
            int w = self->canvas->Width();
            int h = self->canvas->Height();

            if (!PyArg_ParseTuple(args, "|iiii:Clip", &x, &y, &w, &h))
                return 0;

            self->canvas->SetClipRect(Rect(x, y, x + w, y + h));

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
