#include "Python.h"
#include <cassert>

#include "script.h"

#include "common/fileio.h"
#include "common/log.h"
#include "common/version.h"

#include "main.h"
#include "input.h"
#include "script/ObjectDefs.h"

using namespace Script;

bool ScriptEngine::_inited = false;

void ScriptEngine::Init(Engine* njin) {
    assert(!_inited);
    _inited = true;

    Py_Initialize();
    PyRun_SimpleString("import sys; sys.path.insert(0, '.')");  // :x

    PyImport_AddModule("ika");
    PyObject* module = Py_InitModule3("ika", Script::standard_methods,
        "ika standard module. \n"
        "\n"
        "Interface to the ika game engine.\n"
        );

    engine = njin;

    // Initialize objects
    Script::Image::Init();
    Script::Entity::Init();
    Script::Music::Init();
    Script::Sound::Init();
    Script::Font::Init();
    Script::Canvas::Init();
    Script::Control::Init();
    Script::InputDevice::Init();
    Script::Keyboard::Init();
    Script::Mouse::Init();
    Script::Joystick::Init();
    // singletons
    Script::Video::Init();
    Script::Colours::Init();
    Script::Map::Init();
    Script::Tileset::Init();
    Script::Input::Init();
    Script::Error::Init();

    // Create singletons
    PyObject* input = Script::Input::New();
    PyObject* map = Script::Map::New();
    PyObject* tileset = Script::Tileset::New();
    PyObject* video = Script::Video::New(engine->video);

    // We don't need to decref here because we should be increffing as we add the objects.  So
    // we're basically "moving" the reference
    PyModule_AddObject(module, "Input", input);
    PyModule_AddObject(module, "Map",   map);
    PyModule_AddObject(module, "Tileset",   tileset);
    PyModule_AddObject(module, "Video", video);

    PyModule_AddIntConstant(module, "Opaque", 0);
    PyModule_AddIntConstant(module, "Matte", 1);
    PyModule_AddIntConstant(module, "AlphaBlend", 2);
    PyModule_AddIntConstant(module, "AddBlend", 3);
    PyModule_AddIntConstant(module, "SubtractBlend", 4);

    PyModule_AddObject(module, "Version", PyString_FromString(IKA_VERSION));

    Py_INCREF(&Script::Entity::type);   PyModule_AddObject(module, "Entity", (PyObject*)&Script::Entity::type);
    Py_INCREF(&Script::Font::type);     PyModule_AddObject(module, "Font",  (PyObject*)&Script::Font::type);
    Py_INCREF(&Script::Canvas::type);   PyModule_AddObject(module, "Canvas", (PyObject*)&Script::Canvas::type);
    Py_INCREF(&Script::Image::type);    PyModule_AddObject(module, "Image", (PyObject*)&Script::Image::type);
    Py_INCREF(&Script::Music::type);    PyModule_AddObject(module, "Music", (PyObject*)&Script::Music::type);
    Py_INCREF(&Script::Sound::type);    PyModule_AddObject(module, "Sound", (PyObject*)&Script::Sound::type);

    // Create entity dictionary
    entityDict = PyDict_New();
    assert(entityDict != 0);
}

void ScriptEngine::Shutdown() {
    assert(_inited);

    while (!ScriptObject::_instances.empty()) {
        ScriptObject* o = *ScriptObject::_instances.begin();
        ScriptObject::_instances.erase(o);
        o->release();
    }

    Py_XDECREF(entityDict);
    Py_XDECREF(cameraTarget);

    Py_XDECREF(sysModule);
    Py_XDECREF(mapModule);

    Py_Finalize();
}

bool ScriptEngine::LoadSystemScripts(const std::string& /*fname*/) {
    Py_XDECREF(sysModule);                                              // free it if it's already allocated

    sysModule = PyImport_ImportModule("system");
    if (sysModule == 0) {
        PyErr_Print();
        return false;
    }

    return true;
}

bool ScriptEngine::LoadMapScripts(const std::string& fname) {
    Py_XDECREF(mapModule);

    // nuke the extension, drop the trailing dot
    std::string moduleName = Path::replaceExtension(fname, "");
    assert(moduleName[moduleName.length() - 1] == '.');
    moduleName.erase(moduleName.length() - 1);

    // replace path delimiters with dots, so python will search for a package with the correct path.
    for (uint i = 0; i < moduleName.length(); i++) {
        if (Path::delimiters.find(moduleName[i]) != std::string::npos) {
            moduleName[i] = '.';
        }
    }

    mapModule = PyImport_ImportModule(const_cast<char*>(moduleName.c_str()));

    if (mapModule == 0) {
        PyErr_Print();
        return false;
    }

    // Now to execute an AutoExec function, if one exists
    PyObject* mapDict = PyModule_GetDict(mapModule);
    PyObject* autoExecFunc = PyDict_GetItemString(mapDict, "AutoExec");

    if (autoExecFunc == 0) {
        return true; // No AutoExec?  No problem!
    }

    PyObject* result = PyEval_CallObject(autoExecFunc, 0);

    if (result == 0) {
        PyErr_Print();
        engine->Script_Error();
    }

    Py_XDECREF(result);

    return true;
}

