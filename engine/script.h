
#ifndef SCRIPT_H
#define SCRIPT_H

#include <set>

#include "common/types.h"

class CEngine;                                  // proto
class ScriptObject;

/**
 *  Python API encapsulation class.
 *
 *  Due to the way the Python API is structured, the script engine 
 *  simply will not support multiple instances.  Don't try.
 */
class ScriptEngine
{
    static bool _inited;                        // used to assert that only one instance of this class is ever created.
public:
    void Init(CEngine* pEngine);                // -_-
    void Shutdown();

    bool LoadSystemScripts(char* fname);
    bool LoadMapScripts(const char* fname);

    bool ExecFunction(const ScriptObject& func);

    void ClearEntityList();
    void AddEntityToList(class CEntity* e);

    void CallEvent(const char* sName);
};

/// Smart pointer for holding a Python object.  I don't want the rest
/// of the engine to need to be aware of Python.  It just needs opaque,
/// copy-safe handles.
class ScriptObject
{
    friend class ScriptEngine;

protected:
    static std::set<ScriptObject*> _instances;

    void* _object;
    void release();

public:
    ScriptObject(void* o);
    ScriptObject(const ScriptObject& so);
    ~ScriptObject();

    void* get() const;
    void set(void* o);  

    inline bool operator == (void* p) const
    {
        return p == _object;
    }
};

#endif
