
#ifndef SCRIPT_H
#define SCRIPT_H

#include "Python.h"
#include "common/types.h"

/*!
    Python API encapsulation class.

    Due to the way the Python API is structured, the script engine simply will not support multiple instances.
    Don't even try it. ^_~

    um... I think this is technically a singleton. :o

    A very, very good idea to try would be to make a template class for all these Python objects.
    
    I hate this.  It's retarded.  Either wrap it in a namespace and live with its globalness, or
    figure out how to make it not so gay and full of static things. -_-;
*/

class CEngine;                                                // proto

class CScriptEngine
{
    string sCurmapscript;

    bool bInited;

public:
    CScriptEngine() : bInited(false) {}

    void Init(CEngine* pEngine);                            // -_-
    void Shutdown();

    bool LoadSystemScripts(char* fname);
    bool LoadMapScripts(const char* fname);

    bool ExecFunction(void* pFunc);

    void ClearEntityList();
    void AddEntityToList(class CEntity* e);

    void CallEvent(const char* sName);
};

/*
    FIXME:
    Some of hese objects can't have properties added to them at runtime.
    This is unacceptable. -_-

    Objects:

    Image            check!    (extra blits)
    Font             check!
    Entity           check!    (needs extra robustness)
    Music            check!
    Sound effects    check!
*/

#endif
