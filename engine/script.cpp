#include <cassert>
#include <Python.h>

#include "script.h"

#include "common/fileio.h"
#include "common/log.h"

#include "main.h"
#include "script/ObjectDefs.h"

using namespace Script;

bool ScriptEngine::_inited = false;

void ScriptEngine::Init(CEngine* p)
{
    assert(!_inited);
    _inited = true;
    
    Py_Initialize();
    PyRun_SimpleString("import sys; sys.path.insert(0, '.')");  // :x
    
    PyImport_AddModule("ika");
    PyObject* module = Py_InitModule3("ika", Script::standard_methods,
        "ika standard module. \n"
        "\n"
        "Contains functions and crap for manipulating the ika game engine at runtime.\n"
        );
    
    engine = p;   // I cannot express the sheer gayness behind this.
    
    // Initialize objects
    Script::Image::Init();
    Script::Entity::Init();
    Script::Sound::Init();
    Script::Font::Init();
    Script::Canvas::Init();
    Script::Control::Init();
    // singletons
    Script::Video::Init();
    Script::Map::Init();
    Script::Input::Init();
    Script::Error::Init();
    
    // Create singletons
    PyObject* input = Script::Input::New(engine->input);    
    PyObject* map = Script::Map::New();
    PyObject* video = Script::Video::New(engine->video);

    // We don't need to decref here because we should be increffing as we add the objects.  So
    // we're basically "moving" the reference
    PyModule_AddObject(module, "Input", input);
    PyModule_AddObject(module, "Map",   map);
    PyModule_AddObject(module, "Video", video);

    PyModule_AddIntConstant(module, "Opaque", 0);
    PyModule_AddIntConstant(module, "Matte", 1);
    PyModule_AddIntConstant(module, "AlphaBlend", 2);
    PyModule_AddIntConstant(module, "AddBlend", 3);
    PyModule_AddIntConstant(module, "SubtractBlend", 4);

    PyModule_AddIntConstant(module, "Nothing", mc_nothing);
    PyModule_AddIntConstant(module, "Wander", mc_wander);
    PyModule_AddIntConstant(module, "WanderRect", mc_wanderrect);
    PyModule_AddIntConstant(module, "WanderZone", mc_wanderzone);
    PyModule_AddIntConstant(module, "Scripted", mc_script);
    PyModule_AddIntConstant(module, "Chase", mc_chase);

    Py_INCREF(&Script::Image::type);    PyModule_AddObject(module, "Image", (PyObject*)&Script::Image::type);
    Py_INCREF(&Script::Entity::type);   PyModule_AddObject(module, "Entity", (PyObject*)&Script::Entity::type);
    Py_INCREF(&Script::Sound::type);    PyModule_AddObject(module, "Sound", (PyObject*)&Script::Sound::type);
    Py_INCREF(&Script::Font::type);     PyModule_AddObject(module, "Font",  (PyObject*)&Script::Font::type);
    Py_INCREF(&Script::Canvas::type);   PyModule_AddObject(module, "Canvas", (PyObject*)&Script::Canvas::type);
    
    // Create entity dictionary
    entityDict=PyDict_New();
    assert(entityDict != 0);
}

void ScriptEngine::Shutdown()
{
    assert(_inited);

    while (!ScriptObject::_instances.empty())
    {
        ScriptObject* o = *ScriptObject::_instances.begin();
        ScriptObject::_instances.erase(o);
        o->release();
    }
    
    Py_XDECREF(entityDict);
    Py_XDECREF(cameraTarget);
    
    Py_XDECREF(errorHandler);
    Py_XDECREF(sysModule);
    Py_XDECREF(mapModule);
    
    Py_Finalize();
}

bool ScriptEngine::LoadSystemScripts(const std::string& fname)
{
    Py_XDECREF(sysModule);                                              // free it if it's already allocated
    
    sysModule = PyImport_ImportModule("system");
    if (!sysModule)
    {
        PyErr_Print();
        return false;
    }
    
    return true;
}

