
#ifndef HOOKLIST_H
#define HOOKLIST_H

#include <list>
#include "common/types.h"
#include "scriptobject.h"

class HookList
{
public:
    typedef std::list<ScriptObject> List;
private:
    typedef std::list<void*> VoidList;

    List     _hooks;
    VoidList _killList;
    VoidList _addList;

public:
    void Add(void* p)
    {
        _addList.push_back(p);
    }

    void Remove(void* p)
    {
        _killList.push_back(p);
    }

    void Flush()
    {
        for (VoidList::iterator i = _addList.begin(); i != _addList.end(); i++)
        {
            void* p = *i;
            _hooks.push_back(ScriptObject(p));
        }

        if (_killList.size())
        {
            for (VoidList::iterator j = _killList.begin(); j != _killList.end(); j++)
            {
                void* p = *j;

                for (List::iterator k = _hooks.begin(); k != _hooks.end();)
                {
                    if (k->get() == p)
                        _hooks.erase(k++);
                    else
                        k++;
                }
            }
        }

        _addList.clear();
        _killList.clear();
    }

    inline List::iterator begin()
    {
        return _hooks.begin();
    }

    inline List::iterator end()
    {
        return _hooks.end();
    }

    void Clear()
    {
        _hooks.clear();
        _killList.clear();
        _addList.clear();
    }
};

#endif
