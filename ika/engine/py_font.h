/*
	Python font object
*/

#include "font.h"

struct v_FontObject
{
	PyObject_HEAD
	CFont* pFont;
};

PyMethodDef CScriptEngine::font_methods[] =
{
	{	"Print",		CScriptEngine::font_print,			1	},
	{	"CenterPrint",	CScriptEngine::font_centerprint,	1	},
	{	"RightPrint",	CScriptEngine::font_rightprint,		1	},
	{	"StringWidth",	CScriptEngine::font_stringwidth,	1	},
	{	NULL,			NULL	}
};

PyTypeObject CScriptEngine::fonttype;

void CScriptEngine::Init_Font()
{
	ZeroMemory(&fonttype,sizeof fonttype);

	fonttype.ob_refcnt=1;
	fonttype.ob_type=&PyType_Type;
	fonttype.tp_name="Font";
	fonttype.tp_basicsize=sizeof fonttype;
	fonttype.tp_dealloc=(destructor)Font_Destroy;
	fonttype.tp_getattr=(getattrfunc)Font_GetAttribute;
}

PyObject* CScriptEngine::Font_New(PyObject* self,PyObject* args)
{
	char* sFilename;

	if (!PyArg_ParseTuple(args,"s:New_Font",&sFilename))
		return NULL;

	v_FontObject* font=PyObject_New(v_FontObject,&fonttype);
	if (!font)
		return NULL;

	font->pFont=new CFont();
	bool bResult=font->pFont->LoadFNT(sFilename);
	if (!bResult)
	{
		PyErr_SetString(PyExc_OSError,va("Failed to load %s",sFilename));
		return NULL;
	}

	return (PyObject*)font;
}

void CScriptEngine::Font_Destroy(PyObject* self)
{
	((v_FontObject*)self)->pFont->Free();
	delete ((v_FontObject*)self)->pFont;

	PyObject_Del(self);
}

PyObject* CScriptEngine::Font_GetAttribute(PyObject* self,char* name)
{
	if (!strcmp(name,"width"))
		return PyInt_FromLong(((v_FontObject*)self)->pFont->Width());
	if (!strcmp(name,"height"))
		return PyInt_FromLong(((v_FontObject*)self)->pFont->Height());
	if (!strcmp(name,"tabsize"))
		return PyInt_FromLong(((v_FontObject*)self)->pFont->TabSize());

	return Py_FindMethod(font_methods,self,name);
}

PyObject* CScriptEngine::Font_SetAttribute(PyObject* self,char* name,PyObject* value)
{
	if (!strcmp(name,"tabsize"))
		((v_FontObject*)self)->pFont->SetTabSize(PyInt_AsLong(value));

	// FIXME:
	return 0;
}

METHOD(font_print)
{
	int x,y;
	char* sMessage;

	if (!PyArg_ParseTuple(args,"iis:Font.Print",&x,&y,&sMessage))
		return NULL;

	((v_FontObject*)self)->pFont->PrintString(x,y,sMessage);

	Py_INCREF(Py_None);
	return Py_None;
}

METHOD(font_centerprint)
{
	int x,y;
	char* sMessage;

	if (!PyArg_ParseTuple(args,"iis:Font.CenterPrint",&x,&y,&sMessage))
		return NULL;

	CFont* f=((v_FontObject*)self)->pFont;
	f->PrintString(x- f->StringWidth(sMessage)/2 ,y,sMessage);

	Py_INCREF(Py_None);
	return Py_None;
}

METHOD(font_rightprint)
{
	int x,y;
	char* sMessage;

	if (!PyArg_ParseTuple(args,"iis:Font.RightPrint",&x,&y,&sMessage))
		return NULL;

	CFont* f=((v_FontObject*)self)->pFont;
	f->PrintString(x- f->StringWidth(sMessage) ,y,sMessage);

	Py_INCREF(Py_None);
	return Py_None;
}

METHOD(font_stringwidth)
{
	char* sMessage;

	if (!PyArg_ParseTuple(args,"s:Font.Width",&sMessage))
		return NULL;

	return PyInt_FromLong(((v_FontObject*)self)->pFont->StringWidth(sMessage));
}