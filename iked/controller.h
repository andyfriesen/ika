
/*
    This code is a bit more complicated than I'd like.  And it searches for things the most obvious way possible. :x (optimize that
    if it becomes an issue)
*/

#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "types.h"
#include <list>

#include "log.h"

template <class T>
class CController
{
private:
    struct Resource
    {
        string  sFilename;
        int     nRefcount;
        T*      pData;

        Resource(const string& s="", T* d = 0) : sFilename(s), nRefcount(1), pData(d) {}
    };

    typedef std::list <Resource> ResourceList;

    ResourceList    rsrc;

    Resource* Find(const string& name)
    {
        for (ResourceList::iterator i = rsrc.begin(); i!=rsrc.end(); i++)
        {
            if (i->sFilename==name)
                return &*i;
        }

        return 0;
    }

    Resource* Find(const T* data)
    {
        for (ResourceList::iterator i = rsrc.begin(); i!=rsrc.end(); i++)
        {
            if (i->pData==data)
                return &*i;
        }

        return 0;
    }

    // Specialize this if T doesn't have a bool Load(const char* s) method.
    bool LoadFromFile(T* p, const string& name)
    {
        Log::Write("Loading      %s", name.c_str());
        return p->Load(name.c_str());
    }

public:

    T* Load(string name)
    {
#ifdef WIN32
        name = Upper(name);
#endif

        Resource* ri = Find(name);

        if (ri)
        {
            Log::Write("Addref       %s", name.c_str());
            ri->nRefcount++;
            return ri->pData;
        }
        else
        {
            T* pData = new T;
            if (!LoadFromFile(pData, name))
            {
                delete pData;
                return 0;
            }

            rsrc.push_back(Resource(name, pData));

            return pData;
        }
    }

    bool Release(T* data)
    {
        for (ResourceList::iterator i = rsrc.begin(); i!=rsrc.end(); i++)
        {
            if (i->pData==data)
            {
                i->nRefcount--;

                if (i->nRefcount == 0)
                {
                    Log::Write("Deallocating %s", i->sFilename.c_str());
                    delete i->pData;
                    rsrc.erase(i);
                    
                }
                else
                    Log::Write("Decref       %s", i->sFilename.c_str());

                return true;
            }
        }

        return false;
    }

    ~CController()
    {
        for (ResourceList::iterator i = rsrc.begin(); i!=rsrc.end(); i++)
        {
            Log::Write("Leak! %s", i->sFilename.c_str());
            delete i->pData;
        }
    }
};

#endif