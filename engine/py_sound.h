
#include "audiere.h"

#include "sound.h"

/*
TODO:

  global volume variable?
*/

struct v_SoundObject
{
    PyObject_HEAD
    audiere::OutputStream* sound;
};

PyMethodDef CScriptEngine::sound_methods[] =
{
    {   "Play",     CScriptEngine::sound_play,  1   },
    {   "Pause",    CScriptEngine::sound_pause, 1   },
    {    NULL,      NULL                            }
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
    char* filename;
    
    if (!PyArg_ParseTuple(args,"s:newsound",&filename))
        return NULL;
    
    v_SoundObject* sound;
    
    try
    {
        if (!File::Exists(filename))                    throw va("%s does not exist", filename);
        
        sound=PyObject_New(v_SoundObject,&soundtype);
        if (!sound)                                     throw va("Can't load %s due to internal Python weirdness!  Very Bad!", filename);
        
        sound->sound = Sound::OpenSound(filename);
        if (!sound->sound)                              throw va("Failed to load %s", filename);
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
    ((v_SoundObject*)self)->sound->unref();
    PyObject_Del(self);
}

PyObject* CScriptEngine::Sound_GetAttribute(PyObject* self,char* name)
{
    v_SoundObject* snd = (v_SoundObject*)self;

    if (!strcmp(name,"volume"))
        return PyFloat_FromDouble(snd->sound->getVolume());
    else if (!strcmp(name,"pan"))
        return PyFloat_FromDouble(snd->sound->getPan());
    else if (!strcmp(name,"position"))
        return PyInt_FromLong(snd->sound->getPosition());
    else if (!strcmp(name,"pitchshift"))
        return PyFloat_FromDouble(snd->sound->getPitchShift());
    else if (!strcmp(name,"loop"))
        return PyInt_FromLong(snd->sound->getRepeat());
    else
        return Py_FindMethod(sound_methods,self,name);
}

int CScriptEngine::Sound_SetAttribute(PyObject* self,char* name,PyObject* value)
{
    v_SoundObject* snd = (v_SoundObject*)self;

    if (!strcmp(name,"volume"))
        snd->sound->setVolume((float)PyFloat_AsDouble(value));
    else if (!strcmp(name,"pan"))
        snd->sound->setPan((float)PyFloat_AsDouble(value));
    else if (!strcmp(name,"position"))
        snd->sound->setPosition(PyInt_AsLong(value));
    else if (!strcmp(name,"pitchshift"))
        snd->sound->setPitchShift((float)PyFloat_AsDouble(value));
    else if (!strcmp(name,"loop"))
        snd->sound->setRepeat(PyInt_AsLong(value) != 0);

    return 0;
}

METHOD(sound_play)
{
    if (!PyArg_ParseTuple(args,""))
        return NULL;
    
    ((v_SoundObject*)self)->sound->play();
    
    Py_INCREF(Py_None);
    return Py_None;
}

METHOD(sound_pause)
{
    if (!PyArg_ParseTuple(args,""))
        return NULL;
    
    ((v_SoundObject*)self)->sound->stop();
    
    Py_INCREF(Py_None);
    return Py_None;
}