#include "scriptobject.h"
#include "Python.h"

std::set<ScriptObject*> ScriptObject::_instances;

ScriptObject::ScriptObject()
    : _object(0)
{
    _instances.insert(this);
}

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
    release();

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

ScriptObject& ScriptObject::operator = (const ScriptObject& so)
{
    Py_XDECREF((PyObject*)_object);
    _object = so._object;
    Py_XINCREF((PyObject*)_object);

    return *this;
}

void ScriptObject::release()
{
    Py_XDECREF((PyObject*)_object);
    _object = 0;
}
