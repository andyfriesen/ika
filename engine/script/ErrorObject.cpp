/*
 *  A little Python object that takes the place of sys.stdout and stderr, redirecting the
 *  two to pyout.log instead.
 */

#include "ObjectDefs.h"
#include "common/fileio.h"
#include "common/log.h"

namespace Script {
    namespace Error {
        int softspace = 1;

        PyObject obj;
        PyTypeObject type;

        PyMethodDef methods[] =
        {
            {   "write",    Error_Write,    1   },
            {   0   }
        };

        PyObject* getSoftSpace(PyObject* self);
        PyObject* setSoftSpace(PyObject* self, PyObject* value);

        PyGetSetDef properties[] =
        {
            { (char*)"softspace", (getter)getSoftSpace, (setter)setSoftSpace, "blah." },
            {   0   }
        };

        void Init() {
            memset(&type, 0, sizeof type);

            obj.ob_refcnt = 1;
            obj.ob_type = &PyType_Type;
            type.tp_name = "ErrorHandler";
            type.tp_basicsize = sizeof type;
            type.tp_dealloc = (destructor)Destroy;
            type.tp_methods = methods;
            type.tp_getset = properties;
            PyType_Ready(&type);

            // replace stdout and stderr with our error object
            PyObject* pSysmodule = PyImport_ImportModule("sys");
            if (pSysmodule == 0) {
                Log::Write("Could not get sys module.");
                return;
            }
            PyObject* pSysdict = PyModule_GetDict(pSysmodule);
            if (pSysdict == 0) {
                Log::Write("Could not init sys module.");
                return;
            }

            PyObject* errorHandler = New();
            PyDict_SetItemString(pSysdict, "stdout", errorHandler);
            PyDict_SetItemString(pSysdict, "stderr", errorHandler);

            Py_DECREF(errorHandler);
            Py_DECREF(pSysmodule);
        }

        PyObject* New() {
            return (PyObject*)PyObject_New(PyObject, &type);
        }

        void Destroy(PyObject* self) {
            PyObject_Del(self);
        }

#define METHOD(x) PyObject* x(PyObject* /*self*/, PyObject* args)

        METHOD(Error_Write) {
            const char* msg;

            if (!PyArg_ParseTuple(args, "s:Error.Write", &msg))
                return NULL;

            Log::Writen("%s", msg);
            pyOutput << msg;

            Py_INCREF(Py_None);
            return Py_None;
        }

#undef METHOD

        PyObject* getSoftSpace(PyObject* /*self*/) {
            return PyLong_FromLong(softspace);
        }

        PyObject* setSoftSpace(PyObject* /*self*/, PyObject* value) {
            softspace = PyObject_IsTrue(value);
            return 0;
        }
    }
}
