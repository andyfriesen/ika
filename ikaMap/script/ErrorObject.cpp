/*
 *  A little Python object that takes the place of sys.stdout and stderr, redirecting the
 *  two to pyout.log instead.
 */

#include "ObjectDefs.h"
#include "common/fileio.h"
#include "common/log.h"

namespace ScriptObject
{
    std::stringstream pyErrors;

    namespace Error
    {

        int softspace = 1;

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
            { "softspace", (getter)getSoftSpace, (setter)setSoftSpace, "blah." },
            {   0   }
        };

        void Init()
        {
            memset(&type, 0, sizeof type);

            type.ob_refcnt=1;
            type.ob_type=&PyType_Type;
            type.tp_name="ErrorHandler";
            type.tp_basicsize=sizeof type;
            type.tp_dealloc=(destructor)Destroy;
            type.tp_methods = methods;
            type.tp_getset = properties;
            PyType_Ready(&type);

            remove("pyout.log");

            // replace stdout and stderr with our error object
            PyObject* pSysmodule=PyImport_ImportModule("sys");
            if (!pSysmodule)            {   Log::Write("Could not get sys module."); return; }

            PyObject* pSysdict=PyModule_GetDict(pSysmodule);
            if (!pSysdict)      {   Log::Write("Could not init sys module.");   return; }

            PyObject* errorHandler = New();
            //PyDict_SetItemString(pSysdict, "stdout", errorHandler);
            PyDict_SetItemString(pSysdict, "stderr", errorHandler);
            Py_DECREF(errorHandler);

            Py_DECREF(pSysmodule);
        }

        PyObject* New()
        {
            return (PyObject*)PyObject_New(PyObject, &type);    
        }

        void Destroy(PyObject* self)
        {
            PyObject_Del(self);
        }

#define METHOD(x) PyObject* x(PyObject* self, PyObject* args)

        METHOD(Error_Write)
        {
            const char* msg;

            if (!PyArg_ParseTuple(args, "s:Error.Write", &msg))
                return NULL;

            /*File f;

            f.OpenAppend("pyout.log", false);
            f.Write(msg, strlen(msg));
            f.Close();*/
            Log::Writen(msg);
            pyErrors << msg;

            Py_INCREF(Py_None);
            return Py_None;
        }

#undef METHOD

        PyObject* getSoftSpace(PyObject* self)
        {
            return PyInt_FromLong(softspace);
        }

        PyObject* setSoftSpace(PyObject* self, PyObject* value)
        {
            softspace = PyObject_IsTrue(value);
            return 0;
        }

    }
}
