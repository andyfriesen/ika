
#include "sound.h"

/*
	TODO:

	global volume variable?
*/

struct v_SoundObject
{
	PyObject_HEAD
	Ssfx	psfx;
};

PyMethodDef CScriptEngine::sound_methods[] =
{
	{	"Play",		CScriptEngine::sound_play,		1	},
	{	NULL,		NULL	}
};

PyTypeObject CScriptEngine::soundtype;

void CScriptEngine::Init_Sound()
{
	ZeroMemory(&soundtype,sizeof soundtype);

	soundtype.ob_refcnt=1;
	soundtype.ob_type=&PyType_Type;
	soundtype.tp_name="Sound";
	soundtype.tp_basicsize=sizeof soundtype;
	soundtype.tp_dealloc=(destructor)Sound_Destroy;
	soundtype.tp_getattr=(getattrfunc)Sound_GetAttribute;
	soundtype.tp_setattr=(setattrfunc)Sound_SetAttribute;
}

PyObject* CScriptEngine::Sound_New(PyObject* self,PyObject* args)
{
	char* sFilename;

	if (!PyArg_ParseTuple(args,"s:newsound",&sFilename))
		return NULL;

	v_SoundObject* sound;

	try
	{
		if (!File::Exists(sFilename))			throw va("%s does not exist",sFilename);

		sound=PyObject_New(v_SoundObject,&soundtype);
		if (!sound)								throw va("Can't load %s due to internal Python weirdness!  Very Bad!",sFilename);

		bool bResult=sfxLoadEffect(&sound->psfx,sFilename);
		if (!bResult)							throw va("Failed to load %s",sFilename);
	}
	catch(const char* s)
	{
		PyErr_SetString(PyExc_OSError,s);
		return NULL;
	}

	return (PyObject*)sound;
}

void CScriptEngine::Sound_Destroy(PyObject* self)
{
	sfxFreeEffect(((v_SoundObject*)self)->psfx);
	PyObject_Del(self);
}

PyObject* CScriptEngine::Sound_GetAttribute(PyObject* self,char* name)
{
	return Py_FindMethod(sound_methods,self,name);
}

int CScriptEngine::Sound_SetAttribute(PyObject* self,char* name,PyObject* value)
{
	return 0;
}

METHOD(sound_play)
{
	int nVol=255;
	int nPan=128;

	if (!PyArg_ParseTuple(args,"|ii:Sound.Play",&nVol,&nPan))
		return NULL;

	sfxPlayEffect(((v_SoundObject*)self)->psfx,nVol,nPan);

	Py_INCREF(Py_None);
	return Py_None;
}
