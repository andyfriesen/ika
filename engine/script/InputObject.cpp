/*
Python interface for input.
*/

#include "ObjectDefs.h"
#include "common/utility.h"
#include "input.h"
#include "main.h"

namespace Script
{
    namespace Input
    {
        PyTypeObject type;
        //PyMappingMethods mappingmethods;

        PyMethodDef methods[] =
        {
            {   "Update",           (PyCFunction)Input_Update,      METH_NOARGS,
                "Update()\n\n"
                "Updates the state of the mouse, and any attached input devices.\n"
                "Also gives the OS a chance to do background tasks.  Continuous\n"
                "loops should call this occasionally to give the OS time to perform\n"
                "its tasks."
            },
            
            {   "Unpress",          (PyCFunction)Input_Unpress,     METH_NOARGS,
                "Unpress()\n\n"
                "Unsets the Pressed() property of all controls.  Has no effect on\n"
                "their positions.\n"
                "Individual controls can be unpressed by simply calling their Pressed()\n"
                "method, discarding the result."
            },

            /*{   "GetControl",       (PyCFunction)Input_GetControl,  METH_VARARGS,
                "GetControl(name -> string)\n\n"
                "Returns an object that represents the requested control.\n"
                "Input[name->string] performs the exact same function.\n"
                "ie x = ika.Input['UP'] or x = ika.Input.GetControl('UP')"
            },

            {   "GetKey",           (PyCFunction)Input_GetKey,  METH_NOARGS,
                "GetKey() -> char or None\n\n"
                "Returns the next key in the keyboard queue, or None if the queue is empty."
            },

            {   "ClearKeyQueue",    (PyCFunction)Input_ClearKeyQueue,   METH_NOARGS,
                "ClearKeyQueue()\n\n"
                "Flushes the keyboard queue.  Any keypresses that were in the queue will not\n"
                "be returned by Input.GetKey()"
            },

            {   "WasKeyPressed",    (PyCFunction)Input_WasKeyPressed,   METH_NOARGS,
                "WasKeyPressed() -> bool\n\n"
                "Returns True if there is a key in the key queue.  False if not.\n"
                "(ie Input.GetKey() will return None if the result is False)"
            },*/

            {   0   }
        };

        // proto
        //PyObject* Input_Subscript(InputObject* self, PyObject* key);

#define GET(x) PyObject* get ## x(InputObject* self)
#define SET(x) PyObject* set ## x(InputObject* self, PyObject* value)

        GET(Up)         { return Script::Control::New(the< ::Input>()->up);      }
        GET(Down)       { return Script::Control::New(the< ::Input>()->down);    }
        GET(Left)       { return Script::Control::New(the< ::Input>()->left);    }
        GET(Right)      { return Script::Control::New(the< ::Input>()->right);   }
        GET(Enter)      { return Script::Control::New(the< ::Input>()->enter);   }
        GET(Cancel)     { return Script::Control::New(the< ::Input>()->cancel);  }
        GET(Keyboard)   { Py_INCREF(self->keyboard); return self->keyboard;     }
        GET(Mouse)      { Py_INCREF(self->mouse); return self->mouse;           }
        GET(Joysticks)  { Py_INCREF(self->joysticks); return self->joysticks;   }

        SET(Up)         
        {
            Script::Control::ControlObject* o = reinterpret_cast<Script::Control::ControlObject*>(value);

            if (o->ob_type != &Script::Control::type)
                PyErr_SetString(PyExc_RuntimeError, "Standard controls can only be set to control objects!");
            else
                the< ::Input>() ->SetStandardControl(the< ::Input>()->up, o->control);
            return 0;
        }

        SET(Down)         
        {
            Script::Control::ControlObject* o = reinterpret_cast<Script::Control::ControlObject*>(value);

            if (o->ob_type != &Script::Control::type)
                PyErr_SetString(PyExc_RuntimeError, "Standard controls can only be set to control objects!");
            else
                the< ::Input>()->SetStandardControl(the< ::Input>()->down, o->control);
            return 0;
        }

        SET(Left)         
        {
            Script::Control::ControlObject* o = reinterpret_cast<Script::Control::ControlObject*>(value);

            if (o->ob_type != &Script::Control::type)
                PyErr_SetString(PyExc_RuntimeError, "Standard controls can only be set to control objects!");
            else
                the< ::Input>()->SetStandardControl(the< ::Input>()->left, o->control);
            return 0;
        }

        SET(Right)
        {
            Script::Control::ControlObject* o = reinterpret_cast<Script::Control::ControlObject*>(value);

            if (o->ob_type != &Script::Control::type)
                PyErr_SetString(PyExc_RuntimeError, "Standard controls can only be set to control objects!");
            else
                the< ::Input>()->SetStandardControl(the< ::Input>()->right, o->control);
            return 0;
        }

