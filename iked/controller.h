
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

        Resource()
            : T(), nRefcount(1)
        {}
    };

    std::map<string,Resource*> resources;

public:

    Resource* Get(const string& name)
    {
        Resource* pRsrc=resources[name];

        if (pRsrc)
        {
            pRsrc->nRefcount++;
            return pRsrc;
        }
         
        pRsrc=new Resource;

        bool result=Load(pRsrc,name);
        if (!result)
        {
            delete pRsrc;
            return 0;
        }

        resources[name]=pRsrc;
        return pRsrc;
    }

    // specialize this!
    bool Load(T* p,const string& name);
};

#endif