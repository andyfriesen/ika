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
        int nEnt;
    
    //	PyObject* pDict;
};

PyMethodDef CScriptEngine::entity_methods[] =
{
    {	"Move",		CScriptEngine::entity_move,		1	},
    {	"Chase",	CScriptEngine::entity_chase,	1	},
    {	"Wander",	CScriptEngine::entity_wander,	1	},
    {	"WanderZone",	CScriptEngine::entity_wanderzone,	1	},
    {	"Stop",		CScriptEngine::entity_stop,		1	},
    {	"IsMoving",	CScriptEngine::entity_ismoving,	1	},
    {	"DetectCollision",	CScriptEngine::entity_detectcollision,	1	},
    {	NULL,		NULL	},						// end of list
};

PyTypeObject CScriptEngine::entitytype;

void CScriptEngine::Init_Entity()
{
    ZeroMemory(&entitytype,sizeof entitytype);
    
    entitytype.ob_refcnt=1;
    entitytype.ob_type=&PyType_Type;
    entitytype.tp_name="Entity";
    entitytype.tp_basicsize=sizeof(v_EntityObject);
    entitytype.tp_dealloc=(destructor)Entity_Destroy;
    entitytype.tp_getattr=(getattrfunc)Entity_GetAttribute;
    entitytype.tp_setattr=(setattrfunc)Entity_SetAttribute;
}

PyObject* CScriptEngine::Entity_New(PyObject* self,PyObject* args)
{
    int nEnt=-1;
    
    if (!PyArg_ParseTuple(args,"i:Entity_New",&nEnt))
        return NULL;
    
    v_EntityObject* ent=PyObject_New(v_EntityObject,&entitytype);
    if (!ent)
        return NULL;
    
    ent->nEnt=nEnt;
    
    return (PyObject*)ent;
}

void CScriptEngine::Entity_Destroy(PyObject* self)
{
    int nEnt=((v_EntityObject*)self)->nEnt;

    pEngine->sprite.Free(pEngine->entities[nEnt].pSprite);

    if (nEnt>=0)
        pEngine->entities.Free(nEnt);
    ((v_EntityObject*)self)->nEnt=-1;
    
    PyObject_Del(self);
}

PyObject* CScriptEngine::Entity_GetAttribute(PyObject* self,char* name)
{
    // Get a pointer to the entity, to make the below code nicer. :)
    CEntity* pEnt=&pEngine->entities[((v_EntityObject*)self)->nEnt];
    
    // I wonder how heavy a toll this exacts.  Maybe replace it with a hash table.
    if (!strcmp(name,"x"))
        return PyInt_FromLong(pEnt->x);
    if (!strcmp(name,"y"))
        return PyInt_FromLong(pEnt->y);
    
    if (!strcmp(name,"speed"))
        return PyInt_FromLong(pEnt->nSpeed);
    
    if (!strcmp(name,"direction"))
        return PyInt_FromLong(pEnt->facing);
    
    if (!strcmp(name,"curframe"))
        return PyInt_FromLong(pEnt->nCurframe);
    if (!strcmp(name,"specframe"))
        return PyInt_FromLong(pEnt->nSpecframe);
    if (!strcmp(name,"visible"))
        return PyInt_FromLong(pEnt->bVisible?1:0);
    if (!strcmp(name,"name"))
        return PyString_FromString(pEnt->sName.c_str());
    if (!strcmp(name,"actscript"))
        return PyString_FromString(pEnt->sActscript.c_str());
    if (!strcmp(name,"adjacentactivate"))
        return PyInt_FromLong(pEnt->bAdjacentactivate?1:0);
    
    if (!strcmp(name,"isobs"))
        return PyInt_FromLong(pEnt->bIsobs?1:0);
    if (!strcmp(name,"mapobs"))
        return PyInt_FromLong(pEnt->bMapobs?1:0);
    if (!strcmp(name,"entobs"))
        return PyInt_FromLong(pEnt->bEntobs?1:0);
    
    if (!strcmp(name,"sprite"))
        return PyString_FromString(pEnt->pSprite->sFilename.c_str());
    if (!strcmp(name,"hotx"))
        return PyInt_FromLong(pEnt->pSprite->nHotx);
    if (!strcmp(name,"hoty"))
        return PyInt_FromLong(pEnt->pSprite->nHoty);
    if (!strcmp(name,"hotwidth"))
        return PyInt_FromLong(pEnt->pSprite->nHotw);
    if (!strcmp(name,"hotheight"))
        return PyInt_FromLong(pEnt->pSprite->nHoth);
    
    return Py_FindMethod(entity_methods,self,name);				// this would appear to be the default behaviour.  Too bad we can't use it.
}

