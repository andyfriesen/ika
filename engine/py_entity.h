/*
Python entity interface.

  I'm not sure if I'm %100 happy with this setup.  Entities are *only* destroyed through Python's GC, which means that
  entities can be made "persistent" simply by holding on to a reference to them.  Additionally, there is an implicitly
  defined dictionary that holds all the map-defined entities. (and serves as their "life line", to keep them from
  being garbage collected)    
*/

#include "main.h"

struct v_EntityObject
{
    PyObject_HEAD
    CEntity* pEnt;
    
    //    PyObject* pDict;
};

PyMethodDef CScriptEngine::entity_methods[] =
{
    {    "Move",            CScriptEngine::entity_move,             1    },
    {    "Chase",           CScriptEngine::entity_chase,            1    },
    {    "Wander",          CScriptEngine::entity_wander,           1    },
    {    "WanderZone",      CScriptEngine::entity_wanderzone,       1    },
    {    "Stop",            CScriptEngine::entity_stop,             1    },
    {    "IsMoving",        CScriptEngine::entity_ismoving,         1    },
    {    "DetectCollision", CScriptEngine::entity_detectcollision,  1    },
    {    NULL,              NULL    },                        // end of list
};

PyTypeObject CScriptEngine::entitytype;

void CScriptEngine::Init_Entity()
{
    memset(&entitytype, 0, sizeof entitytype);
    
    entitytype.ob_refcnt=1;
    entitytype.ob_type=&PyType_Type;
    entitytype.tp_name="Entity";
    entitytype.tp_basicsize=sizeof(v_EntityObject);
    entitytype.tp_dealloc=(destructor)Entity_Destroy;
    entitytype.tp_getattr=(getattrfunc)Entity_GetAttribute;
    entitytype.tp_setattr=(setattrfunc)Entity_SetAttribute;
    entitytype.tp_doc="Represents an entity in the ika game engine.";
}

PyObject* CScriptEngine::Entity_New(CEntity* e)
{
    v_EntityObject* ent=PyObject_New(v_EntityObject,&entitytype);
    if (!ent)
        return NULL;
    
    ent->pEnt=e;
    
    return (PyObject*)ent;
}

void CScriptEngine::Entity_Destroy(PyObject* self)
{
    CEntity*& pEnt=((v_EntityObject*)self)->pEnt;

    if (pEnt)
        pEngine->DestroyEntity(pEnt);
    else
        Log::Write("Entity_Destroy weirdness");

    pEnt=0;
   
    PyObject_Del(self);
}

PyObject* CScriptEngine::Entity_GetAttribute(PyObject* self,char* name)
{
    // Get a pointer to the entity, to make the below code nicer. :)
    CEntity& ent=*((v_EntityObject*)self)->pEnt;
    
    // I wonder how heavy a toll this exacts.  Maybe replace it with a hash table.
    if (!strcmp(name,"x"))
        return PyInt_FromLong(ent.x);
    if (!strcmp(name,"y"))
        return PyInt_FromLong(ent.y);
    
    if (!strcmp(name,"speed"))
        return PyInt_FromLong(ent.nSpeed);
    
    if (!strcmp(name,"direction"))
        return PyInt_FromLong(ent.direction);
    
    if (!strcmp(name,"curframe"))
        return PyInt_FromLong(ent.nCurframe);
    if (!strcmp(name,"specframe"))
        return PyInt_FromLong(ent.nSpecframe);
    if (!strcmp(name,"visible"))
        return PyInt_FromLong(ent.bVisible?1:0);
    if (!strcmp(name,"name"))
        return PyString_FromString(ent.sName.c_str());
    if (!strcmp(name,"actscript"))
        return PyString_FromString(ent.sActscript.c_str());
    if (!strcmp(name,"adjacentactivate"))
        return PyInt_FromLong(ent.bAdjacentactivate?1:0);
    
    if (!strcmp(name,"isobs"))
        return PyInt_FromLong(ent.bIsobs?1:0);
    if (!strcmp(name,"mapobs"))
        return PyInt_FromLong(ent.bMapobs?1:0);
    if (!strcmp(name,"entobs"))
        return PyInt_FromLong(ent.bEntobs?1:0);
    
    if (!strcmp(name,"sprite"))
        return PyString_FromString(ent.pSprite->sFilename.c_str());
    if (!strcmp(name,"hotx"))
        return PyInt_FromLong(ent.pSprite->nHotx);
    if (!strcmp(name,"hoty"))
        return PyInt_FromLong(ent.pSprite->nHoty);
    if (!strcmp(name,"hotwidth"))
        return PyInt_FromLong(ent.pSprite->nHotw);
    if (!strcmp(name,"hotheight"))
        return PyInt_FromLong(ent.pSprite->nHoth);
    
    return Py_FindMethod(entity_methods,(PyObject*)self,name);                // this would appear to be the default behaviour.  Too bad we can't use it.*/
}

