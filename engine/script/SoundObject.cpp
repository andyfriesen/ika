#include "ObjectDefs.h"
#include "audiere.h"
#include "common/log.h"

#include "common/fileio.h"
#include "common/utility.h"
#include "sound.h"

namespace Script {
    namespace Sound {
        PyTypeObject type;

        PyMethodDef methods[] = {
            {   "Play",     (PyCFunction)Sound_Play,  METH_NOARGS,
                "Sound.Play()\n\n"
                "Plays the sound effect."
            },
            {   "Pause",    (PyCFunction)Sound_Pause, METH_NOARGS,
                "Sound.Pause()\n\n"
                "Pauses the sound effect.  Calling Sound.Play() will cause playback to start over."
            },
            {   0, 0   }
        };

#define GET(x) PyObject* get ## x(SoundObject* self)
#define SET(x) PyObject* set ## x(SoundObject* self, PyObject* value)

        GET(Volume)     { return PyFloat_FromDouble(self->sound->getVolume()); }
        GET(Pan)        { return PyFloat_FromDouble(self->sound->getPan()); }
        GET(PitchShift) { return PyFloat_FromDouble(self->sound->getPitchShift()); }
        SET(Volume)     { self->sound->setVolume(float(PyFloat_AsDouble(value)));       return 0;   }
        SET(Pan)        { self->sound->setPan(float(PyFloat_AsDouble(value)));          return 0;   }
        SET(PitchShift) { self->sound->setPitchShift(float(PyFloat_AsDouble(value)));   return 0;   }

#undef GET
#undef SET

        PyGetSetDef properties[] = {
            {   "volume",       (getter)getVolume,      (setter)setVolume,      "The volume of the sound effect.  Ranges from 0 to 1, with 1 being full volume."   },
            {   "pan",          (getter)getPan,         (setter)setPan,         "Panning.  -1 is left.  1 is right.  0 is centre."   },
            {   "pitchshift",   (getter)getPitchShift,  (setter)setPitchShift,  "Pitch shift.  1.0 is normal, I think.  2.0 being double the frequency.  I think.  TODO: document this after testing" },
            {   0   }
        };

        void Init() {
            memset(&type, 0, sizeof type);

            type.ob_refcnt = 1;
            type.ob_type = &PyType_Type;
            type.tp_name = "Sound";
            type.tp_basicsize = sizeof type;
            type.tp_dealloc = (destructor)Destroy;
            type.tp_methods = methods;
            type.tp_getset = properties;
            type.tp_doc = "A sound effect.  Unlike Music, Sounds can be played multiple times at once.";
            type.tp_new = New;

            PyType_Ready(&type);
        }

        PyObject* New(PyTypeObject* type, PyObject* args, PyObject* kw) {
            char* keywords[] = { "filename" , 0};
            char* filename;

            if (!PyArg_ParseTupleAndKeywords(args, kw, "s:newsound", keywords, &filename))
                return NULL;

            SoundObject* sound;

            try {
                if (!File::Exists(filename)) {
                    throw va("%s does not exist", filename);
                }

                sound = PyObject_New(SoundObject, type);
                if (!sound) {
                    throw va("Can't load %s due to internal Python weirdness!  Very Bad!", filename);
                }

                sound->sound = ::Sound::OpenSoundEffect(filename);
                if (!sound->sound) {
                    throw va("Failed to load %s", filename);
                }
            }
            catch(const char* s) {
                PyErr_SetString(PyExc_IOError, s);
                return 0;
            }

            return (PyObject*)sound;
        }

        void Destroy(SoundObject* self) {
            self->sound->unref();
            PyObject_Del(self);
        }

#define METHOD(x) PyObject* x(SoundObject* self)

        METHOD(Sound_Play) {
            self->sound->play();

            Py_INCREF(Py_None);
            return Py_None;
        }

        METHOD(Sound_Pause) {
            self->sound->stop();

            Py_INCREF(Py_None);
            return Py_None;
        }

#undef METHOD
    }
}
