/*
Colour registry pseudo-dictionary
*/

#include "ObjectDefs.h"
#include "../video/ColourHandler.h"
#include "main.h"

namespace Script {
    namespace Colours {
        PyTypeObject type;
        PyObject obj;
        PyMappingMethods mapmethods;
        PySequenceMethods seqmethods;
        
        PyMethodDef methods[] = {
            {   "has_key",   (PyCFunction)Colours_HasKey, METH_VARARGS, ""
            },
            {   "keys",   (PyCFunction)Colours_Keys, METH_NOARGS, ""
            },
            {   "copy",   (PyCFunction)Colours_Copy, METH_NOARGS, ""
            },
            {   "items",   (PyCFunction)Colours_Items, METH_NOARGS, ""
            },
            {   0   }
        };
        
        int Colours_Len(ColoursObject* self);
        int Colours_Contains(ColoursObject* self, PyObject* key);
        PyObject* Colours_GetItem(ColoursObject* self, PyObject* key);
        int Colours_SetItem(ColoursObject* self, PyObject* key, PyObject* value);
        PyObject* Colours_Iter(ColoursObject* self);

        void Init() {
            memset(&type, 0, sizeof type);
            
            mapmethods.mp_length = (lenfunc)&Colours_Len;
            mapmethods.mp_subscript = (binaryfunc)&Colours_GetItem;
            mapmethods.mp_ass_subscript = (objobjargproc)&Colours_SetItem;
            
            memset(&seqmethods, 0, sizeof(PySequenceMethods));
            seqmethods.sq_contains = (objobjproc)&Colours_Contains;

            obj.ob_refcnt = 1;
            obj.ob_type = &PyType_Type;
            type.tp_name = "Colours";
            type.tp_basicsize = sizeof type;
            type.tp_dealloc = (destructor)Destroy;
            type.tp_methods = methods;
            // type.tp_flags = Py_TPFLAGS_HAVE_SEQUENCE_IN | Py_TPFLAGS_HAVE_ITER;
            type.tp_as_mapping = &mapmethods;
            type.tp_as_sequence = &seqmethods;
            type.tp_iter = (getiterfunc)Colours_Iter;
            type.tp_doc = "Represents a colour registry object\n"
                "This can be accessed only through the Video interface.";

            PyType_Ready(&type);
        }

        PyObject* New(ColourHandler* handler) {
            ColoursObject* coloursObject = PyObject_New(ColoursObject, &type);

            if (!coloursObject)
                return 0;
            coloursObject->handler = handler;
            return (PyObject*)coloursObject;
        }

        void Destroy(ColoursObject* self) {
            PyObject_Del(self);
        }
        
        PyObject* Colours_GetItem(ColoursObject* self, PyObject* key) {
            char* name = PyBytes_AsString(key);
            if (!name) {
                PyErr_SetString(PyExc_SyntaxError, "Colour names must be strings!");
                return 0;
            }

            if(!self->handler->hasColour(name)) {
                PyErr_SetString(PyExc_KeyError, "Colour not defined.");
                return 0;
            }

            return PyLong_FromLong(self->handler->getColour(name));
        }

        int Colours_SetItem(ColoursObject* self, PyObject* key, PyObject* value) {
            char* name = PyBytes_AsString(key);
            if (!name || name[0] == '\0') {
                PyErr_SetString(PyExc_TypeError, "Colour names must be non-empty strings!");
                return -1;
            }

            if(value == 0) {
                self->handler->removeColour(name);
            } else {
                if(!PyLong_Check(value)) {
                    PyErr_SetString(PyExc_TypeError, "Only colour values can be inserted.");
                    return -1;
                }
                RGBA colour = PyLong_AsLong(value);
                self->handler->addColour(name, colour);
            }
            return 0;
        }

        int Colours_Len(ColoursObject* self) {
            return self->handler->getColourMap().size();
        }
        
        int Colours_Contains(ColoursObject* self, PyObject* key) {
            char* colour = PyBytes_AsString(key);
            if(!colour)
                return 0;

            return self->handler->hasColour(colour);
        }
        
        #define METHOD(x)  PyObject* x(ColoursObject* self, PyObject* args)
        #define METHOD1(x) PyObject* x(ColoursObject* self)
        
        METHOD1(Colours_Iter) {
            return PySeqIter_New(Colours_Keys(self));
        }
        
        METHOD(Colours_HasKey) {
            char* name;

            if (!PyArg_ParseTuple(args, "s:Contains", &name)) {
                return 0;
            }
            
            return PyLong_FromLong(self->handler->hasColour(name));
        }
        
        METHOD1(Colours_Keys) {
            const ColourHandler::colourMap& colours = self->handler->getColourMap();
            PyObject* keys;
            PyObject* name;

            if((keys = PyList_New(colours.size())) == 0)
                return 0;

            ColourHandler::colourMap::const_iterator iter = colours.begin();
            for(int i = 0; iter != colours.end(); iter++, i++) {
                name = PyBytes_FromString(iter->first.c_str());
                if(name == 0)
                    return 0;

                PyList_SetItem(keys, i, name);
                // Py_XDECREF(name);
            }
            return keys;
        }
        
        METHOD1(Colours_Copy) {
            const ColourHandler::colourMap& colours = self->handler->getColourMap();
            PyObject* dict;
            PyObject* colour;

            if((dict = PyDict_New()) == 0)
                return 0;

            ColourHandler::colourMap::const_iterator iter = colours.begin();
            for(int i = 0; iter != colours.end(); iter++, i++) {
                colour = PyLong_FromLong(iter->second);
                if(PyDict_SetItemString(dict, iter->first.c_str(), colour) < 0)
                    return 0;
                Py_XDECREF(colour);
            }
            return dict;
        }
        
        METHOD1(Colours_Items) {
            const ColourHandler::colourMap& colours = self->handler->getColourMap();
            PyObject* items;
            PyObject* tuple;
            
            if((items = PyList_New(colours.size())) == 0)
                return 0;

            ColourHandler::colourMap::const_iterator iter = colours.begin();
            for(int i = 0; iter != colours.end(); iter++, i++) {
                tuple = PyTuple_New(2);
                PyTuple_SetItem(tuple, 0, PyBytes_FromString(iter->first.c_str()));
                PyTuple_SetItem(tuple, 1, PyLong_FromLong(iter->second));
                PyList_SetItem(items, i, tuple);
                //Py_XDECREF(tuple);
            }
            return items;
        }
        
    }
}