int CScriptEngine::Entity_SetAttribute(PyObject* self,char* name,PyObject* value)
{
    // Get a pointer to the entity, to make the below code nicer. :)
    CEntity& ent=*((v_EntityObject*)self)->pEnt;
    
    if (!strcmp(name,"x"))
        ent.x=PyInt_AsLong(value);
    if (!strcmp(name,"y"))
        ent.y=PyInt_AsLong(value);
    
    if (!strcmp(name,"speed"))
        ent.nSpeed=PyInt_AsLong(value);
    
    if (!strcmp(name,"direction"))
        ent.direction=(Direction)PyInt_AsLong(value);    // I dislike this
    
    if (!strcmp(name,"curframe"))                        // you's isn't allowed to alter this one
    {
        PyErr_SetString(PyExc_TypeError,"Entity.curframe is read only!");
        return -1;
    }
    if (!strcmp(name,"specframe"))
        ent.nSpecframe=PyInt_AsLong(value);
    if (!strcmp(name,"visible"))
        ent.bVisible= PyInt_AsLong(value)!=0 ;
    if (!strcmp(name,"name"))
        ent.sName=PyString_AsString(value);
    if (!strcmp(name,"actscript"))
        ent.sActscript=PyString_AsString(value);
    if (!strcmp(name,"adjacentactivate"))
        ent.bAdjacentactivate=(PyInt_AsLong(value)!=0);
    
    
    if (!strcmp(name,"isobs"))
        ent.bIsobs= (PyInt_AsLong(value)!=0) ;
    if (!strcmp(name,"mapobs"))
        ent.bMapobs= (PyInt_AsLong(value)!=0) ;
    if (!strcmp(name,"entobs"))
        ent.bEntobs= (PyInt_AsLong(value)!=0) ;
    
    if (!strcmp(name,"sprite"))
    {
        pEngine->sprite.Free(ent.pSprite);

        ent.pSprite=pEngine->sprite.Load(PyString_AsString(value), pEngine->video);
        ent.SetAnimScript(va("F%i",ent.nCurframe));
    }
    if (!strcmp(name,"hotx") || !strcmp(name,"hoty") || 
        !strcmp(name,"hotwidth") || !strcmp(name,"hotheight"))
    {
        PyErr_SetString(PyExc_SyntaxError,va("Entity.%s is read-only",name));
        return -1;
    }    
    
    // FIXME: you can't add properties yet
    return 0;
    
    //    return PyDict_SetItem(self->pDict,name,value);*/
}

METHOD(entity_move)
{
    char* sPattern;
    
    if (!PyArg_ParseTuple(args,"s:Entity.Move",&sPattern))
        return NULL;
    
    CEntity& ent=*((v_EntityObject*)self)->pEnt;
    ent.movecode=mc_script;
    ent.SetMoveScript(sPattern);
    
    Py_INCREF(Py_None);
    return Py_None;
}

METHOD(entity_chase)
{
    int nDistance=0;
    v_EntityObject* pChasetarget;
    
    if (!PyArg_ParseTuple(args,"O!|i:Entity.Chase",&entitytype,&pChasetarget,&nDistance))
        return NULL;
    
    CEntity& ent=*((v_EntityObject*)self)->pEnt;
    ent.movecode=mc_chase;
    ent.pChasetarget=pChasetarget->pEnt;
    ent.nMinchasedist=nDistance;
    
    Py_INCREF(Py_None);
    return Py_None;
}

METHOD(entity_wander)
{
    int nSteps,nDelay;
    int x1=-1,y1=-1,x2=-1,y2=-1;
    
    if (!PyArg_ParseTuple(args,"ii|iiii:Entity.Wander",&nSteps,&nDelay,&x1,&y1,&x2,&y2))
        return NULL;
    
    CEntity& ent=*((v_EntityObject*)self)->pEnt;
    ent.nWandersteps=nSteps;
    ent.nWanderdelay=nDelay;
    
    if (x1==-1 || y1==-1 || x2==-1 || y2==-1)
        ent.movecode=mc_wander;
    else
    {
        ent.movecode=mc_wanderrect;
        ent.wanderrect.left=x1;
        ent.wanderrect.top=y1;
        ent.wanderrect.right=x2;
        ent.wanderrect.bottom=y2;
    }
    
    Py_INCREF(Py_None);
    return Py_None;
}

METHOD(entity_wanderzone)
{
    int nSteps,nDelay;
    int nZone;
    
    if (!PyArg_ParseTuple(args,"iii:Entity.Wander",&nSteps,&nDelay,&nZone))
        return NULL;
    
    CEntity& ent=*((v_EntityObject*)self)->pEnt;
    ent.nWandersteps=nSteps;
    ent.nWanderdelay=nDelay;
    ent.nWanderzone=nZone;
    
    Py_INCREF(Py_None);
    return Py_None;
}

METHOD(entity_stop)
{
    if (!PyArg_ParseTuple(args,""))
        return NULL;
    
    ((v_EntityObject*)self)->pEnt->movecode=mc_nothing;
    
    Py_INCREF(Py_None);
    return Py_None;
}

METHOD(entity_ismoving)
{
    if (!PyArg_ParseTuple(args,""))
        return NULL;
    
    CEntity& ent=*((v_EntityObject*)self)->pEnt;    
       
    return PyInt_FromLong(ent.bMoving?1:0);
}

