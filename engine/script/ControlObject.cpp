#include "ObjectDefs.h"
#include "input.h"
#include <map>

namespace Script
{
    namespace Control
    {
        // A simple way to make sure that multiple control objects aren't created for the same control.
        std::map<std::string, ControlObject*> _instances;

        PyTypeObject type;

        PyMethodDef methods[] =
        {
            {   "Pressed",  (PyCFunction)Control_Pressed,    METH_NOARGS,   
                "Control.Pressed() -> int\n\n"
                "Returns nonzero if the key has been pressed since the last time Pressed() or Delta() have been called"
            },
            {   "Position", (PyCFunction)Control_Position,   METH_NOARGS,   
                "Control.Position() -> float\n\n"
                "Returns the position of the control.  Most controls are normalized, meaning that their value is in the\n"
                "range of 0 through 1. (1 being pressed all the way, 0 being unpressed)\n\n"
                "The mouse axis are the only exception to this.  Their range is equal to the screen resolution."
            },
            {   "Delta",    (PyCFunction)Control_Delta,      METH_NOARGS,   
                "Control.Delta() -> float\n\n"
                "Returns the change in position of the control since the last time Pressed() or Delta() was called."
            },
            {   0, 0    }
        };

#define GET(x) PyObject* get ## x(ControlObject* self)
#define SET(x) PyObject* set ## x(ControlObject* self, PyObject* value)

        GET(OnPress)
        {
            PyObject* o;
            if (self->control->onPress.get())
                o = (PyObject*)self->control->onPress.get();
            else
                o = Py_None;
        
            Py_INCREF(o);
            return o;
        }

        GET(OnUnpress)
        {
            PyObject* o;
            if (self->control->onUnpress.get())
                o = (PyObject*)self->control->onUnpress.get();
            else
                o = Py_None;
        
            Py_INCREF(o);
            return o;
        }

        SET(OnPress)
        {
            self->control->onPress.set(value != Py_None ? value : 0);
            return 0;
        }

        SET(OnUnpress)
        {
            self->control->onUnpress.set(value != Py_None ? value : 0);
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
            if (_instances.count(name))
            {
                ControlObject* c = _instances[name];
                Py_INCREF(c);
                return (PyObject*)c;
            }

            ::Input::Control* c = input[name];
            if (!c)
                return 0;

            ControlObject* ctrl = PyObject_New(ControlObject, &type);
            new (ctrl) ControlObject;

            ctrl->control = c;
            ctrl->name = name;

            _instances[name] = ctrl;

            return (PyObject*)ctrl;
        }

        void Destroy(ControlObject* self)
        {
            _instances.erase(self->name);

            self->~ControlObject();

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
