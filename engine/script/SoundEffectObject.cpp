#include "ObjectDefs.h"
#include "audiere.h"
#include "common/log.h"

#include "common/fileio.h"
#include "common/utility.h"
#include "sound.h"

namespace Script
{
    namespace SoundEffect
    {
        PyTypeObject type;

        PyMethodDef methods[] =
        {
            {   "Play",     (PyCFunction)SoundEffect_Play,  METH_NOARGS,
                "SoundEffect.Play()\n\n"
                "Plays the sound effect."
            },
            {   "Pause",    (PyCFunction)SoundEffect_Pause, METH_NOARGS,
                "SoundEffect.Pause()\n\n"
                "Pauses the sound effect.  Calling SoundEffect.Play() will cause playback to start over."
            },
            {   0, 0   }
        };

#define GET(x) PyObject* get ## x(SoundEffectObject* self)
#define SET(x) PyObject* set ## x(SoundEffectObject* self, PyObject* value)

        GET(Volume)     { return PyFloat_FromDouble(self->soundEffect->getVolume()); }
        GET(Pan)        { return PyFloat_FromDouble(self->soundEffect->getPan()); }
        GET(PitchShift) { return PyFloat_FromDouble(self->soundEffect->getPitchShift()); }
        SET(Volume)     { self->soundEffect->setVolume(float(PyFloat_AsDouble(value)));       return 0;   }
        SET(Pan)        { self->soundEffect->setPan(float(PyFloat_AsDouble(value)));          return 0;   }
        SET(PitchShift) { self->soundEffect->setPitchShift(float(PyFloat_AsDouble(value)));   return 0;   }

#undef GET
#undef SET

        PyGetSetDef properties[] =
        {
            {   "volume",       (getter)getVolume,      (setter)setVolume,      "The volume of the sound effect.  Ranges from 0 to 1, with 1 being full volume."   },
            {   "pan",          (getter)getPan,         (setter)setPan,         "Panning.  0 is left.  2 is right.  1 is centre."   },
            {   "pitchshift",   (getter)getPitchShift,  (setter)setPitchShift,  "Pitch shift.  1.0 is normal, I think.  2.0 being double the frequency.  I think.  TODO: document this after testing" },
            {   0   }
        };

        void Init()
        {
            memset(&type, 0, sizeof type);

            type.ob_refcnt = 1;
            type.ob_type = &PyType_Type;
            type.tp_name = "SoundEffect";
            type.tp_basicsize = sizeof type;
            type.tp_dealloc = (destructor)Destroy;
            type.tp_methods = methods;
            type.tp_getset = properties;
            type.tp_doc = "A soundEffect effect.  Unlike Sounds, SoundEffects can be played multiple times at once.";
            type.tp_new = New;

            PyType_Ready(&type);
        }

        PyObject* New(PyTypeObject* type, PyObject* args, PyObject* kw)
        {
            char* keywords[] = { "filename" , 0};
            char* filename;

            if (!PyArg_ParseTupleAndKeywords(args, kw, "s:newsound", keywords, &filename))
                return NULL;

            SoundEffectObject* soundEffect;

            try
            {
                if (!File::Exists(filename)) {
                    throw va("%s does not exist", filename);
                }

                soundEffect = PyObject_New(SoundEffectObject, type);
                if (!soundEffect) {
                    throw va("Can't load %s due to internal Python weirdness!  Very Bad!", filename);
                }

                soundEffect->soundEffect = ::Sound::OpenSoundEffect(filename);
                if (!soundEffect->soundEffect) {
                    throw va("Failed to load %s", filename);
                }
            }
            catch(const char* s)
            {
                PyErr_SetString(PyExc_RuntimeError, s);
                return 0;
            }

            return (PyObject*)soundEffect;
        }

        void Destroy(SoundEffectObject* self)
        {
            self->soundEffect->unref();
            PyObject_Del(self);
        }

#define METHOD(x) PyObject* x(SoundEffectObject* self)

        METHOD(SoundEffect_Play)
        {
            self->soundEffect->play();

            Py_INCREF(Py_None);
            return Py_None;
        }

        METHOD(SoundEffect_Pause)
        {
            self->soundEffect->stop();

            Py_INCREF(Py_None);
            return Py_None;
        }

#undef METHOD
    }
}
