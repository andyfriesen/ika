#include "ObjectDefs.h"
#include "audiere.h"
#include "common/log.h"

#include "common/fileio.h"
#include "common/utility.h"
#include "sound.h"

namespace Script {
    namespace Music {
        PyTypeObject type;

        PyMethodDef methods[] = {
            {   (char*)"Play",     (PyCFunction)Music_Play,  METH_NOARGS,
                (char*)"Music.Play()\n\n"
                "Plays the stream."
            },
            {   (char*)"Pause",    (PyCFunction)Music_Pause, METH_NOARGS,
                (char*)"Music.Pause()\n\n"
                "Pauses the stream.  Calling Music.Play() will cause playback to resume\n"
                "where it left off."
            },
            {   0, 0   }
        };

#define GET(x) PyObject* get ## x(MusicObject* self)
#define SET(x) PyObject* set ## x(MusicObject* self, PyObject* value)

        GET(Volume)     { return PyFloat_FromDouble(self->music->getVolume()); }
        GET(Pan)        { return PyFloat_FromDouble(self->music->getPan()); }
        GET(Position)   { return PyInt_FromLong(self->music->getPosition()); }
        GET(PitchShift) { return PyFloat_FromDouble(self->music->getPitchShift()); }
        GET(Loop)       { return PyInt_FromLong(self->music->getRepeat()); }
        SET(Volume)     { self->music->setVolume((float)PyFloat_AsDouble(value));       return 0;   }
        SET(Pan)        { self->music->setPan((float)PyFloat_AsDouble(value));          return 0;   }
        SET(Position)   { self->music->setPosition(PyInt_AsLong(value));                return 0;   }
        SET(PitchShift) { self->music->setPitchShift((float)PyFloat_AsDouble(value));   return 0;   }
        SET(Loop)       { self->music->setRepeat(PyInt_AsLong(value) != 0);             return 0;   }

#undef GET
#undef SET

        PyGetSetDef properties[] = {
            {   (char*)"volume",       (getter)getVolume,      (setter)setVolume,      (char*)"The volume of the sound.  Ranges from 0 to 1, with 1 being full volume."   },
            {   (char*)"pan",          (getter)getPan,         (setter)setPan,         (char*)"Panning.  -1 is left.  1 is right.  0 is centre."   },
            {   (char*)"position",     (getter)getPosition,    (setter)setPosition,    (char*)"The chronological position of the sound, in milliseconds." },
            {   (char*)"pitchshift",   (getter)getPitchShift,  (setter)setPitchShift,  (char*)"Pitch shift.  1.0 is normal, I think.  2.0 being double the frequency.  I think.  TODO: document this after testing" },
            {   (char*)"loop",         (getter)getLoop,        (setter)setLoop,        (char*)"If nonzero, the sound loops.  If zero, then the sound stops playing when it reaches the end."  },
            {   0   }
        };

        void Init() {
            memset(&type, 0, sizeof type);

            type.ob_refcnt = 1;
            type.ob_type = &PyType_Type;
            type.tp_name = "Music";
            type.tp_basicsize = sizeof type;
            type.tp_dealloc = (destructor)Destroy;
            type.tp_methods = methods;
            type.tp_getset = properties;
            type.tp_doc = 
                "Streamed sound data.  Almost always used for music, but could be useful\n"
                "any time sound data needs to be looped or paused and resumed.";
            type.tp_new = New;

            PyType_Ready(&type);
        }

        PyObject* New(PyTypeObject* type, PyObject* args, PyObject* kw) {
            char* keywords[] = { "filename" , 0};
            char* filename;

            if (!PyArg_ParseTupleAndKeywords(args, kw, "s:__init__", keywords, &filename)) {
                return 0;
            }

            MusicObject* sound;

            if (!File::Exists(filename)) {
                PyErr_SetString(PyExc_IOError, va("%s does not exist", filename));
                return 0;
            }

            sound = PyObject_New(MusicObject, type);
            if (!sound) {
                PyErr_SetString(PyExc_RuntimeError, va("Can't load %s due to internal Python weirdness!  Very Bad!", filename));
                return 0;
            }

            sound->music = ::Sound::OpenSound(filename);
            if (!sound->music) {
                PyErr_SetString(PyExc_IOError, va("Failed to load %s", filename));
                return 0;
            }

            return (PyObject*)sound;
        }

        void Destroy(MusicObject* self) {
            self->music->unref();
            PyObject_Del(self);
        }

#define METHOD(x) PyObject* x(MusicObject* self)

        METHOD(Music_Play) {
            self->music->play();

            Py_INCREF(Py_None);
            return Py_None;
        }

        METHOD(Music_Pause) {
            self->music->stop();

            Py_INCREF(Py_None);
            return Py_None;
        }

#undef METHOD
    }
}
