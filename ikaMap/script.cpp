#include "script.h"
#include <cassert>
#include <stdexcept>
#include <stdio.h>
#include "misc.h"
#include "fileio.h"

Script::Script(const std::string& fileName)
    : module(0)
    , onMouseDown(0)
    , onMouseUp(0)
    , onMouseMove(0)
    , onMouseWheel(0)

    , onRender(0)
    , onRenderCurrentLayer(0)
    , onBeginState(0)
    , onEndState(0)
    , onSwitchLayers(0)
    , onActivated(0)
{
    struct Local
    {
        static void GetSymbol(PyObject* dict, PyObject*& object, const char* name)
        {
            object = PyDict_GetItemString(dict, name);
            if (object)
            {
                if (!PyCallable_Check(object))
                    object = 0;
            }

            Py_XINCREF(object);
        }
    };

    // All this trouble just to import something from an arbitrary file.
    // Christ.
    uint s = max<int>(fileName.rfind('/'), fileName.rfind('\\')) + 1;
    uint e = s;
    std::string name;
    do
    {
        char c = fileName[++e];
        if (!(
            (c >= 'a' && c <= 'z') ||
            (c >= 'A' && c <= 'Z') ||
            (c >= '0' && c <= '9')
            ))
        {
            name = fileName.substr(s, e - s);
            break;
        }
    } while (e < fileName.length());

    if (name.empty())
        throw std::runtime_error(va("Could not parse module name from %s", fileName.c_str()));

    // Got a name for the module now.  woo.

    File sourceFile;
    sourceFile.OpenRead(fileName.c_str(), false);
    std::string source = sourceFile.ReadAll();
    sourceFile.Close();

    PyObject* code = Py_CompileString(const_cast<char*>(source.c_str()), fileName.c_str(), Py_file_input);
    if (!code)
    {
        PyErr_Print();
        throw std::runtime_error("code error?");
    }
    module = PyImport_ExecCodeModule(const_cast<char*>(name.c_str()), code);
    Py_DECREF(code);

    // FINALLY got the module loaded now.  Time to gank the relevant methods from it.
   
    PyObject* dict = PyModule_GetDict(module);
    assert(dict);

    Local::GetSymbol(dict, onMouseDown,   "OnMouseDown");
    Local::GetSymbol(dict, onMouseUp,     "OnMouseUp");
    Local::GetSymbol(dict, onMouseMove,   "OnMouseMove");
    Local::GetSymbol(dict, onMouseWheel,  "OnMouseWheel");
    Local::GetSymbol(dict, onRender,      "OnRender");
    Local::GetSymbol(dict, onRenderCurrentLayer, "OnRenderCurrentLayer");
    Local::GetSymbol(dict, onBeginState,  "OnBeginState");
    Local::GetSymbol(dict, onEndState,    "OnEndState");
    Local::GetSymbol(dict, onSwitchLayers, "OnSwitchLayers");
    Local::GetSymbol(dict, onActivated,   "OnActivated");

    // Meta!
    _name = name;
    PyObject* docString = PyDict_GetItemString(dict, "__doc__");
    if (docString != Py_None)
        _desc = PyString_AsString(docString);
    else
        _desc = "No description available";
}

Script::~Script()
{
    Py_XDECREF(onMouseDown);
    Py_XDECREF(onMouseUp);
    Py_XDECREF(onMouseMove);
    Py_XDECREF(onMouseWheel);
    Py_XDECREF(onRender);
    Py_XDECREF(onRenderCurrentLayer);
    Py_XDECREF(onBeginState);
    Py_XDECREF(onEndState);
    Py_XDECREF(onSwitchLayers);
    Py_XDECREF(onActivated);
    Py_XDECREF(module);
}

void Script::OnMouseDown(int x, int y)
{
    if (!onMouseDown)   return;

    PyObject* args = Py_BuildValue("(ii)", x, y);
    PyObject* result = PyObject_CallObject(onMouseDown, args);
    Py_DECREF(args);

    // TODO: error checking
    if (!result)    PyErr_Print();
    Py_XDECREF(result);
}

void Script::OnMouseUp(int x, int y)
{
    if (!onMouseUp)   return;

    PyObject* args = Py_BuildValue("(ii)", x, y);
    PyObject* result = PyObject_CallObject(onMouseUp, args);
    Py_DECREF(args);

    // TODO: error checking
    if (!result)    PyErr_Print();
    Py_XDECREF(result);
}

void Script::OnMouseMove(int x, int y)
{
    if (!onMouseMove)   return;

    PyObject* args = Py_BuildValue("(ii)", x, y);
    PyObject* result = PyObject_CallObject(onMouseMove, args);
    Py_DECREF(args);

    // TODO: error checking
    if (!result)    PyErr_Print();
    Py_XDECREF(result);
}

void Script::OnMouseWheel(int x, int y, int wheelDelta)
{
    if (!onMouseWheel)   return;

    PyObject* args = Py_BuildValue("(iii)", x, y, wheelDelta);
    PyObject* result = PyObject_CallObject(onMouseWheel, args);
    Py_DECREF(args);

    // TODO: error checking
    if (!result)    PyErr_Print();
    Py_XDECREF(result);
}

void Script::OnRender()
{
    if (!onRender)  return;

    PyObject* result = PyObject_CallObject(onRender, 0);
    if (!result)    PyErr_Print();
    Py_XDECREF(result);
    // TODO: error checking
}

void Script::OnRenderCurrentLayer()
{
    if (!onRenderCurrentLayer) return;

    PyObject* result = PyObject_CallObject(onRenderCurrentLayer, 0);
    if (!result)    PyErr_Print();
    Py_XDECREF(result);
    // TODO: error checking
}

void Script::OnBeginState()
{
    if (!onBeginState)  return;
    
    PyObject* result = PyObject_CallObject(onBeginState, 0);
    if (!result)    PyErr_Print();
    Py_XDECREF(result);
    // TODO: error checking
}

void Script::OnEndState()
{
    if (!onEndState) return;
    
    PyObject* result = PyObject_CallObject(onEndState, 0);
    if (!result)    PyErr_Print();
    Py_XDECREF(result);
    // TODO: error checking
}

void Script::OnSwitchLayers(uint oldLayer, uint newLayer)
{
    if (!onSwitchLayers)    return;

    PyObject* args = Py_BuildValue("(ii)", oldLayer, newLayer);
    PyObject* result = PyObject_CallObject(onSwitchLayers, args);
    Py_DECREF(args);
    if (!result)    PyErr_Print();
    Py_XDECREF(result);
    // TODO: error checking
}

void Script::OnActivated()
{
    if (!onActivated) return;
    PyObject* result = PyObject_CallObject(onActivated, 0);
    if (!result)    PyErr_Print();
    Py_XDECREF(result);
    // TODO: error checking
}

std::string Script::GetName() const
{
    return _name;
}

std::string Script::GetDesc() const
{
    return _desc;
}