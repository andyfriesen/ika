/*
Python entity interface.

I'm not sure if I'm %100 happy with this setup.  Entities are *only* destroyed through Python's GC, which means that
entities can be made "persistent" simply by holding on to a reference to them.  Additionally, there is an implicitly
defined dictionary that holds all the map-defined entities. (and serves as their "life line", to keep them from
being garbage collected)    
*/

#include "ObjectDefs.h"
#include "main.h"
#include "Entity.h"

namespace Script
{
    namespace Entity
    {
        PyTypeObject type; 
        
        PyMethodDef methods [] =
        {
            {    "Move",            (PyCFunction)Entity_Move,             1    },
            {    "Chase",           (PyCFunction)Entity_Chase,            1    },
            {    "Wander",          (PyCFunction)Entity_Wander,           1    },
            {    "WanderZone",      (PyCFunction)Entity_Wanderzone,       1    },
            {    "Stop",            (PyCFunction)Entity_Stop,             1    },
            {    "IsMoving",        (PyCFunction)Entity_IsMoving,         1    },
            {    "DetectCollision", (PyCFunction)Entity_DetectCollision,  1    },
            {    NULL,              NULL    },                        // end of list
        };

        void Init()
        {
            memset(&type, 0, sizeof type);

            type.ob_refcnt=1;
            type.ob_type=&PyType_Type;
            type.tp_name="Entity";
            type.tp_basicsize=sizeof(EntityObject);
            type.tp_dealloc=(destructor)Destroy;
            type.tp_getattr=(getattrfunc)GetAttr;
            type.tp_setattr=(setattrfunc)SetAttr;
            type.tp_doc="Represents an entity in the ika game engine.";
        }

        PyObject* New(CEntity* e)
        {
            EntityObject* ent=PyObject_New(EntityObject,&type);
            if (!ent)
                return NULL;

            ent->ent=e;

            return (PyObject*)ent;
        }

        // This is much more complicated than it should be.
        PyObject* New(PyObject* self, PyObject* args)
        {
            int x, y;
            char* spritename;

            if (!PyArg_ParseTuple(args, "iis:Entity", &x, &y, &spritename))
                return 0;

            CSprite* sprite = engine->sprite.Load(spritename, engine->video);
            if (!sprite)
            {
                PyErr_SetString(PyExc_OSError, va("Couldn't load sprite file %s", spritename));
                return 0;
            }

            EntityObject* ent = PyObject_New(EntityObject, &type);

            ent->ent = engine->SpawnEntity();
            ent->ent->x = x;
            ent->ent->y = y;
            ent->ent->pSprite = sprite;

            return (PyObject*)ent;
         }

        void Destroy(EntityObject* self)
        {
            if (self->ent)
                engine->DestroyEntity(self->ent);
            else
                Log::Write("Entity_Destroy weirdness");

            PyObject_Del(self);
        }

        PyObject* GetAttr(EntityObject* self,char* name)
        {
            // Get a pointer to the entity, to make the below code nicer. :)
            CEntity& ent=*self->ent;

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

            return Py_FindMethod(methods, (PyObject*)self, name);                // this would appear to be the default behaviour.  Too bad we can't use it.*/
        }