void ScriptEngine::ExecObject(const ScriptObject& func) {
    CDEBUG("ScriptEngine::ExecObject");

    if (func.get() == 0) {
        Log::Write("Attempt to call null object");
        return;
    }

    PyObject* result = PyEval_CallObject((PyObject*)func.get(), 0);
    if (result == 0) {
        PyErr_Print();
        engine->Script_Error();
    }

    Py_DECREF(result);
}

void ScriptEngine::ExecObject(const ScriptObject& func, const ::Entity* ent) {
    CDEBUG("ScriptEngine::ExecObject");

    Script::Entity::EntityObject* entObject =
		Script::Entity::instances[const_cast< ::Entity*>(ent)];
    assert(entObject);

    if (func.get() == 0) {
        Log::Write("Attempt to call null object");
        return;
    }

    PyObject* args = Py_BuildValue("(O)", entObject);
    PyObject* result = PyEval_CallObject((PyObject*)func.get(), args);
    Py_DECREF(args);

    if (result == 0) {
        PyErr_Print();
        engine->Script_Error();
    }

    Py_DECREF(result);
}

void ScriptEngine::ExecObject(const ScriptObject& func, const ::Entity* ent, int x, int y, uint frame) {
    CDEBUG("ScriptEngine::ExecObject");

    Script::Entity::EntityObject* entObject =
		Script::Entity::instances[const_cast< ::Entity*>(ent)];
    assert(entObject != 0);

    if (func.get() == 0) {
        Log::Write("Attempt to call null object");
        return;
    }

    PyObject* args = Py_BuildValue("(Oiii)", entObject, x, y, frame);
    PyObject* result = PyEval_CallObject((PyObject*)func.get(), args);
    Py_DECREF(args);

    if (result == 0) {
        PyErr_Print();
        engine->Script_Error();
    }

    Py_DECREF(result);
}

ScriptObject ScriptEngine::GetObjectFromMapScript(const std::string& name) {
    assert(mapModule != 0);

    PyObject* dict = PyModule_GetDict(mapModule);
    assert(dict != 0);

    PyObject* func = PyDict_GetItemString(dict, name.c_str());
    // func may be 0, which is fine.

    return ScriptObject(func);
}

void ScriptEngine::ClearEntityList() {
    if (!Script::entityDict)
        return;

    PyDict_Clear(Script::entityDict);
}

void ScriptEngine::AddEntityToList(::Entity* e) {
    PyObject* pEnt = Script::Entity::New(e);                // make an object for the entity

    PyDict_SetItemString(Script::entityDict, const_cast<char*>(e->name.c_str()), pEnt);

    Py_DECREF(pEnt);
}

void ScriptEngine::CallScript(const std::string& name) {
    CDEBUG("ScriptEngine::CallScript");

    if (mapModule == 0)
        return;                                                                // no module loaded == no event

    the< ::Input>()->Flush();

    PyObject* dict = PyModule_GetDict(mapModule);
    PyObject* func = PyDict_GetItemString(dict, const_cast<char*>(name.c_str()));

    if (func == 0) {
        Log::Write("CallScript, no such event \"%s\"", name.c_str());
        return;                                                                // no such event
    }

    PyObject* result = PyEval_CallObject(func, 0);

    if (result == 0) {
        PyErr_Print();
        engine->Script_Error();
    }

    Py_XDECREF(result);
}

void ScriptEngine::CallScript(const std::string& name, const ::Entity* ent) {
    assert(Script::Entity::instances.count(const_cast< ::Entity*>(ent)));

    Script::Entity::EntityObject* entObject =
		Script::Entity::instances[const_cast< ::Entity*>(ent)];

    PyObject* dict = PyModule_GetDict(mapModule);
    PyObject* func = PyDict_GetItemString(dict, const_cast<char*>(name.c_str()));

    if (func == 0) {
        Log::Write("CallScript:  No event '%s'", name.c_str());
        return;
    }

    PyObject* args = Py_BuildValue("(O)", entObject);
    PyObject* result = PyObject_CallObject(func, args);
    Py_DECREF(args);

    if (result == 0) {
        PyErr_Print();
        engine->Script_Error();
    }
    else
        Py_DECREF(result);
}

std::string ScriptEngine::GetErrorMessage() {
    return Script::pyOutput.str();
}