int CScriptEngine::Entity_SetAttribute(PyObject* self,char* name,PyObject* value)
{
    // Get a pointer to the entity, to make the below code nicer. :)
    CEntity* pEnt=&pEngine->entities[((v_EntityObject*)self)->nEnt];
    
    if (!strcmp(name,"x"))
        pEnt->x=PyInt_AsLong(value);
    if (!strcmp(name,"y"))
        pEnt->y=PyInt_AsLong(value);
    
    if (!strcmp(name,"speed"))
        pEnt->nSpeed=PyInt_AsLong(value);
    
    if (!strcmp(name,"direction"))
        pEnt->facing=(Direction)PyInt_AsLong(value);	// I dislike this
    
    if (!strcmp(name,"curframe"))						// you's isn't allowed to alter this one
    {
        PyErr_SetString(PyExc_TypeError,"Entity.curframe is read only!");
        return -1;
    }
    if (!strcmp(name,"specframe"))
        pEnt->nSpecframe=PyInt_AsLong(value);
    if (!strcmp(name,"visible"))
        pEnt->bVisible= PyInt_AsLong(value)!=0 ;
    if (!strcmp(name,"name"))
        pEnt->sName=PyString_AsString(value);
    if (!strcmp(name,"actscript"))
        pEnt->sActscript=PyString_AsString(value);
    if (!strcmp(name,"adjacentactivate"))
        pEnt->bAdjacentactivate=(PyInt_AsLong(value)!=0);
    
    
    if (!strcmp(name,"isobs"))
        pEnt->bIsobs= (PyInt_AsLong(value)!=0) ;
    if (!strcmp(name,"mapobs"))
        pEnt->bMapobs= (PyInt_AsLong(value)!=0) ;
    if (!strcmp(name,"entobs"))
        pEnt->bEntobs= (PyInt_AsLong(value)!=0) ;
    
    if (!strcmp(name,"sprite"))
    {
        pEngine->sprite.Free(pEnt->pSprite);

        pEnt->pSprite=pEngine->sprite.Load(PyString_AsString(value));
        pEnt->SetAnimScript(va("F%i",pEnt->nCurframe),-1);
    }
    if (!strcmp(name,"hotx") || !strcmp(name,"hoty") || 
        !strcmp(name,"hotwidth") || !strcmp(name,"hotheight"))
    {
        PyErr_SetString(PyExc_SyntaxError,va("Entity.%s is read-only",name));
        return -1;
    }	
    
    // FIXME: you can't add properties yet
    return 0;
    
    //	return PyDict_SetItem(self->pDict,name,value);
}

METHOD(entity_move)
{
    char* sPattern;
    
    if (!PyArg_ParseTuple(args,"s:Entity.Move",&sPattern))
        return NULL;
    
    int nEnt=((v_EntityObject*)self)->nEnt;
    if (pEngine->entities.IsValid(nEnt))
    {
        pEngine->entities[nEnt].movecode=mc_script;
        pEngine->entities[nEnt].SetMoveScript(sPattern);
    }
    else
        log("entity.move: entity gimpiness.  Must make non-gay.");
    
    Py_INCREF(Py_None);
    return Py_None;
}

METHOD(entity_chase)
{
    int nDistance=0;
    v_EntityObject* pChasetarget;
    
    if (!PyArg_ParseTuple(args,"O!|i:Entity.Chase",&entitytype,&pChasetarget,&nDistance))
        return NULL;
    
    int nEnt=((v_EntityObject*)self)->nEnt;
    if (pEngine->entities.IsValid(nEnt))
    {
        pEngine->entities[nEnt].movecode=mc_chase;
        pEngine->entities[nEnt].nEntchasetarget=pChasetarget->nEnt;
        pEngine->entities[nEnt].nMinchasedist=nDistance;
    }
    else
        log("entity.chase: entity gimpiness.  Must make non-gay.");
    
    Py_INCREF(Py_None);
    return Py_None;
}

METHOD(entity_wander)
{
    int nSteps,nDelay;
    int x1=-1,y1=-1,x2=-1,y2=-1;
    
    if (!PyArg_ParseTuple(args,"ii|iiii:Entity.Wander",&nSteps,&nDelay,&x1,&y1,&x2,&y2))
        return NULL;
    
    int nEnt=((v_EntityObject*)self)->nEnt;
    if (pEngine->entities.IsValid(nEnt))
    {
        pEngine->entities[nEnt].nWandersteps=nSteps;
        pEngine->entities[nEnt].nWanderdelay=nDelay;
        
        if (x1==-1 || y1==-1 || x2==-1 || y2==-1)
            pEngine->entities[nEnt].movecode=mc_wander;
        else
        {
            pEngine->entities[nEnt].movecode=mc_wanderrect;
            pEngine->entities[nEnt].wanderrect.left=x1;
            pEngine->entities[nEnt].wanderrect.top=y1;
            pEngine->entities[nEnt].wanderrect.right=x2;
            pEngine->entities[nEnt].wanderrect.bottom=y2;
        }
    }
    else
        log("entity.wander/wanderrect: entity gayness.  Must make non-gay.");
    
    Py_INCREF(Py_None);
    return Py_None;
}