METHOD(entity_detectcollision)
{
    if (!PyArg_ParseTuple(args,""))
        return NULL;
    
    const CEntity& e1=*((v_EntityObject*)self)->pEnt;
    
    int x1=e1.x;
    int y1=e1.y;
    int x2=x1+e1.pSprite->nHotw;
    int y2=y1+e1.pSprite->nHoth;
    
    int nCount=0;
    PyObject* pKey=NULL;
    PyObject* pValue=NULL;
    while (PyDict_Next(pEntitydict,&nCount,&pKey,&pValue))
    {
        CEntity& e2=*((v_EntityObject*)pValue)->pEnt;
        if (&e1==&e2)   continue;

        if (x1>e2.x+e2.pSprite->nHotw)    continue;
        if (y1>e2.y+e2.pSprite->nHoth)    continue;
        if (x2<e2.x)    continue;
        if (y2<e2.y)    continue;
        
        Py_INCREF(pValue);
        return pValue;
    }
    
    Py_INCREF(Py_None);
    return Py_None;
}

/////////////////

METHOD(std_spawnentity)
{
    int x,y;
    char* chrname;
    
    if (!PyArg_ParseTuple(args,"iis:Entity_new",&x,&y,&chrname))
        return NULL;
    
    if (!File::Exists(chrname))
        pEngine->Sys_Error(va("Could not load %s",chrname));
    
    CEntity* pEnt=pEngine->SpawnEntity();
    CSprite* pSprite=pEngine->sprite.Load(chrname, pEngine->video);
    
    pEnt->sName="Spawned entity";
    pEnt->pSprite=pSprite;
    pEnt->x=x;
    pEnt->y=y;
    
    v_EntityObject* ent=PyObject_New(v_EntityObject,&entitytype);
    if (!ent)
        return NULL;
    
    ent->pEnt=pEnt;
    
    return (PyObject*)ent;
}

METHOD(std_processentities)
{
    if (!PyArg_ParseTuple(args,""))
        return NULL;
    
    pEngine->ProcessEntities();
    
    Py_INCREF(Py_None);
    return Py_None;
}

METHOD(std_setplayer)                                            // FIXME?  Is there a more intuitive way to do this?
{
    v_EntityObject* ent;
    
    if (!PyArg_ParseTuple(args,"O:SetPlayerEntity",&ent))
        return NULL;
    
    if ((PyObject*)ent==Py_None)
    {
        Py_XDECREF(pPlayerent);
        pPlayerent=0;
        pEngine->pPlayer=0;
    }
    else
    {
        if (ent->ob_type!=&entitytype)
        {
            PyErr_SetString(PyExc_TypeError,"SetPlayerEntity not called with entity object or None.");
            return NULL;
        }

        Py_INCREF(ent);
        Py_XDECREF(pPlayerent);
        pPlayerent=(PyObject*)ent;
        
        pEngine->pPlayer=ent->pEnt;
        ent->pEnt->movecode=mc_nothing;
    }

    PyObject* result=std_setcameratarget(self,args);
    Py_XDECREF(result);
       
    Py_INCREF(Py_None);
    return Py_None;
}

METHOD(std_setcameratarget)
{
    v_EntityObject* pEnt;
    
    if (!PyArg_ParseTuple(args,"O:SetCameraTarget",&pEnt))
        return NULL;
    
    if ((PyObject*)pEnt==Py_None)
    {
        pEngine->pCameratarget=0;
        Py_XDECREF(pCameratarget);
        pCameratarget=0;
    }
    else
    {
        if (pEnt->ob_type!=&entitytype)
        {
            PyErr_SetString(PyExc_TypeError,"SetCameraTarget not called with entity/None object");
            return NULL;
        }
        
        pEngine->pCameratarget=pEnt->pEnt;  // oops
        
        Py_INCREF(pEnt);
        Py_XDECREF(pCameratarget);
        pCameratarget=(PyObject*)pEnt;
    }
    
    Py_INCREF(Py_None);
    return Py_None;
}

METHOD(std_getcameratarget)
{
    if (!PyArg_ParseTuple(args,""))
        return NULL;
    
    if (!pCameratarget)
    {
        Py_INCREF(Py_None);
        return Py_None;
    }
    else
    {
        Py_INCREF(pCameratarget);
        return pCameratarget;
    }

    // execution never gets here
}
METHOD(std_entityat)
{
    int x,y,width,height;
    if (!PyArg_ParseTuple(args,"iiii|EntityAt",&x,&y,&width,&height))
        return 0;

    int x2=x+width;
    int y2=y+height;

    int count=0;
    PyObject* pKey=0;
    PyObject* pValue=0;
    while (PyDict_Next(pEntitydict,&count,&pKey,&pValue))
    {
        CEntity& ent=*((v_EntityObject*)pValue)->pEnt;
        if (x>ent.x+ent.pSprite->nHotw)    continue;
        if (y>ent.y+ent.pSprite->nHoth)    continue;
        if (x2<ent.x)    continue;
        if (y2<ent.y)    continue;
        
        Py_INCREF(pValue);
        return pValue;
    }
    
    Py_INCREF(Py_None);
    return Py_None;
}