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

#include "boost/foreach.hpp"

#if defined(_MSC_VER)
#   define DEPRECATED __declspec(deprecated)
#else
#   define DEPRECATED
#endif

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

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

#if defined(_MSC_VER) && _MSC_VER < 1300
    // Silence stupid not-really-a-warning. (identifier too long for debugger hurk blah blah etc)
#   pragma warning (disable:4786)                      
    // Fix broken for() scoping in VC6
#   define for if (0); else for
#endif

#if _MSC_VER
    typedef __int64 s64;
    typedef unsigned __int64 u64;
#elif __GNUC__
    typedef long long s64;
    typedef unsigned long long u64;
#endif

// signed/unsigned ints of whatever size.
typedef size_t uint;
typedef ptrdiff_t sint;

// There was a template version of this, but it was useless anyway.
// It didn't work for locally defined arrays.
#define lengthof(x) (sizeof (x) / sizeof (x[0]))

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

template <typename T>
inline void swap(T& a, T& b) {
    T c;
    c = a;
    a = b;
    b = c;
}

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
template <class T>
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
template <class T>
class ScopedPtr {
    T* _data;
    ScopedPtr(ScopedPtr&){} // no copy!

public:
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
};

/// Same as ScopedPtr, except it uses delete[]
template <class T>
class ScopedArray {
    T* _data;
    ScopedArray(ScopedArray&){} // no copy!

public:
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
};

/*
 * Helper class for creating types that cannot be derived.
 * Source: http://www.codeguru.com/Cpp/Cpp/cpp_mfc/stl/article.php/c4143/
 * Usage:
 *
 * struct MyFinalClass : virtual FinalClass<MyFinalClass> { ... };
 */
template <typename T>
struct FinalClass {
private:
    ~FinalClass() { }
    friend class T;
};

namespace Path {
    // ifdef and blah blah for platform independance
#ifdef _WIN32
    const std::string delimiters = "\\/";
#else // assume unix
    const std::string delimiters = '/';
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

