#pragma once

#include "common/utility.h"

struct RefCounted {
    RefCounted()
        : _refCount(0)
    {}

    virtual ~RefCounted() {}

    void ref() {
        _refCount++;
    }

    /// @returns true if the object died
    bool unref() {
        if (!--_refCount) {
            delete this;
            return true;
        } else {
            return false;
        }
    }

    inline uint getRefCount() const { 
        return _refCount; 
    }

private:
    uint _refCount;
};

/**
 * Refcounting smart pointer.
 * All RefPtr really cares about is that class T
 * has an ref() and a unref() method.
 * The easiest way to do this is to just inherit
 * RefCounted
 */
template <typename T>
struct RefPtr {
    /// Default Constructor.  Initializes to null.
    RefPtr() : data(0) {}

    /**
     * Initializes the RefPtr to p
     * @param p The data that the RefPtr will initially point to.
     */
    RefPtr(T* p) 
        : data(p)
    {
        if (data)
            data->ref();
    }

    template <typename U>
    RefPtr(U* p)
        : data(debug_cast<T*>(p))
    {
        if (data)
            data->ref();
    }

    //! Copy constructor
    RefPtr(const RefPtr& rhs) {
        data = rhs.get();

        if (data)
            data->ref();
    }

    // conversion
    template <typename U>
    RefPtr(const RefPtr<U>& rhs) {
        //data = debug_cast<T*>(rhs.get());
        data = rhs.get();
        if (data)
            data->ref();
    }

    ~RefPtr() {
        if (data)
            data->unref();
    }

    // explicit cast
    template <typename U>
    inline U* cast(U* = 0) {
        return debug_cast<U*>(data);
    }

    inline RefPtr& operator = (const RefPtr& rhs) {
        if (data)
            data->unref();

        data = rhs.get();

        if (data)
            data->ref();

        return *this;
    }

    inline T* get() const {
        return data;
    }

    inline T* operator -> () {
        return data;
    }

    inline const T* operator -> () const {
        return data;
    }

    inline operator T* () {
        return data;
    }

    inline operator bool() const {
        return data != 0;
    }

    inline bool operator !() const {
        return data == 0;
    }

    inline bool operator == (const T* rhs) const {
        return data == rhs;
    }

    inline bool operator == (const RefPtr& rhs) const {
        return data == rhs.get();
    }

    inline bool operator != (const RefPtr& rhs) const {
        return data != rhs.get();
    }

    template <typename U>
    inline bool operator == (const RefPtr<U>& rhs) const {
        return data == debug_cast<T>(rhs.get());
    }

    template <typename U>
    inline bool operator != (const RefPtr<U>& rhs) const {
        return data != debug_cast<T>(rhs.get());
    }

    // for std::map

    inline bool operator < (const RefPtr& rhs) const {
        return data < rhs.get();
    }

private:
    T* data;
};

