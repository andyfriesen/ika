/*
  Things that just don't fit anywhere else, but are handy to have around.

  Some things are simple abstractions over common functions, to make everything
  all happy and portable. (kudos to aegis for the advice)
*/
#ifndef MISC_H
#define MISC_H

#include "types.h"
#include <string>
#include <algorithm>

extern const char* IKA_VERSION;

/*#ifdef min
#   undef min
#endif
#ifdef max
#   undef max
#endif*/

#if !defined(_MSC_VER) || _MSC_VER >= 1300
    // This totally rocks.
    template<typename T, size_t N>
    size_t lengthof(T (&)[N])
    {
        return N;
    }
#else
    // VC6 is homo.
#   define lengthof(x) sizeof x / sizeof x[0];
#endif

using std::min;
using std::max;

template <typename T>
inline void swap(T& a, T& b)
{
    T c;
    c = a;
    a = b;
    b = c;
}

template <typename T>
inline T clamp(T val, T lower, T upper)
{
         if (val < lower) return lower;
    else if (val > upper) return upper;
    else return val;
}

// Neato singleton helper function.  Chad Austin's idea.
template <typename T>
inline T* the()
{
    return T::GetInstance();
}

// Simple little predicate for for_each
template <class T>
struct Destroy {    void operator()(T* t) { delete t; } };

bool IsPowerOf2(uint i);
uint NextPowerOf2(uint i);

int sgn(int x);
void SeedRandom();
int Random(int min, int max);
char* va(char* format, ...);

const std::string Trim(const std::string& s);

std::string Lower(const std::string& s);
std::string Upper(const std::string& s);

std::string ToString(int i);
uint hexToInt(const std::string& s);

template <class T>
class ScopedPtr
{
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
  
    ScopedPtr& operator = (T* t)
    {
        delete _data;
        _data = t;
        return *this;
    }
};

template <class T>
class ScopedArray
{
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
  
    ScopedArray& operator = (T* t)
    {
        delete[] _data;
        _data = t;
        return *this;
    }
};

namespace Path
{
    // ifdef and blah blah for platform independance
    static const char cDelimiter='\\';


    // strips the path from the filename, and returns it.
    // If relativeto is not specified or is empty, then the entire path is returned.
    // If it is specified, then the path returned is relative to the path contained therein.
    std::string Directory(const std::string& s, const std::string& relativeto=""); 

    std::string Filename(const std::string& s);           // same idea, but just returns the filename

    std::string Extension(const std::string& s);          // returns the extension. (excluding the period)
    std::string ReplaceExtension(const std::string& s, const std::string& extension); // swaps the extension, and returns the result

    bool Compare(const std::string& s, const std::string& t); // returns true if the two paths are the same.  Disregards case in win32.
};

#endif
