#include "scriptengine.h"
//#include "mainwindow.h"

#include "script/ObjectDefs.h"

namespace
{
    bool _initted = false;
    MainWindow* _mainWnd = 0;
}

void ScriptEngine::Init(MainWindow* mainWnd)
{
    _mainWnd = mainWnd;
}

void ScriptEngine::Init()
{
    if (_initted)
        return;

    _initted = true;
    Py_Initialize();

    PyImport_AddModule("ikamap");
    PyObject* ikaModule = Py_InitModule3("ikamap", ScriptObject::standard_methods,
        "ikaMap standard module\n"
        "\n"
        "Contains functions for tweaking ikaMap, and the map it is editing."
        );

    ScriptObject::Map::Init();
    ScriptObject::Editor::Init();
    ScriptObject::Error::Init();

    PyObject* mapObject = ScriptObject::Map::New(_mainWnd);
    PyObject* editorObject = ScriptObject::Editor::New(_mainWnd);

    PyModule_AddObject(ikaModule, "Map", mapObject);
    PyModule_AddObject(ikaModule, "Editor", editorObject);
}

void ScriptEngine::ShutDown()
{
    if (_initted)
        Py_Finalize();
    _initted = false;
}