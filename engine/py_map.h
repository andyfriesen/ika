/*
Map manipulation junque.
*/

struct v_MapObject
{
    PyObject_HEAD
        // CMap* pMap; // dear god that'd be cool.  Maybe later. :)
};

PyMethodDef CScriptEngine::map_methods[] =
{
    {	"Switch",	CScriptEngine::map_switch,		1	},
    {	"Render",	CScriptEngine::map_render,		1	},
    {	"GetTile",	CScriptEngine::map_gettile,		1	},
    {	"SetTile",	CScriptEngine::map_settile,		1	},
    {	"GetObs",	CScriptEngine::map_getobs,		1	},
    {	"SetObs",	CScriptEngine::map_setobs,		1	},
    {	"GetZone",	CScriptEngine::map_getzone,		1	},
    {	"SetZone",	CScriptEngine::map_setzone,		1	},
    {	"GetParallax",	CScriptEngine::map_getparallax,	1	},
    {	"SetParallax",	CScriptEngine::map_setparallax,	1	},
    {	NULL,		NULL	}	// end of list
};

PyTypeObject CScriptEngine::maptype;

void CScriptEngine::Init_Map()
{
    ZeroMemory(&maptype,sizeof maptype);
    
    maptype.ob_refcnt=1;
    maptype.ob_type=&PyType_Type;
    maptype.tp_name="Map";
    maptype.tp_basicsize=sizeof maptype;
    maptype.tp_dealloc=(destructor)Map_Destroy;
    maptype.tp_getattr=(getattrfunc)Map_GetAttribute;
    maptype.tp_setattr=(setattrfunc)Map_SetAttribute;
    maptype.tp_doc="Represents the currently executing map.";
}

PyObject* CScriptEngine::Map_New()
{
    v_MapObject* map=PyObject_New(v_MapObject,&maptype);
    
    if (!map)
        return NULL;
    
    return (PyObject*)map;
}

void CScriptEngine::Map_Destroy(PyObject* self)
{
    PyObject_Del(self);
}

PyObject* CScriptEngine::Map_GetAttribute(PyObject* self,char* name)
{
    if (!strcmp(name,"xwin"))
        return PyInt_FromLong(pEngine->xwin);
    if (!strcmp(name,"ywin"))
        return PyInt_FromLong(pEngine->ywin);
    if (!strcmp(name,"numtiles"))
        return PyInt_FromLong(pEngine->tiles.NumTiles());
    if (!strcmp(name,"tilewidth"))
        return PyInt_FromLong(pEngine->tiles.Width());
    if (!strcmp(name,"tileheight"))
        return PyInt_FromLong(pEngine->tiles.Height());
    if (!strcmp(name,"width"))
        return PyInt_FromLong(pEngine->map.Width());
    if (!strcmp(name,"height"))
        return PyInt_FromLong(pEngine->map.Height());
    if (!strcmp(name,"rstring"))
        return PyString_FromString(pEngine->map.GetRString().c_str());
    if (!strcmp(name,"vspname"))
        return PyString_FromString(pEngine->map.GetVSPName().c_str());
    if (!strcmp(name,"defaultmusic"))
        return PyString_FromString(pEngine->map.GetMusic().c_str());
    if (!strcmp(name,"entities"))
    {
        Py_INCREF(pEntitydict);
        return pEntitydict;
    }
    return Py_FindMethod(map_methods,self,name);
}

