
#include "video/Driver.h"
#include "video/Image.h"

struct v_VideoObject
{
    PyObject_HEAD
};

PyMethodDef CScriptEngine::video_methods[] =
{
    {   "Blit",         CScriptEngine::video_blit, 1   },
    {   "ScaleBlit",    CScriptEngine::video_scaleblit, 1   },
    {   "DistortBlit",  CScriptEngine::video_distortblit, 1 },
    // TODO: more blits.  I want a wrapblit, tintblit, and others
    {   "DrawLine",     CScriptEngine::video_drawline,  1   },
    {   "DrawRect",     CScriptEngine::video_drawrect,  1   },
    {   "DrawEllipse",  CScriptEngine::video_drawellipse, 1 },
    {   "DrawPixel",    CScriptEngine::video_drawpixel, 1   },
};

PyTypeObject CScriptEngine::videotype;

void CScriptEngine::Init_Video()
{
    memset(&videotype, 0, sizeof videotype);

    videotype.ob_refcnt = 1;
    videotype.ob_type=&PyType_Type;
    videotype.tp_name="Video";
    videotype.tp_basicsize=sizeof videotype;
    videotype.tp_dealloc=(destructor)Video_Destroy;
    videotype.tp_getattr=(getattrfunc)Video_GetAttribute;
    videotype.tp_doc="Interface for ika's graphics engine.";
}

PyObject* CScriptEngine::Video_New()
{
    return (PyObject*)PyObject_New(v_VideoObject, &videotype);
}

void CScriptEngine::Video_Destroy(PyObject* self)
{
    PyObject_Del(self);
}

PyObject* CScriptEngine::Video_GetAttribute(PyObject* self, char* name)
{
    return Py_FindMethod(video_methods,self,name);
}

int CScriptEngine::Video_SetAttribute(PyObject* self, char* name, PyObject* value)
{
    return 0;
}

#define METHOD(x) PyObject* CScriptEngine::x(PyObject* self,PyObject* args)

METHOD(video_blit)
{
    v_ImageObject* image;
    int x, y;
    int trans = 1;

    if (!PyArg_ParseTuple(args, "Oii|i:Video.Blit", &image, &x, &y, &trans))
        return 0;

    pEngine->video->BlitImage(image->data, x, y, trans != 0);

    Py_INCREF(Py_None);
    return Py_None;
}

METHOD(video_scaleblit)
{
    v_ImageObject* image;
    int x, y;
    int w, h;
    int trans = 1;

    if (!PyArg_ParseTuple(args, "Oiiii|i:Video.ScaleBlit", &image, &x, &y, &w, &h, &trans))
        return 0;

    pEngine->video->ScaleBlitImage(image->data, x, y, w, h, trans != 0);

    Py_INCREF(Py_None);
    return Py_None;
}

METHOD(video_distortblit)
{
    v_ImageObject* image;
    int x[4], y[4];
    int trans = 1;

    if (!PyArg_ParseTuple(args, "O(iiii)(iiii)|i:Video.DistortBlit", &image, x, x + 1, x + 2, x + 3, y, y + 1, y + 2, y + 3, &trans))
        return 0;

    pEngine->video->DistortBlitImage(image->data, x, y, trans != 0);

    Py_INCREF(Py_None);
    return Py_None;
}

METHOD(video_drawline)
{
    int x1, y1, x2, y2;
    u32 colour;

    if (!PyArg_ParseTuple(args, "iiiii:Video.DrawLine", &x1, &y1, &x2, &y2, &colour))
        return 0;

    pEngine->video->DrawLine(x1, y1, x2, y2, colour);

    Py_INCREF(Py_None);
    return Py_None;
}

METHOD(video_drawrect)
{
    int x1, y1, x2, y2;
    u32 colour;
    int filled = 0;

    if (!PyArg_ParseTuple(args, "iiiii|i:Video.DrawRect", &x1, &y1, &x2, &y2, &colour, &filled))
        return 0;

    pEngine->video->DrawRect(x1, y1, x2, y2, colour, filled != 0);

    Py_INCREF(Py_None);
    return Py_None;
}

METHOD(video_drawellipse)
{
    int cx, cy;
    int rx, ry;
    u32 colour;
    int filled = 0;

    if (!PyArg_ParseTuple(args, "iiiii|i:Video.DrawEllipse", &cx, &cy, &rx, &ry, &colour, &filled))
        return 0;

    pEngine->video->DrawEllipse(cx, cy, rx, ry, colour, filled != 0);

    Py_INCREF(Py_None);
    return Py_None;
}

METHOD(video_drawpixel)
{
    int x, y;
    u32 colour;
    int filled = 0;

    if (!PyArg_ParseTuple(args, "iii:Video.DrawPixel", &x, &y, &colour))
        return 0;

    pEngine->video->DrawPixel(x, y, colour);

    Py_INCREF(Py_None);
    return Py_None;
}

#undef METHOD