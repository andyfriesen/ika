
#pragma once

#include <algorithm>
#include <set>

// Shitty listeners.  Exactly one arg, though it can be parameterized.

namespace {
    /**
     * This is where the rubber hits the road, as it were.  If we're going
     * to store a heterogenious collection of callable objects, we need some
     * way to store them all in a sequence.  This is how.  We inherit 
     * CallableBase, and store its subclasses.
     *
     * The Callable template below deals with most any object that has an
     * operator(), allowing us compile time parameterization as well as
     * runtime.  Delegate implements the same interface for efficiency
     * reasons; it could just as easily be wrapped by a Callable, but then
     * calling it involves three levels of virtual indirection for no reason.
     * blech.
     */
    template <typename ArgType>
    struct CallableBase {
        void operator()(ArgType arg) {
            return call(arg);
        }

        virtual void call(ArgType arg) = 0;
        virtual ~CallableBase() {}
    };
};

/**
 * Generic callable wrapper.
 * If it has an operator()(ArgType), this can wrap it, and provide the
 * CallableBase interface.
 */
template <typename T, typename ArgType>
struct Callable : CallableBase<ArgType> {
    Callable(T subject)
        : _subject(subject)
    {}

    virtual void call(ArgType arg) {
        return _subject(arg);
    }

private:
    T _subject;
};

/**
 * Wraps an object instance and a pointer-to-member into a single
 * callable object.  This also implements the CallableBase interface,
 * and as such can be stored in the Listener directly, without having
 * to use Callable as a third proxy.
 */
template <typename T, typename ArgType>
struct Delegate : CallableBase<ArgType> {
    typedef void (T::*Func)(ArgType);

    Delegate(T* inst, Func func)
        : _inst(inst)
        , _func(func)
    {}

    virtual void call(ArgType arg) {
        (_inst->*_func)(arg);
    }

private:
    T* _inst;
    Func _func;
};

/**
 *  Convenience method for creating a delegate.
 *  Template functions can infer the types of their arguments,
 *  so this is generally much easier to use than the Delegate
 *  constructor.
 */
template <typename T, typename ArgType>
Delegate<T, ArgType> bind(T* inst, void (T::*func)(ArgType)) { 
    return Delegate<T, ArgType>(inst, func); 
}

/**
 * Multiple dispatch thingie.
 * You feed it a bunch of callable objects, and it stores them all.
 * Each is called in succession when the fire() method is invoked.
 */
template <class ArgType>
struct Listener {
    ~Listener() {
        for (typename CallSet::iterator iter = _list.begin(); iter != _list.end(); iter++) {
            delete (*iter);
        }
    }

    template <typename T>
    void add(T t) {
        _list.insert(new Callable<T, ArgType>(t));
    }

    template <typename T>
    void add(T* inst, void (T::*func)(ArgType)) {
        _list.insert(new Delegate<T, ArgType>(inst, func));
    }

    void fire(ArgType arg) {
        for (typename CallSet::iterator
            iter = _list.begin();
            iter != _list.end();
            iter++
        ) {
            (**iter)(arg);
        }
    }
    
private:
    typedef std::set<CallableBase<ArgType>*> CallSet;
    CallSet _list;
};
