#include "scriptengine.h"
#include "mainwindow.h"

#include "script/ObjectDefs.h"

namespace
{
    PyObject* ikaModule;
}

void ScriptEngine::Init(MainWindow* mainWnd)
{
    Py_Initialize();

    PyImport_AddModule("ika");
    ikaModule = Py_InitModule3("ika", ScriptObject::standard_methods,
        "ikaMap standard module\n"
        "\n"
        "Contains functions for tweaking ikaMap, and the map it is editing."
        );

    ScriptObject::Map::Init();
    ScriptObject::Editor::Init();
    ScriptObject::Error::Init();

    PyObject* mapObject = ScriptObject::Map::New(mainWnd);
    PyObject* editorObject = ScriptObject::Editor::New(mainWnd);

    PyModule_AddObject(ikaModule, "Map", mapObject);
    PyModule_AddObject(ikaModule, "Editor", editorObject);
}

void ScriptEngine::ShutDown()
{

    Py_Finalize();
}