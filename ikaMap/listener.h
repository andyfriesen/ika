#ifndef LISTENER_H
#define LISTENER_H

#include <algorithm>
#include <set>
#include "misc.h"

// Shitty listeners.  Exactly one arg, though it can be parameterized.

namespace
{
    // Opaque callable thing.  Wraps anything with an operator()(ArgType)
    /*
     * This is where the rubber hits the road, as it were.  If we're going
     * to store a heterogenious collection of callable objects, we need some
     * way to store them all in a sequence.  This is how.  We inherit 
     * CallableBase, and store its subclasses.
     *
     * The Callable template below deals with most any object that has an
     * operator(), allowing us compile time parameterization as well as
     * runtime.  Delegate implements the same interface for efficiency
     * reasons; it could just as easily be wrapped by a Callable, but then
     * calling it involves three levels of virtual indirection.  blech.
     */
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
struct Callable : CallableBase<ArgType>
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

template <typename T, typename ArgType>
struct Delegate : CallableBase<ArgType>
{
    typedef void (T::*Func)(ArgType);

    Delegate(T* inst, Func func)
        : _inst(inst)
        , _func(func)
    {}

    virtual void call(ArgType arg)
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
        _list.insert(new Delegate<T, ArgType>(inst, func));
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