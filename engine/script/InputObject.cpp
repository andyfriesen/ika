/*
Python interface for input.
*/

#include "common/misc.h"
#include "ObjectDefs.h"
#include "input.h"
#include "main.h"

namespace Script
{
    namespace Input
    {
        PyTypeObject type;
        PyMappingMethods mappingmethods;

        PyMethodDef methods[] =
        {
            {   "Update",           (PyCFunction)Input_Update,   METH_NOARGS   },
            {   0   }
        };

        // proto
        PyObject* Input_GetControl(InputObject* self, PyObject* key);

#define GET(x) PyObject* get ## x(InputObject* self)
        GET(Up)     { return Input_GetControl(self, PyString_FromString("UP"));     }
        GET(Down)   { return Input_GetControl(self, PyString_FromString("DOWN"));   }
        GET(Left)   { return Input_GetControl(self, PyString_FromString("LEFT"));   }
        GET(Right)  { return Input_GetControl(self, PyString_FromString("RIGHT"));  }
        GET(Enter)  { return Input_GetControl(self, PyString_FromString("RETURN")); }
        GET(Cancel) { return Input_GetControl(self, PyString_FromString("ESCAPE")); }
#undef GET

        PyGetSetDef properties[] =
        {
            {   "up",       (getter)getUp,      0,  "Gets the standard \"Up\" control."     },
            {   "down",     (getter)getDown,    0,  "Gets the standard \"Down\" control."   },
            {   "left",     (getter)getLeft,    0,  "Gets the standard \"Left\" control."   },
            {   "right",    (getter)getRight,   0,  "Gets the standard \"Right\" control."  },
            {   "enter",    (getter)getEnter,   0,  "Gets the standard \"Enter\" control."  },
            {   "cancel",   (getter)getCancel,  0,  "Gets the standard \"Cancel\" control." },
            {   0   }
        };

        void Init()
        {
            memset(&type, 0, sizeof type);

            mappingmethods.mp_length = 0;
            mappingmethods.mp_subscript = (binaryfunc)&Input_GetControl;
            mappingmethods.mp_ass_subscript = 0;

            type.ob_refcnt = 1;
            type.ob_type = &PyType_Type;
            type.tp_name = "Input";
            type.tp_basicsize = sizeof type;
            type.tp_dealloc = (destructor)Destroy;
            type.tp_methods = methods;
            type.tp_getset = properties;
            type.tp_as_mapping = &mappingmethods;
            type.tp_doc = "Interface for hardware input devices. (such as the keyboard and mouse)";

            PyType_Ready(&type);
        }
        PyObject* New(::Input& i)
        {
            InputObject* input=PyObject_New(InputObject,&type);

            if (!input)
                return NULL;

            input->input = &i;

            return (PyObject*)input;
        }

        void Destroy(InputObject* self)
        {
            PyObject_Del(self);
        }

        PyObject* Input_Update(InputObject* self)
        {
            //self->input->Update();
            engine->CheckMessages();

            Py_INCREF(Py_None);
            return Py_None;
        }

        PyObject* Input_GetControl(InputObject* self, PyObject* key)
        {
            try
            {
                const char* name = PyString_AsString(key);
                if (!name)
                    throw "Non-string passed as input control name.";

                PyObject* obj = Script::Control::New(*self->input, name);

                if (!obj)
                    throw va("%s is not a valid input control name.", name);

                return obj;
            }
            catch (const char* s)
            {
                PyErr_SetString(PyExc_SyntaxError, s);
                return 0;
            }
        }

    }
}