bool ScriptEngine::LoadMapScripts(const std::string& fname)
{
    Py_XDECREF(mapModule);
    
    std::string sTemp = fname;
    
    int nExtension = sTemp.find_last_of(".", sTemp.length());
    sTemp.erase(nExtension, sTemp.length());                             // nuke the extension
    
    mapModule = PyImport_ImportModule((char*)sTemp.c_str());
    
    if (!mapModule)
    {
        PyErr_Print();
        return false;
    }

    // Now to execute an AutoExec function, if one exists
    PyObject* pDict = PyModule_GetDict(mapModule);
    PyObject* pFunc = PyDict_GetItemString(pDict, "AutoExec");

    if (!pFunc)
        return true; // No AutoExec?  No problem!

    PyObject* result = PyEval_CallObject(pFunc, 0);

    if (!result)
        Log::Write("Warning: Module %s had an AutoExec event, but it failed to execute.", sTemp.c_str());

    Py_XDECREF(result);
    
    return true;
}

bool ScriptEngine::ExecObject(const ScriptObject& func)
{
    CDEBUG("ScriptEngine::ExecObject");
    
    if (!func.get())
        return false;
    
    PyObject* result = PyEval_CallObject((PyObject*)func.get(), 0); 
    if (!result)
    {
        PyErr_Print();
        return false;
    }
    
    Py_DECREF(result);
    
    return true;
}

void ScriptEngine::ClearEntityList()
{
    if (!Script::entityDict)
        return;

    PyDict_Clear(Script::entityDict);

    for (std::map<::Entity*, Script::Entity::EntityObject*>::iterator iter = Script::Entity::instances.begin();
        iter != Script::Entity::instances.end();
        iter++)
    {
        Log::Write("%s still exists, with refcount %i", iter->first->name.c_str(), iter->second->ob_refcnt);
    }
}

void ScriptEngine::AddEntityToList(::Entity* e)
{
    PyObject* pEnt = Script::Entity::New(e);                // make an object for the entity

    PyDict_SetItemString(Script::entityDict, const_cast<char*>(e->name.c_str()), pEnt);
    
    Py_DECREF(pEnt);
}

void ScriptEngine::CallScript(const std::string& name)
{
    CDEBUG("ScriptEngine::CallScript");
    
    if (!mapModule)
        return;                                                                // no module loaded == no event

    engine->input.Unpress();
    
    PyObject* pDict=PyModule_GetDict(mapModule);
    PyObject* pFunc=PyDict_GetItemString(pDict, const_cast<char*>(name.c_str()));
    
    if (!pFunc)
    {
        Log::Write("CallScript, no such event \"%s\"", name);
        return;                                                                // no such event
    }

    PyObject* result=PyEval_CallObject(pFunc, 0);
    
    if (!result)
    {
        PyErr_Print();
        engine->Script_Error();
    }
    
    Py_XDECREF(result);
}

void ScriptEngine::CallScript(const std::string& name, const ::Entity* ent)
{
    if (!Script::Entity::instances.count(const_cast<::Entity*>(ent)))
    {
        Log::Write("ScriptEngine::CallScript(name, ent) weirdness. (this should never actually happen)");
        return;
    }

    Script::Entity::EntityObject* entObject = Script::Entity::instances[const_cast<::Entity*>(ent)];

    PyObject* dict = PyModule_GetDict(mapModule);
    PyObject* func = PyDict_GetItemString(dict, const_cast<char*>(name.c_str()));

    if (!func)
    {
        Log::Write("CallScript:  No event '%s'", name.c_str());
        return;
    }

    PyObject* args = Py_BuildValue("(O)", entObject);
    PyObject* result = PyObject_CallObject(func, args);
    Py_DECREF(entObject);
    Py_DECREF(args);

    if (!result)
    {
        PyErr_Print();
        engine->Script_Error();
    }
    else
        Py_DECREF(result);
}

//-

std::set<ScriptObject*> ScriptObject::_instances;

ScriptObject::ScriptObject(void* o)
    : _object(o)
{
    Py_XINCREF((PyObject*)o);
    _instances.insert(this);
}

ScriptObject::ScriptObject(const ScriptObject& rhs)
    : _object(rhs._object)
{
    Py_XINCREF((PyObject*)_object);
    _instances.insert(this);
}

ScriptObject::~ScriptObject()
{
    Py_XDECREF((PyObject*)_object);

    _instances.erase(this);
}

void* ScriptObject::get() const
{
    return _object;
}

void ScriptObject::set(void* o)
{
    Py_XDECREF((PyObject*)_object);
    _object = o;
    Py_XINCREF((PyObject*)o);
}

void ScriptObject::release()
{
    Py_XDECREF((PyObject*)_object);
    _object = 0;
}