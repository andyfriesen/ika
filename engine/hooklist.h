
#ifndef HOOKLIST_H
#define HOOKLIST_H

#include <list>
#include "common/utility.h"
#include "scriptobject.h"

struct HookList {
    typedef std::list<ScriptObject>::iterator iterator;
    typedef std::list<ScriptObject>::const_iterator const_iterator;

    void add(void* p) {
        _addList.push_back(p);
    }

    void remove(void* p) {
        _killList.push_back(p);
    }

    void flush() {
        for (VoidList::iterator i = _addList.begin(); i != _addList.end(); i++) {
            void* p = *i;
            _hooks.push_back(ScriptObject(p));
        }

        if (_killList.size()) {
            for (VoidList::iterator j = _killList.begin(); j != _killList.end(); j++) {
                void* p = *j;

                iterator k = _hooks.begin();
                while (k != _hooks.end()) {
                    if (k->get() == p) {
                        _hooks.erase(k++);
                    } else {
                        k++;
                    }
                }
            }
        }

        _addList.clear();
        _killList.clear();
    }

    inline iterator begin() {
        return _hooks.begin();
    }

    inline iterator end() {
        return _hooks.end();
    }

    inline const_iterator begin()  const {
        return _hooks.begin();
    }

    inline const_iterator end() const {
        return _hooks.end();
    }

    void clear() {
        _hooks.clear();
        _killList.clear();
        _addList.clear();
    }

private:
    typedef std::list<ScriptObject> List;
    typedef std::list<void*> VoidList;

    List     _hooks;
    VoidList _killList;
    VoidList _addList;
};

#endif
 