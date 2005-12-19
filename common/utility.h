/**
 * Things that just don't fit anywhere else, but are handy to have around.
 * It is here (and in common/utility.h) that the suckiness of C++ is mitigated.
 *
 * Not ika specific at all.  Handy anywhere C++ is. (and I am, heh)
 */

#pragma once

#include <algorithm>
#include <cassert>
#include <string>
#include <vector>

#include "foreach.hpp"

#include "port.h"

#ifdef __GNUC__
using namespace std;
#endif

template <typename T, int N>
int lengthof(const T (&array)[N]) {
    return N;
}

// debug_cast<T>(U* u)
// In debug mode, asserts that the cast is good via dynamic_cast,
// if RTTI is enabled.
// Else, static cast all the way.
#if defined(_DEBUG) && defined(_CPPRTTI)
    template <typename ToType, typename FromType>
    ToType debug_cast(FromType from, ToType = 0) {
        if (from == 0) {
            return 0;
        } else {
            ToType result = dynamic_cast<ToType>(from);
            assert(result != 0);
            return result;
        }
    }
#else
    template <typename ToType, typename FromType>
    ToType debug_cast(FromType from, ToType = 0) {
        return static_cast<ToType>(from);
    }
#endif

template <bool B>
struct static_assert {
    int static_assert_failed[B ? 1 : -1];
};

#ifndef __GNUC__
static_assert<sizeof(u8) == 1>;
static_assert<sizeof(u16) == 2>;
static_assert<sizeof(u32) == 4>;
static_assert<sizeof(u64) == 8>;
#endif

// This really belongs in the language anyway, so fuckit.
#define foreach BOOST_FOREACH

// Tempted, but probably a bad idea:
//#pragma warning (disable:4706) // assignment in if statement

using std::min;
using std::max;

// Dummy-template for template functions which do not recieve arguments.
// Usage:  template <typename T> void blahFunc(Type2Type<T> = Type2Type<T>()) { ... }
// (TODO: get Modern C++ Design and grok it)
template<typename T>
struct Type2Type {
    typedef T Type;
};

#ifndef __GNUC__
// note: g++ STL already has a swap(), it will use that instead
template <typename T>
inline void swap(T& a, T& b) {
    T c;
    c = a;
    a = b;
    b = c;
}
#endif //__GNUC__

// Clamps val such that
// lower <= val <= toUpper
// Guaranteed to do weird things if the minimum is bigger than the maximum.
template <typename T>
inline T clamp(T val, T lower, T toUpper) {
         if (val < lower) return lower;
    else if (val > toUpper) return toUpper;
    else return val;
}

// Neato singleton helper function.  Chad Austin's idea.
template <typename T>
inline T* the() {
    return T::getInstance();
}

// Simple little predicate for for_each
template <typename T>
struct Destroy {
    void operator()(T* t) { delete t; }
};

bool isPowerOf2(uint i);
uint nextPowerOf2(uint i);

int sgn(int x);
void SeedRandom();
int Random(int min, int max);
char* va(const char* format, ...);

// String utilities
const std::string trim(const std::string& s);
std::vector<std::string> split(const std::string& delimiter, const std::string& s);
std::string join(const std::string& delimiter, const std::vector<std::string>& s);

std::string toLower(const std::string& s);
std::string toUpper(const std::string& s);

std::string toString(int i);
bool isHexNumber(const std::string& s);
uint hexToInt(const std::string& s);
//--

/// Pointer that deletes its datum when it dies.
template <typename T>
struct ScopedPtr {
    ScopedPtr(T* t)  : _data(t) {}
    ScopedPtr()      : _data(0) {}
    ~ScopedPtr()     { delete _data;  }

    T* operator ->() const { return  _data; }
    T& operator * () const { return *_data; }
    T* get()         const { return  _data; }

    operator bool () { return _data != 0; }

    ScopedPtr& operator = (T* t) {
        delete _data;
        _data = t;
        return *this;
    }

private:
    T* _data;
    ScopedPtr(ScopedPtr&){} // no copy!
};

/// Same as ScopedPtr, except it uses delete[]
template <typename T>
struct ScopedArray {
    ScopedArray(T* t)  : _data(t) {}
    ScopedArray()      : _data(0) {}
    ~ScopedArray()     { delete[] _data;  }

    T& operator[](uint i) const { return _data[i]; }
    T* operator ->()      const { return  _data; }
    T& operator * ()      const { return *_data; }
    T* get()              const { return  _data; }

    operator bool()       const { return _data != 0; }

    ScopedArray& operator = (T* t) {
        delete[] _data;
        _data = t;
        return *this;
    }

private:
    T* _data;
    ScopedArray(ScopedArray&){} // no copy!
};

#if 0
    /*
     * Helper class for creating types that cannot be derived.
     * Source: http://www.codeguru.com/Cpp/Cpp/cpp_mfc/stl/article.php/c4143/
     * Usage:
     *
     * struct MyFinalClass : virtual FinalClass<MyFinalClass> { ... };
     *
     * Does not work in g++.  Only works in MSVC because it specifically
     * defies the standard.
     *
     * C++ is stupid.
     */
    template <typename T>
    struct FinalClass {
    private:
        ~FinalClass() { }
        friend class T;
    };
#endif

namespace Path {
    // ifdef and blah blah for platform independance
#ifdef _WIN32
    const std::string delimiters = "\\/";
#else // assume Unix
    const std::string delimiters = "/";
#endif

    // strips the path from the filename, and returns it.
    // If relativeto is not specified or is empty, then the entire path is returned.
    // If it is specified, then the path returned is relative to the path contained therein.
    std::string getDirectory(const std::string& s);

    std::string getFileName(const std::string& s);           // same idea, but just returns the filename

    std::string getExtension(const std::string& s);          // returns the extension. (excluding the period)
    std::string replaceExtension(const std::string& s, const std::string& extension); // swaps the extension, and returns the result

    bool equals(const std::string& s, const std::string& t); // returns true if the two paths are the same.  Disregards case in win32.
};
