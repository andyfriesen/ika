/*
	Python music stuff
*/

#include "sound.h"

struct v_MusicObject
{
	PyObject_HEAD

	SMusic	pMusic;
};

PyMethodDef CScriptEngine::music_methods[] =
{
	{	"Play",		CScriptEngine::music_play,	1	},
	{	"Pause",	CScriptEngine::music_pause,	1	},
	{	NULL,		NULL	}									// end of list
};

PyTypeObject CScriptEngine::musictype;

void CScriptEngine::Init_Music()
{
	ZeroMemory(&musictype,sizeof musictype);

	musictype.ob_refcnt=1;
	musictype.ob_type=&PyType_Type;
	musictype.tp_name="Music";
	musictype.tp_basicsize=sizeof musictype;
	musictype.tp_dealloc=(destructor)Music_Destroy;
	musictype.tp_getattr=(getattrfunc)Music_GetAttribute;
	musictype.tp_setattr=(setattrfunc)Music_SetAttribute;
}

PyObject* CScriptEngine::Music_New(PyObject* self,PyObject* args)
{
	char* filename;

	if (!PyArg_ParseTuple(args,"s:newmusic",&filename))
		return NULL;

	v_MusicObject* music=PyObject_New(v_MusicObject,&musictype);
	if (!music)
		return NULL;

	music->pMusic=sfxLoadMusic(filename);
	if (music->pMusic==0)
	{
		PyErr_SetString(PyExc_OSError,va("Failed to load %s",filename));
		return NULL;
	}

	return (PyObject*)music;
}

void CScriptEngine::Music_Destroy(PyObject* self)
{
	sfxFreeMusic(((v_MusicObject*)self)->pMusic);
	PyObject_Del(self);
}

PyObject* CScriptEngine::Music_GetAttribute(PyObject* self,char* name)
{
	if (!strcmp(name,"volume"))
		return PyInt_FromLong(sfxGetMusicVolume(((v_MusicObject*)self)->pMusic));
	if (!strcmp(name,"position"))
		return PyInt_FromLong(sfxGetMusicPos(((v_MusicObject*)self)->pMusic));

	return Py_FindMethod(music_methods,self,name);
}

int CScriptEngine::Music_SetAttribute(PyObject* self,char* name,PyObject* value)
{
	if (!strcmp(name,"volume"))
		sfxSetMusicVolume(((v_MusicObject*)self)->pMusic,PyInt_AsLong(value));
	if (!strcmp(name,"position"))
		sfxSetMusicPos(((v_MusicObject*)self)->pMusic,PyInt_AsLong(value));

	return 0;	// FIXME: you can't dynamically add properties with this in place.  I need to find Py_FindMethod's counterpart.
}

METHOD(music_play)
{
	if (!PyArg_ParseTuple(args,""))
		return NULL;

	sfxPlayMusic(((v_MusicObject*)self)->pMusic);

	Py_INCREF(Py_None);
	return Py_None;
}

METHOD(music_pause)
{
	if (!PyArg_ParseTuple(args,""))
		return NULL;

	sfxStopMusic(((v_MusicObject*)self)->pMusic);

	Py_INCREF(Py_None);
	return Py_None;
}