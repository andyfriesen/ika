
#include "audiere.h"

#include "common/FileIO.h"
#include "common/misc.h"
#include "sound.h"
#include "ObjectDefs.h"

namespace Script
{
    namespace Sound
    {
        PyTypeObject type;

        PyMethodDef methods[] =
        {
            {   "Play",     (PyCFunction)Sound_Play,  METH_NOARGS   },
            {   "Pause",    (PyCFunction)Sound_Pause, METH_NOARGS   },
            {   0, 0   }
        };

        void Init()
        {
            memset(&type, 0, sizeof type);

            type.ob_refcnt=1;
            type.ob_type=&PyType_Type;
            type.tp_name="Sound";
            type.tp_basicsize=sizeof type;
            type.tp_dealloc=(destructor)Destroy;
            type.tp_getattr=(getattrfunc)GetAttr;
            type.tp_setattr=(setattrfunc)SetAttr;
            type.tp_doc="A hunk of sound data, like a sound effect, or a piece of music.";
        }

        PyObject* New(PyObject* self, PyObject* args)
        {
            char* filename;

            if (!PyArg_ParseTuple(args, "s:newsound", &filename))
                return NULL;

            SoundObject* sound;

            try
            {
                if (!File::Exists(filename))                    throw va("%s does not exist", filename);

                sound=PyObject_New(SoundObject, &type);
                if (!sound)                                     throw va("Can't load %s due to internal Python weirdness!  Very Bad!", filename);

                sound->sound = ::Sound::OpenSound(filename);
                if (!sound->sound)                              throw va("Failed to load %s", filename);
            }
            catch(const char* s)
            {
                PyErr_SetString(PyExc_OSError, s);
                return NULL;
            }

            return (PyObject*)sound;
        }

        void Destroy(SoundObject* self)
        {
            self->sound->unref();
            PyObject_Del(self);
        }

        PyObject* GetAttr(SoundObject* self, char* name)
        {
            if (!strcmp(name, "volume"))
                return PyFloat_FromDouble(self->sound->getVolume());
            else if (!strcmp(name, "pan"))
                return PyFloat_FromDouble(self->sound->getPan());
            else if (!strcmp(name, "position"))
                return PyInt_FromLong(self->sound->getPosition());
            else if (!strcmp(name, "pitchshift"))
                return PyFloat_FromDouble(self->sound->getPitchShift());
            else if (!strcmp(name, "loop"))
                return PyInt_FromLong(self->sound->getRepeat());
            else
                return Py_FindMethod(methods, (PyObject*)self, name);
        }

        int SetAttr(SoundObject* self, char* name, PyObject* value)
        {
            if (!strcmp(name, "volume"))
                self->sound->setVolume((float)PyFloat_AsDouble(value));
            else if (!strcmp(name, "pan"))
                self->sound->setPan((float)PyFloat_AsDouble(value));
            else if (!strcmp(name, "position"))
                self->sound->setPosition(PyInt_AsLong(value));
            else if (!strcmp(name, "pitchshift"))
                self->sound->setPitchShift((float)PyFloat_AsDouble(value));
            else if (!strcmp(name, "loop"))
                self->sound->setRepeat(PyInt_AsLong(value) != 0);

            return 0;
        }

#define METHOD(x) PyObject* x(SoundObject* self)

        METHOD(Sound_Play)
        {
            self->sound->play();

            Py_INCREF(Py_None);
            return Py_None;
        }

        METHOD(Sound_Pause)
        {
            self->sound->stop();

            Py_INCREF(Py_None);
            return Py_None;
        }

#undef METHOD
    }
}