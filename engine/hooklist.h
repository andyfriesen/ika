
#ifndef HOOKLIST_H
#define HOOKLIST_H

#include "types.h"
#include <list>

class HookList
{
private:
    typedef std::list<void*> List;

    List pHooks;
    List pKilllist;
    List pAddlist;

public:
    void Add(void* p)
    {
        pAddlist.push_back(p);
    }

    void Remove(void* p)
    {
        pKilllist.push_back(p);
    }

    void Flush()
    {
        for (List::iterator i=pAddlist.begin(); i!=pAddlist.end(); i++)
        {
            void* p=*i;
            pHooks.push_back(p);
        }

        if (pKilllist.size())
        {
            for (List::iterator j=pKilllist.begin(); j!=pKilllist.end(); j++)
            {
                void* p=*j;
                pHooks.remove(p);
            }
        }

        pAddlist.clear();
        pKilllist.clear();
    }

    inline List::iterator begin()
    {
        return pHooks.begin();
    }

    inline List::iterator end()
    {
        return pHooks.end();
    }

    void Clear()
    {
        pHooks.clear();
        pKilllist.clear();
        pAddlist.clear();
    }
};

#endif