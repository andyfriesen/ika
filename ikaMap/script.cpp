#include "script.h"
#include <cassert>
#include <stdexcept>
#include <stdio.h>
#include "misc.h"
#include "fileio.h"
#include "script/ObjectDefs.h" // for ScriptObject::Error

#include "scriptengine.h" // geh

#include "log.h"

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

    , _fileName(fileName)
{
    ScriptEngine::Init(); // init python, if it hasn't already been initted.

    Reload();

    // Get the module name from the filename
    // blech
    const uint s = max<int>(fileName.rfind('/'), fileName.rfind('\\')) + 1;
    const uint e = fileName.rfind('.');
    if (e == std::string::npos)
        _name = fileName.substr(s);
    else
        _name = fileName.substr(s, e - s);
}

Script::~Script()
{
    Deallocate();
}

void Script::OnMouseDown(int x, int y)
{
    if (!onMouseDown)   return;

    PyObject* args = Py_BuildValue("(ii)", x, y);
    PyObject* result = PyObject_CallObject(onMouseDown, args);
    Py_DECREF(args);

    if (!result)    ReportError();
    Py_XDECREF(result);
}

void Script::OnMouseUp(int x, int y)
{
    if (!onMouseUp)   return;

    PyObject* args = Py_BuildValue("(ii)", x, y);
    PyObject* result = PyObject_CallObject(onMouseUp, args);
    Py_DECREF(args);

    if (!result)    ReportError();
    Py_XDECREF(result);
}

void Script::OnMouseMove(int x, int y)
{
    if (!onMouseMove)   return;

    PyObject* args = Py_BuildValue("(ii)", x, y);
    PyObject* result = PyObject_CallObject(onMouseMove, args);
    Py_DECREF(args);

    if (!result)    ReportError();
    Py_XDECREF(result);
}

void Script::OnMouseWheel(int x, int y, int wheelDelta)
{
    if (!onMouseWheel)   return;

    PyObject* args = Py_BuildValue("(iii)", x, y, wheelDelta);
    PyObject* result = PyObject_CallObject(onMouseWheel, args);
    Py_DECREF(args);

    if (!result)    ReportError();
    Py_XDECREF(result);
}

void Script::OnRender()
{
    if (!onRender)  return;

    PyObject* result = PyObject_CallObject(onRender, 0);

    // If there is an error here, we risk infinite recursion by popping a dialog over the screen.
    // So, if there is an error, we unset the render callback so that it doesn't get executed again.
    if (!result)
    {
        Py_DECREF(onRender);
        onRender = 0;

        ReportError();
    }

    Py_XDECREF(result);
}

void Script::OnRenderCurrentLayer()
{
    if (!onRenderCurrentLayer) return;

    PyObject* result = PyObject_CallObject(onRenderCurrentLayer, 0);

    // If there is an error here, we risk infinite recursion by popping a dialog over the screen.
    // So, if there is an error, we unset the render callback so that it doesn't get executed again.
    if (!result)
    {
        Py_DECREF(onRender);
        onRender = 0;

        ReportError();
    }

    Py_XDECREF(result);
}

void Script::OnBeginState()
{
    if (!onBeginState)  return;
    
    PyObject* result = PyObject_CallObject(onBeginState, 0);
    if (!result)    ReportError();
    Py_XDECREF(result);
}

void Script::OnEndState()
{
    if (!onEndState) return;
    
    PyObject* result = PyObject_CallObject(onEndState, 0);
    if (!result)    ReportError();
    Py_XDECREF(result);
}

void Script::OnSwitchLayers(uint oldLayer, uint newLayer)
{
    if (!onSwitchLayers)    return;

    PyObject* args = Py_BuildValue("(ii)", oldLayer, newLayer);
    PyObject* result = PyObject_CallObject(onSwitchLayers, args);
    Py_DECREF(args);
    if (!result)    ReportError();
    Py_XDECREF(result);
}

void Script::OnActivated()
{
    if (!onActivated) return;
    PyObject* result = PyObject_CallObject(onActivated, 0);
    if (!result)    ReportError();
    Py_XDECREF(result);
}

/*void Script::OnKeyPress(uint keyCode)
{
    if (!onKeyPress)    return;

    PyObject* args = Py_BuildValue("(i)", keyCode);
    PyObject* result = PyObject_CallObject(onKeyPress, args);
    Py_DECREF(args);
    if (!result) ReportError();
    Py_XDECREF(result);
}
*/
std::string Script::GetName() const
{
    return _name;
}

std::string Script::GetDesc() const
{
    return _desc;
}

bool Script::IsTool() const
{
    return 
        onMouseDown             != 0 ||
        onMouseUp               != 0 ||
        onMouseMove             != 0 ||
        onMouseWheel            != 0 ||
        onBeginState            != 0 ||
        onEndState              != 0 ||
        onSwitchLayers          != 0 ||
        onRender                != 0 ||
        onRenderCurrentLayer    != 0;
}

bool Script::IsActivatable() const
{
    return onActivated != 0;
}

void Script::ReportError()
{
    PyErr_Print();
    std::string msg = "There was an error in " + _name + ".\n";
    msg += ScriptObject::pyErrors.str();
    ScriptObject::pyErrors.str("");
    throw std::runtime_error(msg);
}

void Script::Reload()
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

    Deallocate();
/*
    // All this trouble just to import something from an arbitrary file.
    // Christ.
    uint s = max<int>(_name.rfind('/'), _name.rfind('\\')) + 1;
    uint e = s;
    std::string name;
    do
    {
        char c = _name[++e];
        if (!(
            (c >= 'a' && c <= 'z') ||
            (c >= 'A' && c <= 'Z') ||
            (c >= '0' && c <= '9')
            ))
        {
            name = _name.substr(s, e - s);
            break;
        }
    } while (e < _name.length());

    if (name.empty())
        throw std::runtime_error(va("Could not parse module name from %s", _name.c_str()));
*/
    // Got a name for the module now.  woo.

    File sourceFile;
    sourceFile.OpenRead(_fileName.c_str(), false);
    std::string source = sourceFile.ReadAll();
    sourceFile.Close();

    PyObject* code = Py_CompileString(const_cast<char*>(source.c_str()), _name.c_str(), Py_file_input);
    if (!code)  ReportError();

    module = PyImport_ExecCodeModule(const_cast<char*>(_name.c_str()), code);
    Py_DECREF(code);
    if (!module) ReportError();

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
    //Local::GetSymbol(dict, onKeyPress,    "OnKeyPress");

    // Meta!
    //_name = name;
    PyObject* docString = PyDict_GetItemString(dict, "__doc__");
    if (docString != Py_None)
        _desc = PyString_AsString(docString);
    else
        _desc = "No description available";
}

void Script::Deallocate()
{
    Py_XDECREF(onMouseDown);            onMouseDown = 0;
    Py_XDECREF(onMouseUp);              onMouseUp = 0;
    Py_XDECREF(onMouseMove);            onMouseMove = 0;
    Py_XDECREF(onMouseWheel);           onMouseWheel = 0;
    Py_XDECREF(onRender);               onRender = 0;
    Py_XDECREF(onRenderCurrentLayer);   onRenderCurrentLayer = 0;
    Py_XDECREF(onBeginState);           onBeginState = 0;
    Py_XDECREF(onEndState);             onEndState = 0;
    Py_XDECREF(onSwitchLayers);         onSwitchLayers = 0;
    Py_XDECREF(onActivated);            onActivated = 0;
    Py_XDECREF(module);                 module = 0;
}