        int SetAttr(EntityObject* self,char* name,PyObject* value)
        {
            // Get a pointer to the entity, to make the below code nicer. :)
            CEntity& ent=*self->ent;

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
                engine->sprite.Free(ent.pSprite);

                ent.pSprite=engine->sprite.Load(PyString_AsString(value), engine->video);
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

#define METHOD(x) PyObject* x(EntityObject* self, PyObject* args)

        METHOD(Entity_Move)
        {
            char* sPattern;

            if (!PyArg_ParseTuple(args,"s:Entity.Move",&sPattern))
                return NULL;

            CEntity& ent=*self->ent;
            ent.movecode=mc_script;
            ent.SetMoveScript(sPattern);

            Py_INCREF(Py_None);
            return Py_None;
        }

        METHOD(Entity_Chase)
        {
            int nDistance=0;
            EntityObject* pChasetarget;

            if (!PyArg_ParseTuple(args,"O!|i:Entity.Chase",&type,&pChasetarget,&nDistance))
                return NULL;

            CEntity& ent=*self->ent;
            ent.movecode=mc_chase;
            ent.pChasetarget=pChasetarget->ent;
            ent.nMinchasedist=nDistance;

            Py_INCREF(Py_None);
            return Py_None;
        }

        METHOD(Entity_Wander)
        {
            int nSteps,nDelay;
            int x1=-1,y1=-1,x2=-1,y2=-1;

            if (!PyArg_ParseTuple(args,"ii|iiii:Entity.Wander",&nSteps,&nDelay,&x1,&y1,&x2,&y2))
                return NULL;

            CEntity& ent=*self->ent;
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

        METHOD(Entity_Wanderzone)
        {
            int nSteps,nDelay;
            int nZone;

            if (!PyArg_ParseTuple(args,"iii:Entity.Wander",&nSteps,&nDelay,&nZone))
                return NULL;

            CEntity& ent=*self->ent;
            ent.nWandersteps=nSteps;
            ent.nWanderdelay=nDelay;
            ent.nWanderzone=nZone;

            Py_INCREF(Py_None);
            return Py_None;
        }

        METHOD(Entity_Stop)
        {
            if (!PyArg_ParseTuple(args,""))
                return NULL;

            self->ent->movecode=mc_nothing;

            Py_INCREF(Py_None);
            return Py_None;
        }

        METHOD(Entity_IsMoving)
        {
            if (!PyArg_ParseTuple(args,""))
                return NULL;

            CEntity& ent=*self->ent;    

            return PyInt_FromLong(ent.bMoving?1:0);
        }

        METHOD(Entity_DetectCollision)
        {
            if (!PyArg_ParseTuple(args,""))
                return NULL;

            const CEntity& e1=*self->ent;

            int x1=e1.x;
            int y1=e1.y;
            int x2=x1+e1.pSprite->nHotw;
            int y2=y1+e1.pSprite->nHoth;

            int nCount=0;
            PyObject* pKey=NULL;
            PyObject* pValue=NULL;
            while (PyDict_Next(entitydict, &nCount, &pKey, &pValue))
            {
                CEntity& e2=*((EntityObject*)pValue)->ent;
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
/*
        METHOD(std_spawnentity)
        {
            int x,y;
            char* chrname;

            if (!PyArg_ParseTuple(args,"iis:Entity_new",&x,&y,&chrname))
                return NULL;

            if (!File::Exists(chrname))
                engine->Sys_Error(va("Could not load %s",chrname));

            CEntity* pEnt=engine->SpawnEntity();
            CSprite* pSprite=engine->sprite.Load(chrname, engine->video);

            pEnt->sName="Spawned entity";
            pEnt->pSprite=pSprite;
            pEnt->x=x;
            pEnt->y=y;

            EntityObject* ent=PyObject_New(EntityObject,&type);
            if (!ent)
                return NULL;

            ent->ent=pEnt;

            return (PyObject*)ent;
        }

        METHOD(std_processentities)
        {
            if (!PyArg_ParseTuple(args,""))
                return NULL;

            engine->ProcessEntities();

            Py_INCREF(Py_None);
            return Py_None;
        }

        METHOD(std_setplayer)                                            // FIXME?  Is there a more intuitive way to do this?
        {
            EntityObject* ent;

            if (!PyArg_ParseTuple(args,"O:SetPlayerEntity",&ent))
                return NULL;

            if ((PyObject*)ent==Py_None)
            {
                Py_XDECREF(pPlayerent);
                pPlayerent=0;
                engine->pPlayer=0;
            }
            else
            {
                if (ent->ob_type!=&type)
                {
                    PyErr_SetString(PyExc_TypeError,"SetPlayerEntity not called with entity object or None.");
                    return NULL;
                }

                Py_INCREF(ent);
                Py_XDECREF(pPlayerent);
                pPlayerent=(PyObject*)ent;

                engine->pPlayer=ent->ent;
                ent->ent->movecode=mc_nothing;
            }

            PyObject* result=std_setcameratarget(self,args);
            Py_XDECREF(result);

            Py_INCREF(Py_None);
            return Py_None;
        }

        METHOD(std_setcameratarget)
        {
            EntityObject* pEnt;

            if (!PyArg_ParseTuple(args,"O:SetCameraTarget",&pEnt))
                return NULL;

            if ((PyObject*)pEnt==Py_None)
            {
                engine->pCameratarget=0;
                Py_XDECREF(pCameratarget);
                pCameratarget=0;
            }
            else
            {
                if (pEnt->ob_type!=&type)
                {
                    PyErr_SetString(PyExc_TypeError,"SetCameraTarget not called with entity/None object");
                    return NULL;
                }

                engine->pCameratarget=pEnt->ent;  // oops

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
                CEntity& ent=*((EntityObject*)pValue)->ent;
                if (x>ent.x+ent.pSprite->nHotw)    continue;
                if (y>ent.y+ent.pSprite->nHoth)    continue;
                if (x2<ent.x)    continue;
                if (y2<ent.y)    continue;

                Py_INCREF(pValue);
                return pValue;
            }

            Py_INCREF(Py_None);
            return Py_None;
        }*/
    }
}