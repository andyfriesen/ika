
#ifndef HOOKLIST_H
#define HOOKLIST_H

#include "types.h"
#include <list>

class HookList
{
    typedef std::list<void*> List;

    // public 'cause I'm lazy.
private:
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
            pHooks.push_back(*i);
        }

        for (List::iterator j=pKilllist.begin(); j!=pKilllist.end(); j++)
        {
            pHooks.erase(j);
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