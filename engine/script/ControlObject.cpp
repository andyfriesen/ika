#include "ObjectDefs.h"
#include "Input.h"

namespace Script
{
    namespace Control
    {
        PyTypeObject type;

        PyMethodDef methods[] =
        {
            {   "Pressed",  (PyCFunction)Control_Pressed,    METH_NOARGS },
            {   "Position", (PyCFunction)Control_Position,   METH_NOARGS },
            {   "Delta",    (PyCFunction)Control_Delta,      METH_NOARGS },
            {   0, 0    }
        };

#define GET(x) PyObject* get ## x(ControlObject* self)
#define SET(x) PyObject* set ## x(ControlObject* self, PyObject* value)

        GET(OnPress)
        {
            PyObject* o;
            if (self->control->onPress)
                o = (PyObject*)self->control->onPress;
            else
                o = Py_None;
        
            Py_INCREF(o);
            return o;
        }

        GET(OnUnpress)
        {
            PyObject* o;
            if (self->control->onUnpress)
                o = (PyObject*)self->control->onUnpress;
            else
                o = Py_None;
        
            Py_INCREF(o);
            return o;
        }

        SET(OnPress)
        {
            Py_XDECREF((PyObject*)self->control->onPress);
            if (value != Py_None)
            {
                Py_INCREF(value);
                self->control->onPress = value;
            }
            else
                self->control->onPress = 0;
            return 0;
        }
        SET(OnUnpress)
        {
            Py_XDECREF((PyObject*)self->control->onUnpress);
            if (value != Py_None)
            {
                Py_INCREF(value);
                self->control->onUnpress = value;
            }
            else
                self->control->onUnpress = 0;
            return 0;
        }
#undef GET
#undef SET

        PyGetSetDef properties[] =
        {
            {   "onpress",      (getter)getOnPress,     (setter)setOnPress,     "Gets or sets the function to be executed when the key is pressed."   },
            {   "onunpress",    (getter)getOnUnpress,   (setter)setOnUnpress,   "Gets or sets the function to be executed when the key is released."  },
            {   0   }
        };

        void Init()
        {
            memset(&type, 0, sizeof type);

            type.ob_refcnt = 1;
            type.ob_type = &PyType_Type;
            type.tp_name = "Control";
            type.tp_basicsize = sizeof type;
            type.tp_dealloc = (destructor)Destroy;
            type.tp_methods = methods;
            type.tp_getset = properties;
            type.tp_doc = "Represents a single button or axis. (such as a key, or a joystick button)";

            PyType_Ready(&type);
        }
 
        PyObject* New(::Input& input, const char* name)
        {
            ::Input::Control* c = input[name];
            if (!c)
                return 0;

            ControlObject* ctrl = PyObject_New(ControlObject, &type);
            ctrl->control = c;

            return (PyObject*)ctrl;
        }

        void Destroy(ControlObject* self)
        {
            PyObject_Del(self);
        }

        PyObject* Control_Pressed(ControlObject* self)
        {
            return PyInt_FromLong(self->control->Pressed() ? 1 : 0);
        }

        PyObject* Control_Position(ControlObject* self)
        {
            return PyFloat_FromDouble(self->control->Position());
        }

        PyObject* Control_Delta(ControlObject* self)
        {
            return PyFloat_FromDouble(self->control->Delta());
        }
   }
}