        SET(Enter)
        {
            Script::Control::ControlObject* o = reinterpret_cast<Script::Control::ControlObject*>(value);

            if (o->ob_type != &Script::Control::type)
                PyErr_SetString(PyExc_RuntimeError, "Standard controls can only be set to control objects!");
            else
                the< ::Input>()->SetStandardControl(the< ::Input>()->enter, o->control);
            return 0;
        }

        SET(Cancel)         
        {
            Script::Control::ControlObject* o = reinterpret_cast<Script::Control::ControlObject*>(value);

            if (o->ob_type != &Script::Control::type)
                PyErr_SetString(PyExc_RuntimeError, "Standard controls can only be set to control objects!");
            else
                the< ::Input>()->SetStandardControl(the< ::Input>()->cancel, o->control);
            return 0;
        }

#undef GET
#undef SET

        PyGetSetDef properties[] =
        {
            {   "up",       (getter)getUp,          (setter)setUp,      "Gets the standard \"Up\" control."     },
            {   "down",     (getter)getDown,        (setter)setDown,    "Gets the standard \"Down\" control."   },
            {   "left",     (getter)getLeft,        (setter)setLeft,    "Gets the standard \"Left\" control."   },
            {   "right",    (getter)getRight,       (setter)setRight,   "Gets the standard \"Right\" control."  },
            {   "enter",    (getter)getEnter,       (setter)setEnter,   "Gets the standard \"Enter\" control."  },
            {   "cancel",   (getter)getCancel,      (setter)setCancel,  "Gets the standard \"Cancel\" control." },
            {   "keyboard", (getter)getKeyboard,                    0,  "Gets the keyboard device."             },
            {   "mouse",    (getter)getMouse,                       0,  "Gets the mouse device."                },
            {   "joysticks",(getter)getJoysticks,0, "Gets a tuple containing all the connected joystick devices."   },
            {   0   }
        };

        void Init()
        {
            memset(&type, 0, sizeof type);

            //mappingmethods.mp_length = 0;
            //mappingmethods.mp_subscript = (binaryfunc)&Input_Subscript;
            //mappingmethods.mp_ass_subscript = 0;

            type.ob_refcnt = 1;
            type.ob_type = &PyType_Type;
            type.tp_name = "Input";
            type.tp_basicsize = sizeof type;
            type.tp_dealloc = (destructor)Destroy;
            type.tp_methods = methods;
            type.tp_getset = properties;
            //type.tp_as_mapping = &mappingmethods;
            type.tp_doc = "Interface for hardware input devices. (such as the keyboard and mouse)";

            PyType_Ready(&type);
        }

        PyObject* New()
        {
            InputObject* input = PyObject_New(InputObject, &type);

            if (!input)
                return 0;

            input->keyboard = Script::Keyboard::New();
            input->mouse = Script::Mouse::New();

            const uint numJoy = the< ::Input>()->NumJoysticks();
            input->joysticks = PyTuple_New(numJoy);
            for (uint i = 0; i < numJoy; i++)
            {
                PyObject* stick = Script::Joystick::New(the< ::Input>()->GetJoystick(i));
                PyTuple_SET_ITEM(input->joysticks, i, stick);
            }

            return (PyObject*)input;
        }

        void Destroy(InputObject* self)
        {
            PyObject_Del(self);
        }

#define METHOD(x)  PyObject* x(InputObject* self, PyObject* args)
#define METHOD1(x) PyObject* x(InputObject* self)

        METHOD1(Input_Update)
        {
            engine->CheckMessages();

            Py_INCREF(Py_None);
            return Py_None;
        }

        METHOD1(Input_Unpress)
        {
            the< ::Input>()->Unpress();

            Py_INCREF(Py_None);
            return Py_None;
        }

        /*METHOD(Input_GetControl)
        {
            PyObject* obj;
            if (!PyArg_ParseTuple(args, "O:Input.GetControl", &obj))
                return 0;

            return Input_Subscript(self, obj);
        }

        METHOD1(Input_GetKey)
        {
            char c = self->input->GetKey();

            if (c)
                return PyString_FromStringAndSize(&c, 1);   // wtf.  No PyString_FromChar
            else
            {
                Py_INCREF(Py_None);
                return Py_None;
            }
        }

        METHOD1(Input_ClearKeyQueue)
        {
            self->input->ClearKeyQueue();
            Py_INCREF(Py_None);
            return Py_None;
        }

        METHOD1(Input_WasKeyPressed)
        {
            return PyInt_FromLong(self->input->WasKeyPressed() ? 1 : 0);
        }*/

#undef METHOD
#undef METHOD1

        /*PyObject* Input_Subscript(InputObject* self, PyObject* key)
        {
            try
            {
                const char* name = PyString_AsString(key);
                if (!name)
                    throw "Non-string passed as input control name.";

                PyObject* obj = Script::Control::New(*self->input, name);

                if (!obj)
                    throw va("%s is not a valid input control name.", name);

                return obj;
            }
            catch (const char* s)
            {
                PyErr_SetString(PyExc_SyntaxError, s);
                return 0;
            }
        }*/
    }
}
