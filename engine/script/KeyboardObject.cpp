
#include <cassert>
#include "ObjectDefs.h"
#include "input.h"
#include "keyboard.h"
#include "common/misc.h"

namespace Script
{
    namespace Keyboard
    {
        PyTypeObject type;

        PyMethodDef methods[] =
        {
            {   "GetKey",   (PyCFunction)Keyboard_GetKey, METH_NOARGS,
                "GetKey() -> char\n\n"
                "Returns a key from the keyboard queue, or None if the buffer\n"
                "is empty."
            },

            {   "WasKeyPressed",    (PyCFunction)Keyboard_WasKeyPressed, METH_NOARGS,
                "WasKeyPressed() -> bool\n\n"
                "Returns true if there are keys in the keyboard queue, False if not."
            },

            {   "ClearKeyQueue",    (PyCFunction)Keyboard_ClearKeyQueue, METH_NOARGS,
                "ClearKeyQueue()\n\n"
                "Clears the keyboard queue."
            },
        };

        void Init()
        {
            memset(&type, 0, sizeof type);

            type.ob_refcnt = 1;
            type.ob_type = &PyType_Type;
            type.tp_name = "Keyboard";
            type.tp_base = &Script::InputDevice::type;
            type.tp_basicsize = sizeof type;
            type.tp_dealloc = (destructor)Destroy;
            type.tp_methods = methods;
            type.tp_doc = "teh keybroad";

            PyType_Ready(&type);
        }

        PyObject* New()
        {
            Script::InputDevice::DeviceObject* keyboard = PyObject_New(Script::InputDevice::DeviceObject, &type);
            keyboard->device = the< ::Input>()->GetKeyboard();

            assert(keyboard != 0);
            return (PyObject*)keyboard;
        }

        void Destroy(PyObject* self)
        {
            PyObject_Del(self);
        }

#define METHOD(x) PyObject* x(PyObject* self)

        METHOD(Keyboard_GetKey)
        {
            char c = the< ::Input>()->GetKeyboard()->GetKey();

            if (c)
                return PyString_FromStringAndSize(&c, 1);
            else
            {
                Py_INCREF(Py_None);
                return Py_None;
            }
        }

        METHOD(Keyboard_WasKeyPressed)
        {
            PyObject* o = the< ::Input>()->GetKeyboard()->WasKeyPressed()
                ? Py_True
                : Py_False;

            Py_INCREF(o);
            return o;
        }

        METHOD(Keyboard_ClearKeyQueue)
        {
            the< ::Input>()->GetKeyboard()->ClearKeyQueue();
            Py_INCREF(Py_None);
            return Py_None;
        }

#undef METHOD
    }
}