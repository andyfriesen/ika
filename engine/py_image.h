
/*
Image object stuff. ;D
*/

#include "graph.h"

struct v_ImageObject
{
    PyObject_HEAD
        handle data;
};

PyMethodDef CScriptEngine::image_methods[] =
{
    {   "Load",         CScriptEngine::image_load,         1    },
    {   "Copy",         CScriptEngine::image_copy,         1    },
    {   "Blit",         CScriptEngine::image_blit,         1    },
    {   "ScaleBlit",    CScriptEngine::image_scaleblit,    1    },
    {   "DistortBlit",  CScriptEngine::image_distortblit,  1    },
    {   "CopyChannel",  CScriptEngine::image_copychannel,  1    },
    {   "Clip",         CScriptEngine::image_clip,         1    },
    {   "Line",         CScriptEngine::image_line,         1    },
    {   "Rect",         CScriptEngine::image_rect,         1    },
    {   "Ellipse",      CScriptEngine::image_ellipse,      1    },
    {   "SetPixel",     CScriptEngine::image_setpixel,     1    },
    {   "GetPixel",     CScriptEngine::image_getpixel,     1    },
    {   "FlatPoly",     CScriptEngine::image_flatpoly,     1    },
    {   NULL,    NULL }
};

PyTypeObject CScriptEngine::imagetype;

void CScriptEngine::Init_Image()
{
    ZeroMemory(&imagetype,sizeof imagetype);
    
    imagetype.ob_refcnt=1;
    imagetype.ob_type=&PyType_Type;
    imagetype.tp_name="Image";
    imagetype.tp_basicsize=sizeof imagetype;
    imagetype.tp_dealloc=(destructor)Image_Destroy;
    imagetype.tp_getattr=(getattrfunc)Image_GetAttribute;
    
    // Create the Python screen image
    pScreenobject=(PyObject*)PyObject_New(v_ImageObject,&imagetype);
    ((v_ImageObject*)pScreenobject)->data=gfxGetScreenImage();
    
    // Init the render dest
    Py_INCREF(pScreenobject);
    pRenderdest=pScreenobject;
    gfxSetRenderDest(((v_ImageObject*)pRenderdest)->data);
}

PyObject* CScriptEngine::Image_New(PyObject* self,PyObject* args)
{
    int width=10,height=10;
    
    if (!PyArg_ParseTuple(args,"|ii:newimage",&width,&height))
        return NULL;
    
    v_ImageObject* image=PyObject_New(v_ImageObject,&imagetype);
    if (!image)
        return NULL;
    
    image->data=gfxCreateImage(width,height);
    
    return (PyObject*)image;
}

void CScriptEngine::Image_Destroy(PyObject* self)
{
    gfxFreeImage(((v_ImageObject*)self)->data);
    PyObject_Del(self);
}

PyObject* CScriptEngine::Image_GetAttribute(PyObject* self,char* name)
{
    if (!strcmp(name,"width"))
        return Py_BuildValue("i",gfxImageWidth(((v_ImageObject*)self)->data));          // @_@;
    if (!strcmp(name,"height"))
        return Py_BuildValue("i",gfxImageHeight(((v_ImageObject*)self)->data));         // yipes
    
    return Py_FindMethod(image_methods,self,name);
}

#define METHOD(x) PyObject* CScriptEngine::x(PyObject* self,PyObject* args)

METHOD(image_load)
{
    char* filename;
    
    if (!PyArg_ParseTuple(args,"s:Image.Load",&filename))
        return NULL;
    
    try
    {
        if (!File::Exists(filename))
            throw va("%s does not exist",filename);
        
        bool bResult=gfxLoadPNG(((v_ImageObject*)self)->data,filename);
        
        if (!bResult)
            throw va("Failed to load %s",filename);
    }
    catch (const char* s)
    {
        PyErr_SetString(PyExc_OSError,s);
        return NULL;
    }
    
    Py_INCREF(Py_None);
    return Py_None;
}

METHOD(image_copy)
{
    if (!PyArg_ParseTuple(args,""))
        return NULL;
    
    handle srcimage=((v_ImageObject*)self)->data;
    
    v_ImageObject* image=PyObject_New(v_ImageObject,&imagetype);
    if (!image)
        return NULL;
    
    image->data=gfxCopyImage(srcimage);
    
    return (PyObject*)image;
}

METHOD(image_blit)
{
    int x,y;
    int trans=1;
    
    if (!PyArg_ParseTuple(args,"ii|i:Image.Blit",&x,&y,&trans))
        return NULL;
    
    gfxBlitImage(
        ((v_ImageObject*)self)->data,
        x,y,trans?true:false);
    
    Py_INCREF(Py_None);
    return Py_None;
}

METHOD(image_scaleblit)
{
    int x,y,width,height;
    int trans=1;
    
    if (!PyArg_ParseTuple(args,"iiii|i:Image.ScaleBlit",&x,&y,&width,&height,&trans))
        return NULL;
    
    gfxScaleBlitImage(
        ((v_ImageObject*)self)->data,
        x,y,width,height,trans?true:false);
    
    Py_INCREF(Py_None);
    return Py_None;
}

