
#ifndef SCRIPT_H
#define SCRIPT_H

#include <set>

#include "common/utility.h"

struct Engine;                                  // proto
class ScriptObject;
struct Entity;

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
    void Init(Engine* njin);                    // Passing the engine here is ungood.  sigh.
    void Shutdown();

    bool LoadSystemScripts(const std::string& fname);
    bool LoadMapScripts(const std::string& fname);

    void ExecObject(const ScriptObject& func);
    void ExecObject(const ScriptObject& func, const Entity* ent);       // needed for entity movescripts.  Passes the entity as an argument to the function object.

    ScriptObject GetObjectFromMapScript(const std::string& name);       // a bit verbose, but it says what it does.

    void ClearEntityList();
    void AddEntityToList(Entity* e);

    void CallScript(const std::string& name);
    void CallScript(const std::string& name, const Entity* ent);        // Calls the function, passing the equivalent Python Entity object as an argument.

    std::string GetErrorMessage();
};

#endif
