
#include "ObjectDefs.h"
#include "misc.h"
#include "log.h"
#include "../userdialog.h"

#define METHOD(x)  PyObject* x(PyObject* self, PyObject* args)
#define METHOD1(x) PyObject* x(PyObject*)

namespace ScriptObject
{
    METHOD(std_log)
    {
        char* message;

        if (!PyArg_ParseTuple(args, "s:log", &message))
            return 0;

        Log::Write(message);

        Py_INCREF(Py_None);
        return Py_None;                                    // returning void :)
    }

    METHOD(std_showdialog)
    {
        int cols;
        char* caption;
        PyObject* elements; // sequence

        if (!PyArg_ParseTuple(args, "isO:ShowDialog", &cols, &caption, &elements))
            return 0;        

        if (!PySequence_Check(elements))
        {
            PyErr_SetString(PyExc_ValueError, "Elements must be a sequence");
            return 0;
        }

        const int len = PySequence_Length(elements);
        if (len == -1)
        {
            PyErr_SetString(PyExc_ValueError, "Unable to get the length of elements list?");
            return 0;
        }

        UserDialog::ArgList argList;

        // parse argument list
        for (int i = 0; i < len; i++)
        {
            PyObject* el = PySequence_GetItem(elements, i);
            char* name;
            char* type;

            if (!PyArg_Parse(el, "(ss)", &name, &type))
            {
                Py_DECREF(el);
                PyErr_SetString(PyExc_ValueError, "Elements list should contain two-tuples of strings.  ie ('Name', 'Text')");
                return 0;
            }

            Py_DECREF(el);
            argList.push_back(UserDialog::Arg(name, type));
        }

        UserDialog::ArgList results;

        // time to actually run the dialog
        try
        {
            UserDialog dlg(cols, caption, argList);

            dlg.ShowModal();
            results = dlg.GetResults();
        }
        catch (std::runtime_error err)
        {
            PyErr_SetString(PyExc_RuntimeError, err.what());
            return 0;
        }

        // construct our return value
        // results will be empty if cancel was clicked, so we don't need special case code to handle it.
        {
            PyObject* dict = PyDict_New();
            int len = results.size();

            for (int i = 0; i < len; i++)
            {
                PyDict_SetItemString(dict, results[i].first.c_str(), PyString_FromString(results[i].second.c_str()));
            }

            return dict;
        }
    }

    METHOD(std_messagebox)
    {
        char* message;
        char* caption = "Message";

        if (!PyArg_ParseTuple(args, "s|s:MessageBox", &message, &caption))
            return 0;

        ::wxMessageBox(message, caption);

        Py_INCREF(Py_None);
        return Py_None;
    }

    /*METHOD(std_exit)
    {
        char* message="";

        if (!PyArg_ParseTuple(args, "|s:exit", &message))
            return 0;

        engine->Sys_Error(message);

        // never gets called... I hope.
        Py_INCREF(Py_None);
        return Py_None;
    }*/

    /*METHOD(std_entityat)
    {
        int x, y, width, height;
        if (!PyArg_ParseTuple(args, "iiii|EntityAt", &x, &y, &width, &height))
            return 0;

        int x2=x+width;
        int y2=y+height;

        int count = 0;
        PyObject* pKey = 0;
        PyObject* pValue = 0;
        while (PyDict_Next(entityDict, &count, &pKey, &pValue))
        {
            if (pValue->ob_type != &ScriptObject::Entity::type)
            {
                Log::Write("Someone's being a goober and putting non-entities in ika.Map.entities! >:(");
                continue;
            }

            ::Entity* ent=((ScriptObject::Entity::EntityObject*)pValue)->ent;
            
            if (x>ent->x+ent->sprite->nHotw)    continue;
            if (y>ent->y+ent->sprite->nHoth)    continue;
            if (x2<ent->x)    continue;
            if (y2<ent->y)    continue;

            Py_INCREF(pValue);
            return pValue;
        }

        Py_INCREF(Py_None);
        return Py_None;
    }*/

    PyMethodDef standard_methods[] =
    {
        //  name  | function

        // Misc
        {   "Log",          (PyCFunction)std_log,           METH_VARARGS,
            "Log(message)\n\n"
            "Writes a string to ika.log, if logging is enabled."
        },

        {   "ShowDialog",   (PyCFunction)std_showdialog,    METH_VARARGS,
            "ShowDialog(columns -> int, caption -> string, elements -> sequence) -> dict\n\n"
            "Shows a dialog to the user.  columns is the number of elements to show\n"
            "in one vertical column.\n\n"
            "Elements is a sequence containing two-tuples of strings in the format\n"
            "('name', 'type')\n"
            "Currently, the only valid type is 'edit', which creates a text box.\n\n"
            "The return value is a dict containing string results for each element\n"
            "passed.  The value depends on the type of control created.\n\n"
            "Edit boxes return their contents."
        },

        {   "MessageBox",   (PyCFunction)std_messagebox,    METH_VARARGS,
            "MessageBox(message, caption = 'Message')\n\n"
            "Displays a popup dialog box containing the message specified.\n"
            "An Ok button is placed on the box."
        },

        /*{ "Exit",           (PyCFunction)std_exit,              METH_VARARGS,
            "Exit([message])\n\n"
            "Exits ika immediately, displaying the message onscreen, if specified."
        },*/

        /*{ "EntityAt",       (PyCFunction)std_entityat,          METH_VARARGS,
            "EntityAt(x, y, width, height) -> Entity\n\n"
            "If there is an entity within the rectangle passed, it is returned.\n"
            "If not, then None is returned."
        },*/

        {    0    }
    };

#undef METHOD
#undef METHOD1
}
