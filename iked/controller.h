
/*
    This is a little icky, because it depends on some of the base class's members, among other things.

    Hope it doesn't cause problems.
*/

#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "types.h"
#include <map>

template <class T>
class CController
{
    struct Resource : public T
    {
        int nRefcount;
        string sName;

        Resource(const string& n)
            : T(), nRefcount(1),sName(n)
        {}

        virtual ~Resource()
        {
            //T::~T();  // wah, don't want to have to tweak existing classes. ;_;
        }
    };

    std::map<string,Resource*> resources;


    // specialize this!
    bool LoadFromFile(T* p,const string& name)
    {
        // default behaviour, just guess. ^_^
        bool result=p->Load(name.c_str());

        return result;
    }

public:

    T* Load(const string& name)
    {        
        Resource* pRsrc=resources[name];

        if (pRsrc)
        {
            pRsrc->nRefcount++;
            return pRsrc;
        }
         
        pRsrc=new Resource(name);

        bool result=LoadFromFile(pRsrc,name);
        if (!result)
        {
            delete pRsrc;
            return 0;
        }

        resources[name]=pRsrc;
        return pRsrc;
    }

    void Release(T* r)
    {
        Resource* pRsrc=(Resource*) r;

        pRsrc->nRefcount--;
        if (!pRsrc->nRefcount)
        {
            if (resources.find(pRsrc->sName)==resources.end())
            {
                return;
            }

            resources.erase(pRsrc->sName);
            delete pRsrc;
        }
    }
};

#endif