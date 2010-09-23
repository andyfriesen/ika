#include "ObjectDefs.h"
#include "input.h"
#include "mouse.h"

#include <cassert>

namespace Script
{
    namespace Mouse
    {
        PyTypeObject type;

#define GET(x) PyObject* get ## x(PyObject*)
        GET(X)      { return Script::Control::New(the< ::Input>()->GetMouse()->GetAxis(::Mouse::X)); }
        GET(Y)      { return Script::Control::New(the< ::Input>()->GetMouse()->GetAxis(::Mouse::Y)); }
        GET(Wheel)  { return Script::Control::New(the< ::Input>()->GetMouse()->GetAxis(::Mouse::WHEEL)); }
        GET(Left)   { return Script::Control::New(the< ::Input>()->GetMouse()->GetButton(1));        }
        GET(Middle) { return Script::Control::New(the< ::Input>()->GetMouse()->GetButton(2));        }
        GET(Right)  { return Script::Control::New(the< ::Input>()->GetMouse()->GetButton(3));        }
#undef GET

        PyGetSetDef properties[] =
        {
            {   (char*)"x",        (getter)getX,       0,  (char*)"Gets the X axis of the mouse." },
            {   (char*)"y",        (getter)getY,       0,  (char*)"Gets the Y axis of the mouse." },
            {   (char*)"wheel",    (getter)getWheel,   0,  (char*)"Gets the mouse wheel."         },
            {   (char*)"left",     (getter)getLeft,    0,  (char*)"Gets the left mouse button."   },
            {   (char*)"right",    (getter)getRight,   0,  (char*)"Gets the right mouse button."  },
            {   (char*)"middle",   (getter)getMiddle,  0,  (char*)"Gets the middle mouse button." },
            {   0   }
        };

        void Init()
        {
            memset(&type, 0, sizeof type);

            //type.ob_refcnt = 1;
            //type.ob_type = &PyType_Type;
            type.tp_name = "Mouse";
            type.tp_basicsize = sizeof type;
            type.tp_dealloc = (destructor)Destroy;
            type.tp_getset = properties;
            type.tp_base = &Script::InputDevice::type;
            type.tp_doc = "The mouse.  Access this through ika.Input.mouse\n"
                "Additional instances of this object cannot be created.";
            PyType_Ready(&type);
        }

        PyObject* New()
        {
            PyObject* obj = PyObject_New(PyObject, &type);
            assert(obj);
            return obj;
        }

        void Destroy(PyObject* self)
        {
            PyObject_Del(self);
        }
    }
}
