#include "script.h"

#include "common/fileio.h"
#include "common/log.h"

#include "python_stdlib.h"
#include "py_image.h"
#include "py_entity.h"
#include "py_sound.h"
#include "py_font.h"
#include "py_video.h"
#include "py_map.h"
#include "py_input.h"
#include "py_error.h"

#include "main.h"

CEngine* CScriptEngine::pEngine = 0;
PyObject* CScriptEngine::pEntitydict = 0;
PyObject* CScriptEngine::pPlayerent = 0;
PyObject* CScriptEngine::pCameratarget = 0;
PyObject* CScriptEngine::pErrorhandler = 0;

PyObject* CScriptEngine::pSysmodule = 0;
PyObject* CScriptEngine::pMapmodule = 0;

void CScriptEngine::Init(CEngine* p)
{
    bInited=true;
    
    Py_Initialize();
    
    string s(".;");                                             // The delimiter is platform dependant.
    s.append(Py_GetPath());
    PySys_SetPath((char*)s.c_str());
    
    PyImport_AddModule("ika");
    PyObject* module=Py_InitModule3("ika", standard_methods,
        "ika standard module. \n"
        "\n"
        "Contains functions and crap for manipulating the ika game engine at runtime.\n"
        );
    PyObject* dict  =PyModule_GetDict(module);
    
    pEngine=p;                                                  // urk
    
    // Initialize objects
    Init_Image();
    Init_Entity();
    Init_Sound();
    Init_Font();
    // singletons
    Init_Map();
    Init_Input();
    Init_Error();
    
    // Create singletons
    PyObject* input=Input_New();
    PyDict_SetItemString(dict, "Input", input);
    Py_DECREF(input);
    
    PyObject* map=Map_New();
    PyDict_SetItemString(dict, "Map", map);
    Py_DECREF(map);

    PyObject* video = Video_New();
    PyDict_SetItemString(dict, "Video", video);
    Py_DECREF(video);
    
    // Create entity dictionary
    pEntitydict=PyDict_New();
    if (!pEntitydict)
        Log::Write("!pEntitydict");
}

void CScriptEngine::Shutdown()
{
    if (!bInited)
        return;
    
    // Clear hooks
    std::list<void*>::iterator i;
    for (i=pEngine->pHookretrace.begin(); i!=pEngine->pHookretrace.end(); i++)
        Py_XDECREF((PyObject*)*i);
    pEngine->pHookretrace.Clear();
    
    for (i=pEngine->pHooktimer.begin(); i!=pEngine->pHooktimer.end(); i++)
        Py_XDECREF((PyObject*)*i);
    pEngine->pHooktimer.Clear();
    
    Py_XDECREF(pEntitydict);
    Py_XDECREF(pCameratarget);
    
    Py_XDECREF(pErrorhandler);
    Py_XDECREF(pSysmodule);
    Py_XDECREF(pMapmodule);
    
    Py_Finalize();
}

bool CScriptEngine::LoadSystemScripts(char* fname)
{
    Py_XDECREF(pSysmodule);                                            // free it if it's already allocated
    
    pSysmodule=PyImport_ImportModule("system");
    if (!pSysmodule)
    {
        PyErr_Print();
        return false;
    }
    
    return true;
}

bool CScriptEngine::LoadMapScripts(const char* fname)
{
    Py_XDECREF(pMapmodule);
    
    string sTemp=fname;
    
    int nExtension=sTemp.find_last_of(".", sTemp.length());
    sTemp.erase(nExtension, sTemp.length());                             // nuke the extension
    
    pMapmodule=PyImport_ImportModule((char*)sTemp.c_str());
    
    if (!pMapmodule)
    {
        PyErr_Print();
        return false;
    }

    // Now to execute an AutoExec function, if one exists
    PyObject* pDict=PyModule_GetDict(pMapmodule);
    PyObject* pFunc=PyDict_GetItemString(pDict, "AutoExec");

    if (!pFunc)
        return true; // No AutoExec?  No problem!

    PyObject* result=PyEval_CallObject(pFunc, 0);

    if (!result)
        Log::Write("Warning: Module %s had an AutoExec event, but it could not execute.", fname);

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
    if (!pEntitydict)
        return;
    
    // Wipe the entity list clean
    PyObject* pKeys=PyDict_Keys(pEntitydict);
    
    for (int i = 0; i<PyDict_Size(pEntitydict); i++)
        PyDict_DelItem(pEntitydict, PyList_GetItem(pKeys, i));
    
    Py_DECREF(pKeys);
}

void CScriptEngine::AddEntityToList(CEntity* e)
{
    PyObject* pEnt=Entity_New(e);                // make an object for the entity

    char c[1024];
    strcpy(c, e->sName.c_str());
  
    PyDict_SetItemString(pEntitydict, c, pEnt);
    
    Py_DECREF(pEnt);
}

void CScriptEngine::CallEvent(const char* sName)
{
    CDEBUG("CScriptEngine::CallEvent");
    
    if (!pMapmodule)
        return;                                                                // no module loaded == no event
    
    PyObject* pDict=PyModule_GetDict(pMapmodule);
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
        pEngine->Script_Error();
    }
    
    Py_XDECREF(result);
}
