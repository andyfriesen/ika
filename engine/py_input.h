/*
Python interface for input.
*/

#include "input.h"

struct v_InputObject
{
    PyObject_HEAD
};

PyMethodDef CScriptEngine::input_methods[] =
{
    {	"Update",			CScriptEngine::input_update,	1	},
    {	"Button",			CScriptEngine::input_button,	1	},
    {	"SetButton",		CScriptEngine::input_setbutton,	1	},
    {	"NextPressed",		CScriptEngine::input_nextpressed,1	},
    {	"SetNextPressed",	CScriptEngine::input_setnextpressed,1},
    {	"MouseState",		CScriptEngine::input_mousestate,1	},
    {	"SetMouseState",	CScriptEngine::input_setmousestate,1},
    {	"ClipMouse",		CScriptEngine::input_clipmouse,	1	},
    {	"BindKey",			CScriptEngine::input_bindkey,	1	},
    {	"UnbindKey",		CScriptEngine::input_unbindkey,	1	},
    {	NULL,NULL	}
};

PyTypeObject CScriptEngine::inputtype;

void CScriptEngine::Init_Input()
{
    ZeroMemory(&inputtype,sizeof inputtype);
    
    inputtype.ob_refcnt=1;
    inputtype.ob_type=&PyType_Type;
    inputtype.tp_name="Input";
    inputtype.tp_basicsize=sizeof inputtype;
    inputtype.tp_dealloc=(destructor)Input_Destroy;
    inputtype.tp_getattr=(getattrfunc)Input_GetAttribute;
    inputtype.tp_setattr=(setattrfunc)Input_SetAttribute;
}

PyObject* CScriptEngine::Input_New()
{
    v_InputObject* input=PyObject_New(v_InputObject,&inputtype);
    
    if (!input)
        return NULL;
    
    return (PyObject*)input;
}

void CScriptEngine::Input_Destroy(PyObject* self)
{
    PyObject_Del(self);
}

PyObject* CScriptEngine::Input_GetAttribute(PyObject* self,char* name)
{
    if (!strcmp(name,"up"))
        return PyInt_FromLong(pEngine->input.up);
    if (!strcmp(name,"down"))
        return PyInt_FromLong(pEngine->input.down);
    if (!strcmp(name,"left"))
        return PyInt_FromLong(pEngine->input.left);
    if (!strcmp(name,"right"))
        return PyInt_FromLong(pEngine->input.right);
    if (!strcmp(name,"enter"))
        return PyInt_FromLong(pEngine->input.enter);
    if (!strcmp(name,"cancel"))
        return PyInt_FromLong(pEngine->input.cancel);
    
    return Py_FindMethod(input_methods,self,name);
}

int CScriptEngine::Input_SetAttribute(PyObject* self,char* name,PyObject* value)
{
    if (!strcmp(name,"up"))
        pEngine->input.up=PyInt_AsLong(value)!=0;
    if (!strcmp(name,"down"))
        pEngine->input.down=PyInt_AsLong(value)!=0;
    if (!strcmp(name,"left"))
        pEngine->input.left=PyInt_AsLong(value)!=0;
    if (!strcmp(name,"right"))
        pEngine->input.right=PyInt_AsLong(value)!=0;
    if (!strcmp(name,"enter"))
        pEngine->input.enter=PyInt_AsLong(value)!=0;
    if (!strcmp(name,"cancel"))
        pEngine->input.cancel=PyInt_AsLong(value)!=0;
    
    return 0;
}

METHOD(input_update)
{
    pEngine->CheckMessages();
    
    if (!PyArg_ParseTuple(args,""))
        return NULL;
    
    pEngine->input.Update();
    pEngine->input.ClearControls();
    
    Py_INCREF(Py_None);
    return Py_None;
}

METHOD(input_button)
{
    int nButton;
    
    if (!PyArg_ParseTuple(args,"i:Input.Button",&nButton))
        return NULL;
    
    return PyInt_FromLong(pEngine->input.control[nButton]);
}

METHOD(input_setbutton)
{
    int nButton,nState;
    
    if (!PyArg_ParseTuple(args,"ii:Input.SetButton",&nButton,&nState))
        return NULL;
    
    pEngine->input.control[nButton]=nState?1:0;
    
    Py_INCREF(Py_None);
    return Py_None;
}

METHOD(input_nextpressed)
{
    if (!PyArg_ParseTuple(args,""))
        return NULL;
    
    char s[255];
    char c=pEngine->input.GetKey();
    
    c=pEngine->input.Scan2ASCII(c);
    
    if (c)
        sprintf(s,"%c",c);
    else
        strcpy(s,"");
    
    return PyString_FromString(s);
}

METHOD(input_setnextpressed)
{
    int nKey;
    
    if (!PyArg_ParseTuple(args,"i:Input.SetNextPressed",&nKey))
        return NULL;
    
    pEngine->input.StuffKey(nKey);
    
    Py_INCREF(Py_None);
    return Py_None;
}

METHOD(input_mousestate)
{
    if (!PyArg_ParseTuple(args,""))
        return NULL;
    
    int mx,my,b1,b2,b3;
    
    mx=pEngine->input.mousex;
    my=pEngine->input.mousey;
    b1=pEngine->input.mouseb&1;
    b2=(pEngine->input.mouseb>>1)&1;
    b3=(pEngine->input.mouseb>>2)&1;
    
    /*	PyObject* rvalue=PyTuple_New(5);	// x,y and three mouse buttons
    
      PyTuple_SetItem(rvalue,0,PyInt_FromLong(mx));
      PyTuple_SetItem(rvalue,1,PyInt_FromLong(my));
      PyTuple_SetItem(rvalue,2,PyInt_FromLong(b1));
      PyTuple_SetItem(rvalue,3,PyInt_FromLong(b2));
    PyTuple_SetItem(rvalue,4,PyInt_FromLong(b3));*/
    
    PyObject* rvalue=Py_BuildValue("iiiii",mx,my,b1,b2,b3);
    
    return rvalue;
}

METHOD(input_setmousestate)
{
    int mx,my,b1,b2,b3;
    
    if (!PyArg_ParseTuple(args,"iiiii:Input.SetMouseState",&mx,&my,&b1,&b2,&b3))
        return NULL;
    
    b1=b1?1:0;
    b2=b2?2:0;
    b3=b3?4:0;
    
    pEngine->input.mousex=mx;
    pEngine->input.mousey=my;
    pEngine->input.mouseb=b1 | b2 | b3;
    
    Py_INCREF(Py_None);
    return Py_None;
}

METHOD(input_clipmouse)
{
    int x1,y1,x2,y2;
    
    if (!PyArg_ParseTuple(args,"iiii:Input.ClipMouse",&x1,&y1,&x2,&y2))
        return NULL;
    
    pEngine->input.ClipMouse(x1,y1,x2,y2);
    
    Py_INCREF(Py_None);
    return Py_None;
}

METHOD(input_bindkey)
{
    int nKey,nControl;
    
    if (!PyArg_ParseTuple(args,"ii:Input.BindKey",&nKey,&nControl))
        return NULL;
    
    pEngine->input.BindKey(nControl,nKey);
    
    Py_INCREF(Py_None);
    return Py_None;
}

METHOD(input_unbindkey)
{
    int nKey;
    
    if (!PyArg_ParseTuple(args,"i:Input.UnbindKey",&nKey))
        return NULL;
    
    pEngine->input.UnbindKey(nKey);
    
    Py_INCREF(Py_None);
    return Py_None;
}
