/*
Python interface for input.
*/

#include "input.h"

namespace Script
{
    namespace Input
    {

        struct v_InputObject
        {
            PyObject_HEAD
            ::Input* input;
        };

        PyTypeObject inputtype;
        PyMappingMethods mappingmethods;

        PyObject* Input_New(::Input& i)
        {
            v_InputObject* input=PyObject_New(v_InputObject,&inputtype);

            if (!input)
                return NULL;

            input->input = &i;

            return (PyObject*)input;
        }

        void Input_Destroy(PyObject* self)
        {
            PyObject_Del(self);
        }

        PyObject* Input_Update(v_InputObject* self)
        {
            //self->input->Update();

            Py_INCREF(Py_None);
            return Py_None;
        }

        PyObject* Input_GetControl(v_InputObject* self, PyObject* key)
        {
            try
            {
                const char* name = PyString_AsString(key);
                if (!name)
                    throw "input[] must be accessed with a string";

                PyObject* obj = Script::Control::Control_New(*self->input, name);

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

        PyMethodDef input_methods[] =
        {
            {   "Update",           (PyCFunction)Input_Update,   1   },
            {   NULL,NULL   }
        };

        PyObject* Input_GetAttribute(PyObject* self,char* name)
        {
            return Py_FindMethod(input_methods,self,name);
        }

        /*int CScriptEngine::Input_SetAttribute(PyObject* self,char* name,PyObject* value)
        {
        return -1;
        }*/

        void Init_Input()
        {
            memset(&inputtype, 0, sizeof inputtype);

            mappingmethods.mp_length = 0;
            mappingmethods.mp_subscript = (binaryfunc)&Input_GetControl;
            mappingmethods.mp_ass_subscript = 0;

            inputtype.ob_refcnt = 1;
            inputtype.ob_type = &PyType_Type;
            inputtype.tp_name = "Input";
            inputtype.tp_basicsize = sizeof inputtype;
            inputtype.tp_dealloc = (destructor)Input_Destroy;
            inputtype.tp_getattr = (getattrfunc)Input_GetAttribute;
            //inputtype.tp_setattr = (setattrfunc)Input_SetAttribute;
            inputtype.tp_as_mapping = &mappingmethods;
            inputtype.tp_doc = "Interface for hardware input devices. (such as the keyboard and mouse)";
        }
    }
}