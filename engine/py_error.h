/*
error handling stuff
*/

#include "fileio.h"

struct v_ErrorObject
{
    PyObject_HEAD
};

PyMethodDef CScriptEngine::error_methods[] =
{
    {	"write",	CScriptEngine::error_write,	1	},
    {	NULL,	NULL	}
};

PyTypeObject CScriptEngine::errortype;

void CScriptEngine::Init_Error()
{
    ZeroMemory(&errortype,sizeof errortype);
    
    errortype.ob_refcnt=1;
    errortype.ob_type=&PyType_Type;
    errortype.tp_name="ErrorHandler";
    errortype.tp_basicsize=sizeof errortype;
    errortype.tp_dealloc=(destructor)Error_Destroy;
    errortype.tp_getattr=(getattrfunc)Error_GetAttribute;
    
    remove("pyout.log");
    
    // replace stdout and stderr with our error object
    PyObject* pSysmodule=PyImport_ImportModule("sys");
    if (!pSysmodule)            {   Log::Write("Could not create sys module."); return; }
    
    PyObject* pSysdict=PyModule_GetDict(pSysmodule);
    if (!pSysdict)		{   Log::Write("Could not init sys module.");   return; }
    
    pErrorhandler=Error_New();
    PyDict_SetItemString(pSysdict,"stdout",pErrorhandler);
    PyDict_SetItemString(pSysdict,"stderr",pErrorhandler);
    
    Py_DECREF(pSysmodule);
}

PyObject* CScriptEngine::Error_New()
{
    return (PyObject*)PyObject_New(v_ErrorObject,&errortype);	
}

void CScriptEngine::Error_Destroy(PyObject* self)
{
    PyObject_Del(self);
}

PyObject* CScriptEngine::Error_GetAttribute(PyObject* self,char* name)
{
    return Py_FindMethod(error_methods,self,name);
}

METHOD(error_write)
{
    const char* msg;
    
    if (!PyArg_ParseTuple(args,"s:Error.Write",&msg))
        return NULL;
    
    File f;
    
    f.OpenAppend("pyout.log");
    f.Write(msg,strlen(msg));
    f.Close();
    
    Py_INCREF(Py_None);
    return Py_None;
}