METHOD(entity_wanderzone)
{
    int nSteps,nDelay;
    int nZone;
    
    if (!PyArg_ParseTuple(args,"iii:Entity.Wander",&nSteps,&nDelay,&nZone))
        return NULL;
    
    int nEnt=((v_EntityObject*)self)->nEnt;
    if (pEngine->entities.IsValid(nEnt))
    {
        pEngine->entities[nEnt].nWandersteps=nSteps;
        pEngine->entities[nEnt].nWanderdelay=nDelay;
        pEngine->entities[nEnt].nWanderzone=nZone;
    }
    else
        log("entity.wanderzone: entity gayness.  Must make non-gay.");
    
    Py_INCREF(Py_None);
    return Py_None;
}

METHOD(entity_stop)
{
    if (!PyArg_ParseTuple(args,""))
        return NULL;
    
    int nEnt=((v_EntityObject*)self)->nEnt;
    if (pEngine->entities.IsValid(nEnt))
        pEngine->entities[nEnt].movecode=mc_nothing;
    else
        log("entity.stop: entity gayness.  Must make non-gay.");
    
    Py_INCREF(Py_None);
    return Py_None;
}

METHOD(entity_ismoving)
{
    if (!PyArg_ParseTuple(args,""))
        return NULL;
    
    int nEnt=((v_EntityObject*)self)->nEnt;	
    
    if (!pEngine->entities.IsValid(nEnt))
    {
        log("entity.IsMoving: gay!");
        return NULL;
    }
    
    return PyInt_FromLong(pEngine->entities[nEnt].bMoving?1:0);
}

METHOD(entity_detectcollision)
{
    if (!PyArg_ParseTuple(args,""))
        return NULL;
    
    CEntity* e1=&(pEngine->entities[((v_EntityObject*)self)->nEnt]);
    
    int x1=e1->x;
    int y1=e1->y;
    int x2=x1+e1->pSprite->nHotw;
    int y2=y1+e1->pSprite->nHoth;
    
    int nCount=0;
    PyObject* pKey=NULL;
    PyObject* pValue=NULL;
    while (PyDict_Next(pEntitydict,&nCount,&pKey,&pValue))
    {
        CEntity* e2=&(pEngine->entities[((v_EntityObject*)pValue)->nEnt]);
        
        if (x1>e2->x+e2->pSprite->nHotw)	continue;
        if (y1>e2->y+e2->pSprite->nHoth)	continue;
        if (x2<e2->x)	continue;
        if (y2<e2->y)	continue;
        
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
    
    int nEnt=pEngine->entities.GetNew();
    CSprite* pSprite=pEngine->sprite.Load(chrname);
    
    pEngine->entities[nEnt].sName="Spawned entity";
    pEngine->entities[nEnt].pSprite=pSprite;
    pEngine->entities[nEnt].x=x;
    pEngine->entities[nEnt].y=y;
    
    v_EntityObject* ent=PyObject_New(v_EntityObject,&entitytype);
    if (!ent)
        return NULL;
    
    ent->nEnt=nEnt;
    
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

METHOD(std_setplayer)											// FIXME?  Is there a more intuitive way to do this?
{
    v_EntityObject* ent;
    
    if (!PyArg_ParseTuple(args,"O:SetPlayerEntity",&ent))
        return NULL;
    
    if ((PyObject*)ent==Py_None)
        pEngine->player=-1;
    else
    {
        if (ent->ob_type!=&entitytype)
        {
            PyErr_SetString(PyExc_TypeError,"SetPlayerEntity not called with entity object or None.");
            return NULL;
        }
        
        pEngine->player=ent->nEnt;
        pEngine->entities[ent->nEnt].movecode=mc_nothing;
    }
    
    pEngine->hCameratarget=pEngine->player;
    
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
        pEngine->hCameratarget=-1;
        Py_XDECREF(pCameratarget);
        pCameratarget=NULL;
    }
    else
    {
        if (pEnt->ob_type!=&entitytype)
        {
            PyErr_SetString(PyExc_TypeError,"SetCameraTarget not called with entity/None object");
            return NULL;
        }
        
        pEngine->hCameratarget=pEnt->nEnt;
        
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
    
    return NULL; // !!
}