
/*
 * Image object stuff. ;D
 */

#include "video/Driver.h"
#include "video/Image.h"

struct v_ImageObject
{
    PyObject_HEAD
    Video::Image* data;
};

PyMethodDef CScriptEngine::image_methods[] =
{
    {   "Blit",         CScriptEngine::image_blit,         1    },
    {   "ScaleBlit",    CScriptEngine::image_scaleblit,    1    },
    {   "DistortBlit",  CScriptEngine::image_distortblit,  1    },
    {   "Clip",         CScriptEngine::image_clip,         1    },
    {   NULL,    NULL }
};

PyTypeObject CScriptEngine::imagetype;

void CScriptEngine::Init_Image()
{
    memset(&imagetype, 0, sizeof imagetype);
    
    imagetype.ob_refcnt=1;
    imagetype.ob_type=&PyType_Type;
    imagetype.tp_name="Image";
    imagetype.tp_basicsize=sizeof imagetype;
    imagetype.tp_dealloc=(destructor)Image_Destroy;
    imagetype.tp_getattr=(getattrfunc)Image_GetAttribute;
    imagetype.tp_doc="A hardware-dependant image.";
}

PyObject* CScriptEngine::Image_New(PyObject* self,PyObject* args)
{
    char* filename;
    
    if (!PyArg_ParseTuple(args,"s:newimage",&filename))
        return NULL;
    
    try
    {
        Canvas img(filename);

        v_ImageObject* image=PyObject_New(v_ImageObject,&imagetype);
        if (!image)
            return 0;
    
        image->data = pEngine->video->CreateImage(img);
        
        return (PyObject*)image;
    }
    catch (std::runtime_error e)
    {
        Log::Write(e.what());
        return 0;
    }
}

void CScriptEngine::Image_Destroy(PyObject* self)
{
    delete ((v_ImageObject*)self)->data;
    PyObject_Del(self);
}

PyObject* CScriptEngine::Image_GetAttribute(PyObject* self,char* name)
{
    if (!strcmp(name,"width"))
        return Py_BuildValue("i",((v_ImageObject*)self)->data->Width());          // @_@;
    if (!strcmp(name,"height"))
        return Py_BuildValue("i",((v_ImageObject*)self)->data->Height());         // yipes
    
    return Py_FindMethod(image_methods,self,name);
}

#define METHOD(x) PyObject* CScriptEngine::x(PyObject* self,PyObject* args)

METHOD(image_blit)
{
    int x,y;
    int trans = 1;
    
    if (!PyArg_ParseTuple(args,"ii|i:Image.Blit",&x,&y, &trans))
        return NULL;
    
    pEngine->video->BlitImage(
        ((v_ImageObject*)self)->data,
        x,y, trans != 0);
    
    Py_INCREF(Py_None);
    return Py_None;
}

METHOD(image_scaleblit)
{
/*    int x,y,width,height;
    int trans=1;
    
    if (!PyArg_ParseTuple(args,"iiii|i:Image.ScaleBlit",&x,&y,&width,&height,&trans))
        return NULL;
    
    gfxScaleBlitImage(
        ((v_ImageObject*)self)->data,
        x,y,width,height,trans?true:false);
  */  
    Py_INCREF(Py_None);
    return Py_None;
}

METHOD(image_distortblit)
{
    //int x[4],y[4];
    //int trans=1;

    //if (!PyArg_ParseTuple(args,"(iiii)(iiii)|i:Image.DistortBlit",&x[0],&x[1],&x[2],&x[3],&y[0],&y[1],&y[2],&y[3],&trans))
    //    return NULL;

    //gfxDistortBlitImage(
    //    ((v_ImageObject*)self)->data,
    //    x,y,trans!=0);

    Py_INCREF(Py_None);
    return Py_None;
};

METHOD(image_clip)
{
    //int x1=-1,y1=-1,x2=-1,y2=-1;
    //
    //if (!PyArg_ParseTuple(args,"|iiii:Image.Clip",&x1,&y1,&x2,&y2))
    //    return NULL;
    //
    //if (x1==-1 || y1==-1 || x2==-1 || y2==-1)
    //{
    //    Rect r
    //    (
    //        0,
    //            0, gfxImageWidth(((v_ImageObject*)self)->data),
    //            gfxImageHeight(((v_ImageObject*)self)->data) 
    //    );
    //    
    //    gfxClipImage(((v_ImageObject*)self)->data,r);
    //}
    //else
    //{
    //    Rect r( x1,y1,x2,y2 );
    //    gfxClipImage(
    //        ((v_ImageObject*)self)->data,
    //        r);
    //}
    
    Py_INCREF(Py_None);
    return Py_None;
}