int CScriptEngine::Map_SetAttribute(PyObject* self,char* name,PyObject* value)
{
    if (!strcmp(name,"xwin"))
    {
        int i=PyInt_AsLong(value);
        int maxx=pEngine->map.Width()*pEngine->tiles.Width()-gfxImageWidth(pEngine->hRenderdest);
        if (i<0) i=0;
        if (i>=maxx) i=maxx-1;
        
        pEngine->xwin=i;
    }
    if (!strcmp(name,"ywin"))
    {
        int i=PyInt_AsLong(value);
        int maxy=pEngine->map.Height()*pEngine->tiles.Height()-gfxImageHeight(pEngine->hRenderdest);
        if (i<0) i=0;
        if (i>maxy) i=maxy;
        
        pEngine->ywin=i;
    }
    
    if (!strcmp(name,"numtiles"))
    {
        PyErr_SetString(PyExc_SyntaxError,"map.numtiles is read only");
        return -1;
    }
    if (!strcmp(name,"tilewidth"))
    {
        PyErr_SetString(PyExc_SyntaxError,"map.tilewidth is read only");
        return -1;
    }
    if (!strcmp(name,"tileheight"))
    {
        PyErr_SetString(PyExc_SyntaxError,"map.tileheight is read only");
        return -1;
    }
    if (!strcmp(name,"width"))
    {
        PyErr_SetString(PyExc_SyntaxError,"map.width is read only");
        return -1;
    }
    if (!strcmp(name,"height"))
    {
        PyErr_SetString(PyExc_SyntaxError,"map.height is read only");
        return -1;
    }
    if (!strcmp(name,"rstring"))
        pEngine->map.SetRString(PyString_AsString(value));
    if (!strcmp(name,"vspname"))
        pEngine->map.SetVSPName(PyString_AsString(value));
    if (!strcmp(name,"defaultmusic"))
        pEngine->map.SetMusic(PyString_AsString(value));
    if (!strcmp(name,"entities"))
    {
        PyErr_SetString(PyExc_SyntaxError,"map.entities is read only");
        return -1;
    }
    
    return 0;
}

METHOD(map_switch)
{
    char* filename;
    
    if (!PyArg_ParseTuple(args,"s:MapSwitch",&filename))
        return NULL;
    
    if (!File::Exists(filename))
    {
        PyErr_SetString(PyExc_OSError,va("Unable to load %s",filename));
        return NULL;
    }
    
    pEngine->LoadMap(filename);
    
    Py_INCREF(Py_None);
    return Py_None;
}

METHOD(map_render)
{
    char* rstring=NULL;
    
    if (!PyArg_ParseTuple(args,"|s:RenderMap",&rstring))
        return NULL;
    
    if (rstring)
        pEngine->Render(rstring);
    else
        pEngine->Render();
    
    Py_INCREF(Py_None);
    return Py_None;
}

METHOD(map_gettile)
{
    int x,y,lay;
    
    if (!PyArg_ParseTuple(args,"iii:Map.GetTile",&x,&y,&lay))
        return NULL;
    
    return PyInt_FromLong(pEngine->map.GetTile(x,y,lay));
}

METHOD(map_settile)
{
    int x,y,lay,tile;
    
    if (!PyArg_ParseTuple(args,"iiii:Map.SetTile",&x,&y,&lay,&tile))
        return NULL;
    
    pEngine->map.SetTile(x,y,lay,tile);
    
    Py_INCREF(Py_None);
    return Py_None;
}

METHOD(map_getobs)
{
    int x,y;
    
    if (!PyArg_ParseTuple(args,"ii:Map.GetObs",&x,&y))
        return NULL;
    
    return PyInt_FromLong(pEngine->map.IsObs(x,y));
}

METHOD(map_setobs)
{
    int x,y,bSet;
    
    if (!PyArg_ParseTuple(args,"iii:Map.SetObs",&x,&y,&bSet))
        return NULL;
    
    pEngine->map.SetObs(x,y,bSet!=0);
    
    Py_INCREF(Py_None);
    return Py_None;
}

METHOD(map_getzone)
{
    int x,y;
    
    if (!PyArg_ParseTuple(args,"ii:Map.GetZone",&x,&y))
        return NULL;
    
    return PyInt_FromLong(pEngine->map.GetZone(x,y));
}

METHOD(map_setzone)
{
    int x,y,z;
    
    if (!PyArg_ParseTuple(args,"iii:Map.SetZone",&x,&y,&z))
        return NULL;
    
    pEngine->map.SetZone(x,y,z);
    
    Py_INCREF(Py_None);
    return Py_None;
}

// urk.. temporary code.  Please be temporary code.
METHOD(map_getparallax)
{
    int lay;
    
    if (!PyArg_ParseTuple(args,"i:Map.GetParallax",&lay))
        return NULL;
    
    SMapLayerInfo layerinfo;
    pEngine->map.GetLayerInfo(layerinfo,lay);
    
    return Py_BuildValue("((iiii)i)",
        layerinfo.pmulx,
        layerinfo.pdivx,
        layerinfo.pmuly,
        layerinfo.pdivy,
        layerinfo.nTransmode
        );
}

METHOD(map_setparallax)
{
    return NULL;
}