#ifndef SCRIPTOBJECT_H
#define SCRIPTOBJECT_H

#include <set>

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

    ScriptObject& operator = (const ScriptObject& so);

    inline operator bool() const
    {
        return _object != 0;
    }

    inline bool operator == (void* p) const
    {
        return p == _object;
    }
};

#endif