#include "script.h"

#include "common/fileio.h"
#include "common/log.h"

#include "main.h"
#include "script/ObjectDefs.h"

using namespace Script;

void CScriptEngine::Init(CEngine* p)
{
    bInited=true;
    
    Py_Initialize();
    
    PyRun_SimpleString("import sys; sys.path.insert(0, '.')");

    PyImport_AddModule("ika");
    PyObject* module=Py_InitModule3("ika", Script::standard_methods,
        "ika standard module. \n"
        "\n"
        "Contains functions and crap for manipulating the ika game engine at runtime.\n"
        );
    PyObject* dict  =PyModule_GetDict(module);
    
    engine=p;   // I cannot express the sheer gayness behind this.
    
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
    PyDict_SetItemString(dict, "Input", input);
    Py_DECREF(input);
    
    PyObject* map = Script::Map::New();
    PyDict_SetItemString(dict, "Map", map);
    Py_DECREF(map);

    PyObject* video = Script::Video::New(engine->video);
    PyDict_SetItemString(dict, "Video", video);
    Py_DECREF(video);

    PyModule_AddIntConstant(module, "Opaque", 0);
    PyModule_AddIntConstant(module, "Matte", 1);
    PyModule_AddIntConstant(module, "AlphaBlend", 2);

    Py_INCREF(&Script::Image::type);    PyModule_AddObject(module, "Image", (PyObject*)&Script::Image::type);
    Py_INCREF(&Script::Entity::type);   PyModule_AddObject(module, "Entity",(PyObject*)&Script::Entity::type);
    Py_INCREF(&Script::Sound::type);    PyModule_AddObject(module, "Sound", (PyObject*)&Script::Sound::type);
    Py_INCREF(&Script::Font::type);     PyModule_AddObject(module, "Font",  (PyObject*)&Script::Font::type);
    Py_INCREF(&Script::Canvas::type);   PyModule_AddObject(module, "Canvas",(PyObject*)&Script::Canvas::type);
    
    // Create entity dictionary
    entitydict=PyDict_New();
    if (!entitydict)
        Log::Write("!entitydict");
}

void CScriptEngine::Shutdown()
{
    if (!bInited)
        return;
    
    // Clear hooks
    std::list<void*>::iterator i;
    for (i=engine->pHookretrace.begin(); i!=engine->pHookretrace.end(); i++)
        Py_XDECREF((PyObject*)*i);
    engine->pHookretrace.Clear();
    
    for (i=engine->pHooktimer.begin(); i!=engine->pHooktimer.end(); i++)
        Py_XDECREF((PyObject*)*i);
    engine->pHooktimer.Clear();

    // Clear key bindings
    for (::Input::iterator i = engine->input.begin(); i != engine->input.end(); i++)
    {
        Py_XDECREF((PyObject*)i->second->onPress);
        Py_XDECREF((PyObject*)i->second->onUnpress);
    }
    
    Py_XDECREF(entitydict);
    Py_XDECREF(cameratarget);
    
    Py_XDECREF(errorhandler);
    Py_XDECREF(sysmodule);
    Py_XDECREF(mapmodule);
    
    Py_Finalize();
}

bool CScriptEngine::LoadSystemScripts(char* fname)
{
    Py_XDECREF(sysmodule);                                            // free it if it's already allocated
    
    sysmodule=PyImport_ImportModule("system");
    if (!sysmodule)
    {
        PyErr_Print();
        return false;
    }
    
    return true;
}

bool CScriptEngine::LoadMapScripts(const char* fname)
{
    Py_XDECREF(mapmodule);
    
    string sTemp=fname;
    
    int nExtension=sTemp.find_last_of(".", sTemp.length());
    sTemp.erase(nExtension, sTemp.length());                             // nuke the extension
    
    mapmodule=PyImport_ImportModule((char*)sTemp.c_str());
    
    if (!mapmodule)
    {
        PyErr_Print();
        return false;
    }

    // Now to execute an AutoExec function, if one exists
    PyObject* pDict=PyModule_GetDict(mapmodule);
    PyObject* pFunc=PyDict_GetItemString(pDict, "AutoExec");

    if (!pFunc)
        return true; // No AutoExec?  No problem!

    PyObject* result=PyEval_CallObject(pFunc, 0);

    if (!result)
        Log::Write("Warning: Module %s had an AutoExec event, but it could not execute.", sTemp.c_str());

    Py_XDECREF(result);
    
    return true;
}

bool CScriptEngine::ExecFunction(void* pFunc)
{
    CDEBUG("CScriptEngine::ExecFunction");
    
    if (!pFunc)
        return false;
    
    PyObject* result=PyEval_CallObject((PyObject*)pFunc, 0);
    if (!result)
    {
        PyErr_Print();
        return false;
    }
    
    Py_DECREF(result);
    
    return true;
}

void CScriptEngine::ClearEntityList()
{
    if (!Script::entitydict)
        return;
    
    // Wipe the entity list clean
    PyObject* pKeys=PyDict_Keys(Script::entitydict);
    
    for (int i = 0; i<PyDict_Size(Script::entitydict); i++)
        PyDict_DelItem(Script::entitydict, PyList_GetItem(pKeys, i));
    
    Py_DECREF(pKeys);
}

void CScriptEngine::AddEntityToList(CEntity* e)
{
    PyObject* pEnt = Script::Entity::New(e);                // make an object for the entity

    PyDict_SetItemString(Script::entitydict, const_cast<char*>(e->sName.c_str()), pEnt);
    
    Py_DECREF(pEnt);
}

void CScriptEngine::CallEvent(const char* sName)
{
    CDEBUG("CScriptEngine::CallEvent");
    
    if (!mapmodule)
        return;                                                                // no module loaded == no event
    
    PyObject* pDict=PyModule_GetDict(mapmodule);
    PyObject* pFunc=PyDict_GetItemString(pDict, (char*)sName);
    
    if (!pFunc)
    {
        Log::Write("CallEvent, no such event \"%s\"", sName);
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
