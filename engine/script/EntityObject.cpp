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

#define GET(x) PyObject* get ## x(EntityObject* self)
#define SET(x) PyObject* set ## x(EntityObject* self, PyObject* value)
            GET(X)                  { return PyInt_FromLong(self->ent->x); }
            GET(Y)                  { return PyInt_FromLong(self->ent->y); }
            GET(Speed)              { return PyInt_FromLong(self->ent->nSpeed); }
            GET(Direction)          { return PyInt_FromLong(self->ent->direction); }
            GET(CurFrame)           { return PyInt_FromLong(self->ent->nCurframe); }
            GET(SpecFrame)          { return PyInt_FromLong(self->ent->nSpecframe); }
            GET(Visible)            { return PyInt_FromLong(self->ent->bVisible?1:0); }
            GET(Name)               { return PyString_FromString(self->ent->sName.c_str()); }
            GET(ActScript)          { return PyString_FromString(self->ent->sActscript.c_str()); }
            GET(AdjacentActivate)   { return PyInt_FromLong(self->ent->bAdjacentactivate?1:0); }
            GET(IsObs)              { return PyInt_FromLong(self->ent->bIsobs?1:0); }
            GET(MapObs)             { return PyInt_FromLong(self->ent->bMapobs?1:0); }
            GET(EntObs)             { return PyInt_FromLong(self->ent->bEntobs?1:0); }
            GET(Sprite)             { return PyString_FromString(self->ent->pSprite->sFilename.c_str()); }
            GET(HotX)               { return PyInt_FromLong(self->ent->pSprite->nHotx); }
            GET(HotY)               { return PyInt_FromLong(self->ent->pSprite->nHoty); }
            GET(HotWidth)           { return PyInt_FromLong(self->ent->pSprite->nHotw); }
            GET(HotHeight)          { return PyInt_FromLong(self->ent->pSprite->nHoth); }
            SET(X)                  { self->ent->x = PyInt_AsLong(value); return 0; }
            SET(Y)                  { self->ent->y = PyInt_AsLong(value); return 0; }
            SET(Speed)              { self->ent->nSpeed = PyInt_AsLong(value); return 0; }
            SET(Direction)          { self->ent->direction = (Direction)PyInt_AsLong(value);    return 0; } // I dislike this 
            SET(SpecFrame)          { self->ent->nSpecframe = PyInt_AsLong(value); return 0; }
            SET(Visible)            { self->ent->bVisible = PyInt_AsLong(value)!=0 ; return 0; }
            SET(Name)               { self->ent->sName = PyString_AsString(value); return 0; }
            SET(ActScript)          { self->ent->sActscript = PyString_AsString(value); return 0; }
            SET(AdjacentActivate)   { self->ent->bAdjacentactivate = (PyInt_AsLong(value) != 0); return 0; }
            SET(IsObs)              { self->ent->bIsobs = (PyInt_AsLong(value)!=0) ; return 0; }
            SET(MapObs)             { self->ent->bMapobs = (PyInt_AsLong(value)!=0) ; return 0; }
            SET(EntObs)             { self->ent->bEntobs = (PyInt_AsLong(value)!=0) ; return 0; }
            SET(Sprite)
            {
                engine->sprite.Free(self->ent->pSprite);

                self->ent->pSprite=engine->sprite.Load(PyString_AsString(value), engine->video);
                self->ent->SetAnimScript(va("F%i",self->ent->nCurframe));
                return 0;
            }
#undef SET
#undef GET

        PyGetSetDef properties[] =
        {
            {   "x",                (getter)getX,                   (setter)setX,               "Gets or sets the entity's X position. (in pixels)" },
            {   "y",                (getter)getY,                   (setter)setY,               "Gets or sets the entity's Y position. (in pixels)" },
            {   "speed",            (getter)getSpeed,               (setter)setSpeed,           "Gets or sets the entity's speed, in pixels/second" },
            {   "direction",        (getter)getDirection,           (setter)setDirection,       "Gets or sets the entity's direction"   },
            {   "curframe",         (getter)getCurFrame,            0,                          "Gets the entity's currently displayed frame"   },
            {   "specframe",        (getter)getSpecFrame,           (setter)setSpecFrame,       "If nonzeno, this frame is displayed instead of the normal animation" },
            {   "visible",          (getter)getVisible,             (setter)setVisible,         "If nonzero, the entity is drawn when onscreen" },
            {   "name",             (getter)getName,                (setter)setName,            "Gets or sets the entity's name.  This is more or less for your own convenience only."  },
            {   "actscript",        (getter)getActScript,           (setter)setActScript,       "Gets or sets the name of the function called when the entity is activated."    },
            {   "adjacentactivate", (getter)getAdjacentActivate,    (setter)setAdjacentActivate,"If nonzero, the entity will activate when it touches the player entity. (not implemented)" },
            {   "isobs",            (getter)getIsObs,               (setter)setIsObs,           "If nonzero, the entity will obstruct other entities."  },
            {   "mapobs",           (getter)getMapObs,              (setter)setMapObs,          "If nonzero, the entity is unable to walk on obstructed areas of the map."  },
            {   "entobs",           (getter)getEntObs,              (setter)setEntObs,          "If nonzero, the entity is unable to walk through entities whose isobs property is set."    },
            {   "sprite",           (getter)getSprite,              (setter)setSprite,          "Gets or sets the sprite used to display the entity."   },
            {   "hotx",             (getter)getHotX,                0,                          "Gets the X position of the entity's hotspot."  },
            {   "hoty",             (getter)getHotY,                0,                          "Gets the Y position of the entity's hotspot."  },
            {   "hotwidth",         (getter)getHotWidth,            0,                          "Gets the width of the entity's hotspot."  },
            {   "hotheight",        (getter)getHotHeight,           0,                          "Gets the height of the entity's hotspot."  },
            {   0   }
        };

        void Init()
        {
            memset(&type, 0, sizeof type);

            type.ob_refcnt=1;
            type.ob_type=&PyType_Type;
            type.tp_name="Entity";
            type.tp_basicsize=sizeof(EntityObject);
            type.tp_dealloc=(destructor)Destroy;
            type.tp_methods = methods;
            type.tp_getset = properties;
            type.tp_doc="Represents an entity in the ika game engine.";

            PyType_Ready(&type);
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