
#include "ObjectDefs.h"
#include "input.h"
#include "debug.cpp"

namespace Script {
    namespace InputDevice {
        PyObject obj;
        PyTypeObject type;
        PyMappingMethods mappingmethods;

        PyMethodDef methods[] = {
            {   "Update",   (PyCFunction)Device_Update, METH_NOARGS,
                "Update()\n\n"
                "Polls the device for changes."
            },
            {   "GetControl",   (PyCFunction)Device_GetControl, METH_VARARGS,
                "GetControl(name) -> Control\n\n"
                "Returns the control with the given name, or None if no such\n"
                "control exists."
            },

            {   0   }
        };

        PyObject* Device_Subscript(DeviceObject* self, PyObject* key);

        void Init() {
            memset(&type, 0, sizeof type);

            mappingmethods.mp_length = 0;
            mappingmethods.mp_subscript = (binaryfunc)&Device_Subscript;
            mappingmethods.mp_ass_subscript = 0;

            obj.ob_refcnt = 1;
            obj.ob_type = &PyType_Type;
            type.tp_base = 0;
            type.tp_name = "InputDevice";
            type.tp_basicsize = sizeof type;
            type.tp_dealloc = (destructor)Destroy;
            type.tp_methods = methods;
            type.tp_as_mapping = &mappingmethods;
            type.tp_doc = "Interface for hardware input devices. (such as the keyboard and mouse)\n"
                "This object is a singleton; you cannot create additional instances.";

            PyType_Ready(&type);
        }

        PyObject* New(::InputDevice* device) {
            DeviceObject* obj = PyObject_New(DeviceObject, &type);

            if (!obj)
                return 0;
            obj->device = device;
            return (PyObject*)obj;
        }

        void Destroy(DeviceObject* self) {
            PyObject_Del(self);
        }

#define METHOD(x)  PyObject* x(DeviceObject* self, PyObject* args)
#define METHOD1(x) PyObject* x(DeviceObject* self)

        METHOD1(Device_Update) {
            self->device->Update();
            Py_INCREF(Py_None);
            return Py_None;
        }

        METHOD(Device_GetControl) {
            char* s;

            if (!PyArg_ParseTuple(args, "s:GetControl", &s)) {
                return 0;
            }

            return ::Script::Control::New(self->device->GetControl(s));
        }

#undef METHOD
#undef METHOD1

        PyObject* Device_Subscript(DeviceObject* self, PyObject* key) {
            const char* name = PyBytes_AsString(key);

            if (!name) {
                DEBUG_PyERR(PyExc_KeyError, "Control names must be strings!");
                return 0;
            }

            ::InputControl* control = self->device->GetControl(name);
            if (!control) {
                DEBUG_PyERR(PyExc_KeyError, va("%s is not a valid control name", name));
                return 0;
            }

            PyObject* obj = Script::Control::New(control);

            if (!obj) {
                throw "Internal Python weirdness!";
            }

            return obj;
        }
    }
}
