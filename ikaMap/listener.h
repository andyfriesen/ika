#ifndef LISTENER_H
#define LISTENER_H

#include <algorithm>
#include <set>
#include "misc.h"

// Shitty listeners.  Exactly one arg, though it can be parameterized.
template <typename T, typename ArgType>
struct Delegate
{
    typedef void (T::*Func)(ArgType);

    Delegate(T* inst, Func func)
        : _inst(inst)
        , _func(func)
    {}

    void operator()(ArgType arg)
    {
        (_inst->*_func)(arg);
    }

private:
    T* _inst;
    Func _func;
};

template <typename T, typename ArgType>
Delegate<T, ArgType> bind(T* inst, void (T::*func)(ArgType))
{ return Delegate<T, ArgType>(inst, func); }

namespace
{
    // Opaque callable thing.  Wraps anything with an operator()(ArgType)
    template <typename ArgType>
    struct CallableBase
    {
        void operator()(ArgType arg)
        {
            return call(arg);
        }

        virtual void call(ArgType arg) = 0;
    };
};

template <typename T, typename ArgType>
struct Callable : public CallableBase<ArgType>
{
    Callable(T subject)
        : _subject(subject)
    {}

    virtual void call(ArgType arg)
    {
        return _subject(arg);
    }

private:
    T _subject;
};

template <class ArgType>
struct Listener
{
    ~Listener()
    {
        std::for_each(_list.begin(), _list.end(), Destroy<CallableBase<ArgType> >());
    }

    template <typename T>
    void add(T t)
    {
        _list.insert(new Callable<T, ArgType>(t));
    }

    template <typename T>
    void add(T* inst, void (T::*func)(ArgType))
    {
        add(bind(inst, func));
    }

#if 0
    void operator()(ArgType arg)
    {
        return fire(arg);
    }
#endif

    void fire(ArgType arg)
    {
        for (CallSet::iterator
            iter = _list.begin();
            iter != _list.end();
            iter++)
        {
            (**iter)(arg);
        }
    }
    
private:
    typedef std::set<CallableBase<ArgType>*> CallSet;
    CallSet _list;
};

#endif