METHOD(image_distortblit)
{
    int x[4],y[4];
    int trans=1;

    if (!PyArg_ParseTuple(args,"(iiii)(iiii)|i:Image.DistortBlit",&x[0],&x[1],&x[2],&x[3],&y[0],&y[1],&y[2],&y[3],&trans))
        return NULL;

    gfxDistortBlitImage(
        ((v_ImageObject*)self)->data,
        x,y,trans!=0);

    Py_INCREF(Py_None);
    return Py_None;
};

METHOD(image_copychannel)
{
    v_ImageObject* pDest;
    int nSrcchan,nDestchan;
    
    if (!PyArg_ParseTuple(args,"O!ii",&imagetype,&pDest,&nSrcchan,&nDestchan))
        return NULL;
    
    gfxCopyChan(((v_ImageObject*)self)->data,nSrcchan,pDest->data,nDestchan);
    
    Py_INCREF(Py_None);
    return Py_None;
}

METHOD(image_clip)
{
    int x1=-1,y1=-1,x2=-1,y2=-1;
    
    if (!PyArg_ParseTuple(args,"|iiii:Image.Clip",&x1,&y1,&x2,&y2))
        return NULL;
    
    if (x1==-1 || y1==-1 || x2==-1 || y2==-1)
    {
        Rect r
        (
            0,
                0, gfxImageWidth(((v_ImageObject*)self)->data),
                gfxImageHeight(((v_ImageObject*)self)->data) 
        );
        
        gfxClipImage(((v_ImageObject*)self)->data,r);
    }
    else
    {
        Rect r( x1,y1,x2,y2 );
        gfxClipImage(
            ((v_ImageObject*)self)->data,
            r);
    }
    
    Py_INCREF(Py_None);
    return Py_None;
}

METHOD(image_line)
{
    int x1,y1,x2,y2,colour;
    
    if (!PyArg_ParseTuple(args,"iiiii:Image.Line",&x1,&y1,&x2,&y2,&colour))
        return NULL;
    
    gfxLine(((v_ImageObject*)self)->data,x1,y1,x2,y2,colour);
    
    Py_INCREF(Py_None);
    return Py_None;
}

METHOD(image_rect)
{
    int x1,y1,x2,y2,colour,filled=0;
    
    if (!PyArg_ParseTuple(args,"iiiii|i:Image.Rect",&x1,&y1,&x2,&y2,&colour,&filled))
        return NULL;
    
    gfxRect(((v_ImageObject*)self)->data,x1,y1,x2,y2,colour,filled!=0);
    
    Py_INCREF(Py_None);
    return Py_None;
}

METHOD(image_ellipse)
{
    int cx,cy,rx,ry,colour,filled=0;
    
    if (!PyArg_ParseTuple(args,"iiiii|i:Image.Ellipse",&cx,&cy,&rx,&ry,&colour,&filled))
        return NULL;
    
    gfxEllipse(((v_ImageObject*)self)->data,cx,cy,rx,ry,colour,filled?true:false);
    
    Py_INCREF(Py_None);
    return Py_None;
}

METHOD(image_setpixel)
{
    int x,y,colour;
    int trans=1;
    
    if (!PyArg_ParseTuple(args,"iii:Image.SetPixel",&x,&y,&colour))
        return NULL;
    
    gfxSetPixel(((v_ImageObject*)self)->data,x,y,colour);
    
    Py_INCREF(Py_None);
    return Py_None;
}

METHOD(image_getpixel)
{
    int x,y;
    
    if (!PyArg_ParseTuple(args,"ii:Image.GetPixel",&x,&y))
        return NULL;
    
    u32 c=gfxGetPixel(((v_ImageObject*)self)->data,x,y);
    
    return PyInt_FromLong(c);
}

METHOD(image_flatpoly)
{
    int x[3];
    int y[3];
    int c[3];
    
    if (!PyArg_ParseTuple(args,"iiiiiiiii:Image.FlatPoly",&(x[0]),&(y[0]),&(c[0]),&(x[1]),&(y[1]),&(c[1]),&(x[2]),&(y[2]),&(c[2])))
        return NULL;
    
    gfxFlatPoly(((v_ImageObject*)self)->data,x,y,c);
    
    Py_INCREF(Py_None);
    return Py_None;
}

/////////////////////////

METHOD(std_loadimage)
{
    PyObject* image=Image_New(NULL,Py_BuildValue("()"));
    
    PyObject* result=image_load(image,args);
    
    Py_XDECREF(result);
    
    return image;
}

METHOD(std_setrenderdest)
{
    PyObject* newrenderdest;
    
    if (!PyArg_ParseTuple(args,"O!",&imagetype,&newrenderdest))
        return NULL;
    
    pEngine->hRenderdest=((v_ImageObject*)newrenderdest)->data;
    
    gfxSetRenderDest(pEngine->hRenderdest);
    
    Py_XDECREF(pRenderdest);
    Py_INCREF(newrenderdest);
    pRenderdest=newrenderdest;
    
    Py_INCREF(Py_None);
    return Py_None;
}

METHOD(std_getrenderdest)
{
    if (!PyArg_ParseTuple(args,""))
        return NULL;
    
    Py_INCREF(pRenderdest);
    return pRenderdest;
}