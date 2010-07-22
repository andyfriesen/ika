/*
Python font object
*/

#include "ObjectDefs.h"
#include "font.h"
#include "main.h"

namespace Script {
    namespace Font {
        PyTypeObject type;

        PyMethodDef methods[] = {
            {   (char*)"Print",        (PyCFunction)Font_Print,        METH_VARARGS,
                (char*)"Font.Print(x, y, text)\n\n"
                "Prints a string of text on screen at (x, y)."
            },
            {   (char*)"CenterPrint",  (PyCFunction)Font_CenterPrint,  METH_VARARGS,
                (char*)"Font.CenterPrint(x, y, text)\n\n"
                "Prints a string of text on screen, \n"
                "with x as the center point rather than the leftmost point."

            },
            {   (char*)"RightPrint",   (PyCFunction)Font_RightPrint,   METH_VARARGS,
                (char*)"Font.RightPrint(x, y, text)\n\n"
                "Prints a string of text on screen, with x as the rightmost point\n"
                "rather than the leftmost point."
            },
            {   (char*)"StringWidth",  (PyCFunction)Font_StringWidth,  METH_VARARGS,
                (char*)"Font.StringWidth(text) -> int\n\n"
                "Returns how many pixels in width the passed string would be, \n"
                "if printed in this font.  Takes newlines into account."
            },
            {   (char*)"StringHeight",  (PyCFunction)Font_StringHeight,  METH_VARARGS,
                (char*)"Font.StringHeight(text) -> int\n\n"
                "Returns how many pixels in height the passed string would be, \n"
                "if printed in this font.  Takes newlines into account."
            },            
            {   0,           0    }
        };

#define GET(x) PyObject* get ## x(FontObject* self)
#define SET(x) PyObject* set ## x(FontObject* self, PyObject* value)
        GET(Width)         { return PyInt_FromLong(self->font->Width()); }
        GET(Height)        { return PyInt_FromLong(self->font->Height()); }
        GET(TabSize)       { return PyInt_FromLong(self->font->TabSize()); }
        SET(TabSize)       { self->font->SetTabSize(PyInt_AsLong(value)); return 0; }
        GET(LetterSpacing) { return PyInt_FromLong(self->font->LetterSpacing()); }
        SET(LetterSpacing) { self->font->SetLetterSpacing(PyInt_AsLong(value)); return 0; }
        GET(WordSpacing)   { return PyInt_FromLong(self->font->WordSpacing()); }
        SET(WordSpacing)   { self->font->SetWordSpacing(PyInt_AsLong(value)); return 0; }        
        GET(LineSpacing)   { return PyInt_FromLong(self->font->WordSpacing()); }
        SET(LineSpacing)   { self->font->SetLineSpacing(PyInt_AsLong(value)); return 0; }        
#undef GET
#undef SET

        PyGetSetDef properties[] = {
            {   (char*)"width",         (getter)getWidth,        0,                        (char*)"Gets the width of the widest glyph in the font."   },
            {   (char*)"height",        (getter)getHeight,       0,                        (char*)"Gets the height of the font."  },
            {   (char*)"tabsize",       (getter)getTabSize,      (setter)setTabSize,       (char*)"Gets or sets the tab size of the font."    },
            {   (char*)"letterspacing", (getter)getLetterSpacing,(setter)setLetterSpacing, (char*)"Gets or sets the letter spacing of the font."   },
            {   (char*)"wordspacing",   (getter)getWordSpacing,  (setter)setWordSpacing,   (char*)"Gets or sets the word spacing of the font."   },
            {   (char*)"linespacing",   (getter)getLineSpacing,  (setter)setLineSpacing,   (char*)"Gets or sets the line spacing of the font."   },
            {   0   }
        };

        void Init() {
            memset(&type, 0, sizeof type);

            type.ob_refcnt = 1;
            type.ob_type=&PyType_Type;
            type.tp_name="Font";
            type.tp_basicsize = sizeof type;
            type.tp_dealloc=(destructor)Destroy;
            type.tp_methods = methods;
            type.tp_getset = properties;
            type.tp_doc="ika.Font(fontFileName)->font\n\n"
                "A proportional bitmap font.  fontFileName is a string that contains\n"
                "the filename to a font file.";
            type.tp_new = New;

            PyType_Ready(&type);
        }

        PyObject* New(PyTypeObject* type, PyObject* args, PyObject* kw) {
            static char* keywords[] = { (char*)"filename", 0 };
            char* filename;

            if (!PyArg_ParseTupleAndKeywords(args, kw, "s:Font", keywords, &filename)) {
                return 0;
            }

            FontObject* font = PyObject_New(FontObject, type);
            if (!font) {
                return 0;
            }

            try {
                font->font = new Ika::Font(filename, engine->video);
            } catch (Ika::FontException) {
                PyErr_SetString(PyExc_IOError, va("Failed to load %s", filename));
                return 0;
            }

            return (PyObject*)font;
        }

        void Destroy(FontObject* self) {
            delete self->font;

            PyObject_Del(self);
        }

#define METHOD(x) PyObject* x(FontObject* self, PyObject* args)

        METHOD(Font_Print) {
            int x, y;
            char* message;

            if (!PyArg_ParseTuple(args, "iis:Font.Print", &x, &y, &message))
                return 0;

            self->font->PrintString(x, y, message);

            Py_INCREF(Py_None);
            return Py_None;
        }

        METHOD(Font_CenterPrint) {
            int x, y;
            char* message;

            if (!PyArg_ParseTuple(args, "iis:Font.CenterPrint", &x, &y, &message)) {
                return 0;
            }

            Ika::Font* f = self->font;
            f->PrintString(x - f->StringWidth(message) / 2 , y, message);

            Py_INCREF(Py_None);
            return Py_None;
        }

        METHOD(Font_RightPrint) {
            int x, y;
            char* message;

            if (!PyArg_ParseTuple(args, "iis:Font.RightPrint", &x, &y, &message)) {
                return 0;
            }

            Ika::Font* f = self->font;
            f->PrintString(x - f->StringWidth(message) , y, message);

            Py_INCREF(Py_None);
            return Py_None;
        }

        METHOD(Font_StringWidth) {
            char* message;

            if (!PyArg_ParseTuple(args, "s:Font.StringWidth", &message)) {
                return 0;
            }

            return PyInt_FromLong(self->font->StringWidth(message));
        }

        METHOD(Font_StringHeight) {
            char* message;

            if (!PyArg_ParseTuple(args, "s:Font.StringHeight", &message)) {
                return 0;
            }

            return PyInt_FromLong(self->font->StringHeight(message));
        }
        
#undef METHOD
    }
}
