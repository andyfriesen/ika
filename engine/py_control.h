
#include "input.h"

namespace Script
{
    namespace Control
    {
        PyTypeObject controltype;

        struct v_ControlObject
        {
            PyObject_HEAD
            Input::Control* control;
        };

        PyObject* Control_New(Input& input, const char* name)
        {
            Input::Control* c = input[name];
            if (!c)
                return 0;

            v_ControlObject* ctrl = PyObject_New(v_ControlObject, &controltype);
            ctrl->control = c;

            return (PyObject*)ctrl;
        }

        void Control_Destroy(PyObject* self)
        {
            v_ControlObject* c = (v_ControlObject*)self;
            c->control->Release();
            PyObject_Del(self);
        }

        PyObject* Control_Pressed(v_ControlObject* self)
        {
            return PyInt_FromLong(self->control->Pressed() ? 1 : 0);
        }

        PyObject* Control_Position(v_ControlObject* self)
        {
            return PyFloat_FromDouble(self->control->Position());
        }

        PyObject* Control_Delta(v_ControlObject* self)
        {
            return PyFloat_FromDouble(self->control->Delta());
        }

        PyMethodDef controlmethods[] =
        {
            {   "Pressed",  (PyCFunction)Control_Pressed,    METH_NOARGS },
            {   "Position", (PyCFunction)Control_Position,   METH_NOARGS },
            {   "Delta",    (PyCFunction)Control_Delta,      METH_NOARGS },
            {   0, 0    }
        };

        PyObject* Control_GetAttribute(PyObject* self, char* name)
        {
            return Py_FindMethod(controlmethods, self, name);
        }

        int Control_SetAttribute(PyObject* self, char* name, PyObject* value)
        {
            return -1;
        }

        void Init_Control()
        {
            memset(&controltype, 0, sizeof controltype);

            controltype.ob_refcnt = 1;
            controltype.ob_type = &PyType_Type;
            controltype.tp_name = "Control";
            controltype.tp_basicsize = sizeof controltype;
            controltype.tp_dealloc = (destructor)Control_Destroy;
            controltype.tp_getattr = (getattrfunc)Control_GetAttribute;
            controltype.tp_setattr = (setattrfunc)Control_SetAttribute;
            controltype.tp_doc = "Represents a single button or axis. (such as a key, or a joystick button)";
        }

    }
}