#ifndef REFCOUNT_H
#define REFCOUNT_H

struct RefCounted
{
    RefCounted()
        : _refCount(0)
    {}

    virtual ~RefCounted() {}

    void ref()
    {
        _refCount++;
    }

    void unref()
    {
        if (!--_refCount)
            delete this;
    }

    inline uint getRefCount() const { return _refCount; }

private:
    uint _refCount;
};

// RefPtr if I ever feel like it.  meh.
// I'm usually using refcounting on a pretty coarse level, like high-level resource allocation.
// Screw it.

#endif